#ifndef GAME_SERVER_LASTDAY_ITEM_MAKE_H
#define GAME_SERVER_LASTDAY_ITEM_MAKE_H

#include <base/tl/array.h>
#include "resource.h"

class CItemData
{
public:
	CItemData();
	char m_aName[64];
	int m_GiveID;
	int m_GiveNum;
	Resource m_NeedResource;
};

class CItemMake
{
	class CGameContext *m_pGameServer;
	CGameContext *GameServer() const { return m_pGameServer; }

	array<CItemData*> m_apDatas;
	void ReturnItem(CItemData Item, int ClientID);
	bool InitItem();

public:
    CItemMake(CGameContext *pGameServer);
	void MakeItem(const char *pMakeItem, int ClientID);
	bool FindItem(const char *pName, CItemData *pData);
	Resource *GetNeed(const char *pMakeItem, int ClientID);
};

#endif