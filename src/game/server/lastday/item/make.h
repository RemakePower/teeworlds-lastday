#ifndef GAME_SERVER_LASTDAY_ITEM_MAKE_H
#define GAME_SERVER_LASTDAY_ITEM_MAKE_H

#include "resource.h"

class CItemMake
{
	class CGameContext *m_pGameServer;
	CGameContext *GameServer() const { return m_pGameServer; }

	class CItemData
	{
	public:
		CItemData();
		char m_aName[64];
		int m_GiveType;
		int m_GiveID;
		int m_GiveNum;
		Resource m_NeedResource;
	};

	void ReturnItem(CItemData Item, int ClientID);
	bool FindItem(const char *pMakeItem, CItemData *ItemInfo);
	void ShowNeed(CItemData ItemInfo, int ClientID);

public:
    CItemMake(CGameContext *pGameServer);
	void MakeItem(const char *pMakeItem, int ClientID);
	void ShowNeed(const char *pMakeItem, int ClientID);
	void ShowMakeList(int ClientID);
};

#endif