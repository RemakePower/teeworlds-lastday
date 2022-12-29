#include <engine/external/nlohmann/json.hpp>
#include <game/server/gamecontext.h>
#include <game/server/define.h>
#include "make.h"

#include <fstream>

using json = nlohmann::json;

CItemData::CItemData()
{
	m_GiveID = 0;
	m_GiveNum = 0;
	m_NeedResource.ResetResource();
}

CItemMake::CItemMake(CGameContext *pGameServer)
{
	m_pGameServer = pGameServer;
	m_apDatas.clear();
	InitItem();
}

// Public Make
void CItemMake::MakeItem(const char *pMakeItem, int ClientID)
{
	CItemData ItemInfo;
	if(!FindItem(pMakeItem, &ItemInfo))
	{
		dbg_msg(pMakeItem, pMakeItem);
		GameServer()->SendMenuChat_Locazition(ClientID, _("No this item!"));
		return;
	}
	
	CPlayer *pPlayer = GameServer()->m_apPlayers[ClientID];

	if(!pPlayer)
		return;

	if(!pPlayer->GetCharacter())
		return;

	const char *pLanguageCode = pPlayer->GetLanguage();

	Resource *pPResource = &pPlayer->m_Resource;
	bool CanMake = true;
	for(int i = 0; i < NUM_RESOURCES;i ++)
	{
		if(pPResource->GetResource(i) < ItemInfo.m_NeedResource.GetResource(i))
		{
			CanMake = false;
			break;
		}
	}

	//dbg_msg("item", "make %s, need %d metal and %d wood, this player have %d metal, and %d wood", pMakeItem,ItemInfo.m_NeedResource.m_Metal, ItemInfo.m_NeedResource.m_Wood, pPResource->m_Metal, pPResource->m_Wood);

	if(!CanMake)
	{
		GameServer()->SendMenuChat_Locazition(ClientID, "You don't have enough resources");
		return;
	}

	CanMake=true;
	CCharacter *pChr = pPlayer->GetCharacter();

	switch (ItemInfo.m_GiveID)
	{
		case ITEM_GUN: if(pChr->GetWeaponStat()[LD_WEAPON_GUN].m_Got) CanMake = false;break;
		case ITEM_SHOTGUN: if(pChr->GetWeaponStat()[LD_WEAPON_SHOTGUN].m_Got) CanMake = false;break;
		case ITEM_GRENADE: if(pChr->GetWeaponStat()[LD_WEAPON_GRENADE].m_Got) CanMake = false;break;
		case ITEM_RIFLE: if(pChr->GetWeaponStat()[LD_WEAPON_RIFLE].m_Got) CanMake = false;break;
		case ITEM_NINJA: if(pChr->GetWeaponStat()[LD_WEAPON_NINJA].m_Got) CanMake = false;break;
	}
	

	if(!CanMake)
	{
		GameServer()->SendMenuChat_Locazition(ClientID, _("You had %s"), 
			GameServer()->Localize(pLanguageCode, pMakeItem));
		return;
	}

	GameServer()->SendMenuChat_Locazition(ClientID, _("Making %s..."), 
		GameServer()->Localize(pLanguageCode, pMakeItem));

	ReturnItem(ItemInfo, ClientID);
	
}

void CItemMake::ReturnItem(CItemData Item, int ClientID)
{
	CPlayer *pPlayer = GameServer()->m_apPlayers[ClientID];
	if(!pPlayer)
		return;


	const char *pLanguageCode = pPlayer->GetLanguage();
	
	CCharacter *pChr = pPlayer->GetCharacter();
	if(!pChr)
		return;

	switch (Item.m_GiveID)
	{
		case ITEM_GUN_AMMO: pChr->GetWeaponStat()[LD_WEAPON_GUN].m_Ammo += Item.m_GiveNum;break;
		case ITEM_SHOTGUN_AMMO: pChr->GetWeaponStat()[LD_WEAPON_SHOTGUN].m_Ammo += Item.m_GiveNum;break;
		case ITEM_GRENADE_AMMO: pChr->GetWeaponStat()[LD_WEAPON_GRENADE].m_Ammo += Item.m_GiveNum;break;
		case ITEM_RIFLE_AMMO: pChr->GetWeaponStat()[LD_WEAPON_RIFLE].m_Ammo += Item.m_GiveNum;break;
		case ITEM_GUN: pChr->GetWeaponStat()[LD_WEAPON_GUN].m_Got = true;break;
		case ITEM_SHOTGUN: pChr->GetWeaponStat()[LD_WEAPON_SHOTGUN].m_Got = true;break;
		case ITEM_GRENADE: pChr->GetWeaponStat()[LD_WEAPON_GRENADE].m_Got = true;break;
		case ITEM_RIFLE: pChr->GetWeaponStat()[LD_WEAPON_RIFLE].m_Got = true;break;
		case ITEM_NINJA: pChr->GiveNinja();break;
	}	

	
	for(int i = 0; i < NUM_RESOURCES;i ++)
	{
		int Need = Item.m_NeedResource.GetResource(i);
		int Have = pPlayer->m_Resource.GetResource(i);
		pPlayer->m_Resource.SetResource(i, Have - Need);
	}

	if(Item.m_GiveNum)
		GameServer()->SendMenuChat_Locazition(ClientID, _("Make finish, you get %d %s"), 
			Item.m_GiveNum, Item.m_aName);
	else GameServer()->SendMenuChat_Locazition(ClientID, _("Make finish, you get %s"), 
			Item.m_aName);
}

// Find Item, if it in the json.return the FOUND(and the Item info)
bool CItemMake::InitItem()
{
	// read file data into buffer
	const char *pFilename = "./data/json/make.json";
	std::ifstream File(pFilename);

	if(!File.is_open())
	{
		dbg_msg("Item", "can't open 'data/json/make.json'");
		return false;
	}

	// parse json data
	json Data = json::parse(File);

	json rStart = Data["json"];
	if(rStart.is_array())
	{
		for(unsigned i = 0; i < rStart.size(); ++i)
		{
			CItemData *pData = new CItemData();
			str_copy(pData->m_aName, rStart[i].value("name", " ").c_str());
			pData->m_GiveID = rStart[i].value("give_id", -1);
			pData->m_GiveNum = rStart[i].value("give_num", 0);
			pData->m_NeedResource.m_Metal = rStart[i].value("metal", 0);
			pData->m_NeedResource.m_Wood = rStart[i].value("wood", 0);
			m_apDatas.add(pData);
			GameServer()->Menu()->RegisterMake(pData->m_aName);
		}
	}

	return true;
}

bool CItemMake::FindItem(const char *pName, CItemData *pData)
{
	bool Found = false;
	for(int i = 0;i < m_apDatas.size();i ++)
	{
		if(str_comp(m_apDatas[i]->m_aName, pName) == 0)
		{
			str_copy(pData->m_aName, m_apDatas[i]->m_aName);
			pData->m_GiveID = m_apDatas[i]->m_GiveID;
			pData->m_GiveNum = m_apDatas[i]->m_GiveNum;
			pData->m_NeedResource = m_apDatas[i]->m_NeedResource;
			Found = true;
			break;
		}
	}
	return Found;
}

// Public
Resource *CItemMake::GetNeed(const char *pMakeItem, int ClientID)
{
	CItemData ItemInfo;
	if(FindItem(pMakeItem, &ItemInfo))
	{
		GameServer()->SendMenuChat_Locazition(ClientID, _("No this item!"));
		return 0;
	}

	return &ItemInfo.m_NeedResource;
}

