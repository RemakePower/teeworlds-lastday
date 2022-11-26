/* (c) GutZuFusss. See licence.txt in the root of the distribution for more information.     */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */
#include "db_sqlite3.h"
#include <game/server/gamecontext.h>
#include <engine/server/crypt.h>

bool CQuery::Next()
{
	/*CALL_STACK_ADD();*/

    int Ret = sqlite3_step(m_pStatement);
    return Ret == SQLITE_ROW;
}

bool CQuery::Busy()
{
	/*CALL_STACK_ADD();*/

    int Ret = sqlite3_step(m_pStatement);
    return Ret == SQLITE_BUSY;
}

void CQuery::Query(CSql *pDatabase, char *pQuery)
{
	/*CALL_STACK_ADD();*/

    m_pDatabase = pDatabase;
    m_pDatabase->Query(this, pQuery);
}
void CQuery::OnData()
{
	/*CALL_STACK_ADD();*/

    Next();
}
int CQuery::GetID(const char *pName)
{
	/*CALL_STACK_ADD();*/

    for (int i = 0; i < GetColumnCount(); i++)
    {
        if (str_comp(GetName(i), pName) == 0)
            return i;
    }
    return -1;
}

CQuery::~CQuery()
{
}

void CSql::WorkerThread()
{
	/*CALL_STACK_ADD();*/

    while(m_Running)
    {
        lock_wait(m_Lock); //lock queue
        if (m_lpQueries.size() > 0)
        {
            CQuery *pQuery = m_lpQueries.front();
            m_lpQueries.pop();
            lock_unlock(m_Lock); //unlock queue

            int Ret;
            Ret = sqlite3_prepare_v2(m_pDB, pQuery->m_Query.c_str(), -1, &pQuery->m_pStatement, 0);
            if (Ret == SQLITE_OK)
            {
                if (!m_Running) //last check
                    break;
                pQuery->OnData();

                sqlite3_finalize(pQuery->m_pStatement);
            }
            else
                dbg_msg("SQLite", "%s", sqlite3_errmsg(m_pDB));

            delete pQuery;
        }
        else
        {
            thread_sleep(100);
            lock_unlock(m_Lock); //unlock queue
        }

        thread_sleep(10);
    }
}

void CSql::InitWorker(void *pUser)
{
	/*CALL_STACK_ADD();*/

    CSql *pSelf = (CSql *)pUser;
    pSelf->WorkerThread();
}

CQuery *CSql::Query(CQuery *pQuery, std::string QueryString)
{
	/*CALL_STACK_ADD();*/

    pQuery->m_Query = QueryString;


    lock_wait(m_Lock);
    m_lpQueries.push(pQuery);
    lock_unlock(m_Lock);

    return pQuery;
}

CSql::CSql()
{
	/*CALL_STACK_ADD();*/

    int rc = sqlite3_open("lastday-accounts.db", &m_pDB);
    if (rc)
    {
        dbg_msg("SQLite", "can't open database");
        sqlite3_close(m_pDB);
    }

	char *pQuery = (char *)"CREATE TABLE IF NOT EXISTS Accounts (" \
		"ID                         INTEGER 		PRIMARY KEY		AUTOINCREMENT," \
        "Username                   VARCHAR(32)     NOT NULL," \
        "Password                   VARCHAR(32)     NOT NULL," \
        "Language                   VARCHAR(16)     NOT NULL," \
		"Metal                      BIGINT          DEFAULT 0," \
        "Wood                       BIGINT          DEFAULT 0);";

    char *pErrorMsg;
    sqlite3_exec(m_pDB, pQuery, NULL, NULL, &pErrorMsg);
    if(pErrorMsg)
        dbg_msg("SQLite3","Error msg: %s",pErrorMsg);

    m_Lock = lock_create();
    m_Running = true;
    thread_init(InitWorker, this);
    dbg_msg("SQLite", "create table");
}

CSql::~CSql()
{
	/*CALL_STACK_ADD();*/

    m_Running = false;
    lock_wait(m_Lock);
    while (m_lpQueries.size())
    {
        CQuery *pQuery = m_lpQueries.front();
        m_lpQueries.pop();
        delete pQuery;
    }
    lock_unlock(m_Lock);
    lock_destroy(m_Lock);
}

bool CSql::Register(const char *Username, const char *Password, const char *Language, int ClientID)
{
    lock_wait(m_Lock);

    char pQuery[555];
    char aHash[64];
	Crypt(Password, (const unsigned char*) "d9", 1, 14, aHash);

    str_format(pQuery, sizeof(pQuery), (char *)"INSERT INTO Accounts(" \
		"Username, " \
        "Password, Language) " \
        "VALUES ('%s', '%s', '%s');", 
        Username, aHash, Language);

    char *pErrorMsg;
    sqlite3_exec(m_pDB, pQuery, NULL, NULL, &pErrorMsg);

    if(pErrorMsg)
        dbg_msg("SQLite3","Register error msg: %s",pErrorMsg);

    lock_unlock(m_Lock);
    return true;
}

bool CSql::Login(const char *Username, const char *Password, int ClientID)
{
    char pQuery[555];

    str_format(pQuery, sizeof(pQuery), (char *)"SELECT * FROM Accounts WHERE " \
		"Username='%s' AND " \
        "Password='%s';", 
        Username, Password);

    char *pErrorMsg;
    lock_wait(m_Lock);
    sqlite3_exec(m_pDB, pQuery, NULL, NULL, &pErrorMsg);
    lock_unlock(m_Lock);
    if(pErrorMsg)
    {
        dbg_msg("SQLite3","Login error msg: %s",pErrorMsg);
        return false;
    }
    return true;
}

bool CSql::Apply(const char *Username, const char *Password, const char *Language, 
                int AccID, int64 Metal, int64 Wood)
{
    char pQuery[300];
    
    char* aBuf = (char *)"UPDATE Accounts SET Language='%s',Metal=%lld,Wood=%lld WHERE ID=%d;";

    str_format(pQuery, sizeof(pQuery), aBuf, Language, Metal, Wood, AccID);

    int nRet=0;
    char *pErrorMsg;

    sqlite3_exec(m_pDB, pQuery, NULL, NULL, &pErrorMsg);
    m_Running = true;
    thread_init(InitWorker, this);
    if(pErrorMsg)
        dbg_msg("SQLite3","Apply error msg: %s",pErrorMsg);
	dbg_msg("SQLite", "Apply, Language='%s',Metal=%lld,Wood=%lld WHERE ID=%d", Language, Metal, Wood, AccID);
    return true;
}