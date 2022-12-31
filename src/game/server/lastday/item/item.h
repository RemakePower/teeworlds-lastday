#ifndef GAME_SERVER_LASTDAY_ITEM_H
#define GAME_SERVER_LASTDAY_ITEM_H

#include "item-data.h"

class CItemCore
{
    friend class CMakeCore;

    class CGameContext *m_pGameServer;
	CGameContext *GameServer() const { return m_pGameServer; }

    class CMakeCore *m_pMake;

	array<CItemData> m_aItems;
    CInventoryData m_aInventories[MAX_CLIENTS];

    void InitItem();
public:
    CItemCore(CGameContext *pGameServer);
    CMakeCore *Make() const {return m_pMake;}
    
    array<CItemData*> m_aDrops;

    CItemData *GetItemData(const char* Name);
    CInventoryData *GetInventory(int ClientID);
    int GetInvItemNum(const char *ItemName, int ClientID);
    void AddInvItemNum(const char *ItemName, int Num, int ClientID);
    void SetInvItemNum(const char *ItemName, int Num, int ClientID);
};

#endif