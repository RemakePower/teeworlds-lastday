#include "gamecontext.h"
#include "gamemenu.h"

CMenu::CMenu(CGameContext *pGameServer) :
    m_pGameServer(pGameServer)
{
    str_copy(m_aLanguageCode, "en", sizeof(m_aLanguageCode));
}

const char* CMenu::Localize(const char* pText) const
{
	return GameServer()->Localize(m_aLanguageCode, pText);
}

void CMenu::GetData(int Page)
{
    m_DataTemp.clear();

    for(int i = 0;i < m_apOptions.size();i ++)
    {
        if(m_apOptions[i]->m_Page == Page)
            m_DataTemp.add(m_apOptions[i]->m_pName);
        else if(Page != MENUPAGE_MAIN && m_apOptions[i]->m_Page == MENUPAGE_NOTMAIN)
            m_DataTemp.add(m_apOptions[i]->m_pName);
    }
}

int CMenu::FindOption(const char *pName, int Pages)
{
	for(int i = 0;i < m_apOptions.size();i ++)
	{
        if(str_comp_nocase(m_apOptions[i]->m_pName, pName) == 0)
        {
            if(m_apOptions[i]->m_Page == Pages)
                return i;
            else if(Pages != MENUPAGE_MAIN && m_apOptions[i]->m_Page == MENUPAGE_NOTMAIN)
                return i;
            
        }
	}

	return -1;
}

void CMenu::Register(const char *pName, int Pages, MenuCallback pfnFunc, void *pUser, bool CloseMenu)
{
	int OptionID = FindOption(pName, Pages);
    if(OptionID > -1)
        return;

	COptions *pOption = new(mem_alloc(sizeof(COptions), sizeof(void*))) COptions;
	
	pOption->m_pfnCallback = pfnFunc;

	pOption->m_pName = pName;
	pOption->m_Page = Pages;
    pOption->m_pUserData = pUser;
    pOption->m_CloseMenu = CloseMenu;

    m_apOptions.add(pOption);
}

void CMenu::ShowMenu(int ClientID, int Line)
{
    CPlayer *pPlayer = GameServer()->m_apPlayers[ClientID];

    if(!pPlayer)
        return;
    
    str_copy(m_aLanguageCode, pPlayer->GetLanguage(), sizeof(m_aLanguageCode));

    std::string MenuBuffer;
    int Page;

    Page = pPlayer->GetMenuPage();

    GetData(Page);
    if(!m_DataTemp.size())
        return;
        
    MenuBuffer.append("===");
    MenuBuffer.append(Localize("Menu"));
    MenuBuffer.append("===");
    MenuBuffer.append("\n");
    while (Line < 0)
    {
        Line += m_DataTemp.size();
    }

    while (Line >= m_DataTemp.size())
    {
        Line -= m_DataTemp.size();
    }


    if(m_DataTemp.size() > 6)
    {
        int j = Line;
        for(int i = 0;i < 6; i++)
        {
            while (j < 0)
            {
                j += 6;
            }

            while (j >= m_DataTemp.size())
            {
                j -= 6;
            }

            const char* Buffer = m_DataTemp[j].c_str();
            MenuBuffer.append(Buffer);
            MenuBuffer.append("\n");

            j++;
        }
    }
    else
    {

        int j = 0;
        for(int i = 0;i < m_DataTemp.size(); i++)
        {
            const char* Buffer = m_DataTemp[j].c_str();
            if(j == Line)
                MenuBuffer.append("[");
            MenuBuffer.append(Localize(Buffer));
            if(j == Line)
                MenuBuffer.append("]");
            MenuBuffer.append("\n");

            if(j == Line)
            {
                pPlayer->m_SelectOption = Buffer;
            }

            j++;
        }
    }
        
    MenuBuffer.append(Localize("Use <mouse1>(Fire) to use option"));

    GameServer()->SendBroadcast(MenuBuffer.c_str(), ClientID);

}

void CMenu::UseOptions(int ClientID)
{
    CPlayer *pPlayer = GameServer()->m_apPlayers[ClientID];

    if(!pPlayer)
        return;
    
    int OptionID = FindOption(pPlayer->m_SelectOption, pPlayer->GetMenuPage());

    dbg_assert(OptionID > -1 && OptionID < m_apOptions.size(), "no this option, but a player use it");
    if(m_apOptions[OptionID]->m_CloseMenu)
        pPlayer->CloseMenu();
    m_apOptions[OptionID]->m_pfnCallback(ClientID, m_apOptions[OptionID]->m_pUserData);
}
