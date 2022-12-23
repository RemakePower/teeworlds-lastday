#ifndef GAME_SERVER_LASTDAY_ITEM_CORE_H
#define GAME_SERVER_LASTDAY_ITEM_CORE_H

#include "ItemData.h"

class CItemCore
{
	class CGameContext *m_pGameServer;
	CGameContext *GameServer() const { return m_pGameServer; }

	void ReturnItem(CItemData Item, int ClientID);
	bool FindItem(const char *pMakeItem, CItemData *ItemInfo);
	void ShowNeed(CItemData ItemInfo, int ClientID);

public:
    CItemCore(CGameContext *pGameServer);
	void MakeItem(const char *pMakeItem, int ClientID);
	void ShowNeed(const char *pMakeItem, int ClientID);
	void ShowMakeList(int ClientID);
};

#endif