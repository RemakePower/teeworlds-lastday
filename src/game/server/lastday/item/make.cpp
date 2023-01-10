#include <engine/external/nlohmann/json.hpp>
#include <game/server/gamecontext.h>
#include <game/server/define.h>
#include "make.h"
#include "item.h"
#include "item-data.h"


#include <fstream>


CMakeCore::CMakeCore(CItemCore *pItem)
{
	m_pParent = pItem;
}

CGameContext *CMakeCore::GameServer() const
{
	return m_pParent->GameServer();
}

// Public Make
void CMakeCore::MakeItem(const char *pMakeItem, int ClientID)
{
	CItemData *pItemInfo = m_pParent->GetItemData(pMakeItem);
	if(!pItemInfo)
	{
		GameServer()->SendMenuChat_Locazition(ClientID, _("No this item!"));
		return;
	}
	
	CPlayer *pPlayer = GameServer()->m_apPlayers[ClientID];

	if(!pPlayer)
		return;

	if(!pPlayer->GetCharacter())
		return;

	const char *pLanguageCode = pPlayer->GetLanguage();

	bool Makeable = true;
	for(int i = 0; i < pItemInfo->m_Needs.m_Name.size();i ++)
	{
		if(m_pParent->GetInvItemNum(pItemInfo->m_Needs.m_Name[i].c_str(), ClientID) < pItemInfo->m_Needs.m_Num[i])
		{
			Makeable = false;
			break;
		}
	}

	//dbg_msg("item", "make %s, need %d metal and %d wood, this player have %d metal, and %d wood", pMakeItem,ItemInfo.m_NeedResource.m_Metal, ItemInfo.m_NeedResource.m_Wood, pPResource->m_Metal, pPResource->m_Wood);

	if(!Makeable)
	{
		GameServer()->SendMenuChat_Locazition(ClientID, "You don't have enough resources");
		return;
	}

	Makeable=true;

	if(pItemInfo->m_WeaponID > -1)
	{
		if(m_pParent->GetInvItemNum(pItemInfo->m_aName, ClientID))
		{
			Makeable = false;
		}
	}
	

	if(!Makeable)
	{
		GameServer()->SendMenuChat_Locazition(ClientID, _("You had %s"), pMakeItem);
		return;
	}

	GameServer()->SendMenuChat_Locazition(ClientID, _("Making %s..."), pMakeItem);

	ReturnItem(pItemInfo, ClientID);
	
}

void CMakeCore::ReturnItem(class CItemData *Item, int ClientID)
{
	CPlayer *pPlayer = GameServer()->m_apPlayers[ClientID];
	if(!pPlayer)
		return;


	const char *pLanguageCode = pPlayer->GetLanguage();

	m_pParent->AddInvItemNum(Item->m_aName, Item->m_GiveNum, ClientID);
	
	for(int i = 0; i < Item->m_Needs.m_Name.size();i ++)
	{
		m_pParent->AddInvItemNum(Item->m_Needs.m_Name[i].c_str(), -Item->m_Needs.m_Num[i], ClientID);
	}

	if(Item->m_GiveNum > 1)
		GameServer()->SendMenuChat_Locazition(ClientID, _("Make finish, you get %d %s"), 
			Item->m_GiveNum, Item->m_aName);
	else GameServer()->SendMenuChat_Locazition(ClientID, _("Make finish, you get %s"), 
			Item->m_aName);
}
