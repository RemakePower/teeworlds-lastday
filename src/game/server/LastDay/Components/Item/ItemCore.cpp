
#include <engine/external/json-parser/json.h>
#include <game/server/gamecontext.h>
#include <game/server/define.h>
#include "ItemCore.h"

CItemCore::CItemCore(CGameContext *pGameServer)
{
	m_pGameServer = pGameServer;
}

// Public Make
void CItemCore::MakeItem(const char *pMakeItem, int ClientID)
{
	CItemData ItemInfo;
	if(!FindItem(pMakeItem, &ItemInfo))
	{
		GameServer()->SendChatTarget_Locazition(ClientID, _("No this item!"));
		ShowMakeList(ClientID);
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
		ShowNeed(ItemInfo, ClientID);
		return;
	}

	CanMake=true;
	if(ItemInfo.m_GiveType == ITEMTYPE_WEAPON)
	{
		CCharacter *pChr = pPlayer->GetCharacter();

		switch (ItemInfo.m_GiveID)
		{
			case ITEM_SHOTGUN: if(pChr->GetWeaponStat()[WEAPON_SHOTGUN].m_Got) CanMake = false;break;
			case ITEM_GRENADE: if(pChr->GetWeaponStat()[WEAPON_GRENADE].m_Got) CanMake = false;break;
			case ITEM_RIFLE: if(pChr->GetWeaponStat()[WEAPON_RIFLE].m_Got) CanMake = false;break;
		}
	}

	if(!CanMake)
	{
		GameServer()->SendChatTarget_Locazition(ClientID, _("You had %s"), 
			GameServer()->Localize(pLanguageCode, pMakeItem));
		return;
	}

	GameServer()->SendChatTarget_Locazition(ClientID, _("Making %s..."), 
		GameServer()->Localize(pLanguageCode, pMakeItem));

	ReturnItem(ItemInfo, ClientID);
	
}

void CItemCore::ReturnItem(CItemData Item, int ClientID)
{
	CPlayer *pPlayer = GameServer()->m_apPlayers[ClientID];
	if(!pPlayer)
		return;


	const char *pLanguageCode = pPlayer->GetLanguage();

	if(Item.m_GiveType == ITEMTYPE_AMMO)
	{
		CCharacter *pChr = pPlayer->GetCharacter();
		if(!pChr)
			return;
		switch (Item.m_GiveID)
		{
			case ITEM_GUN_AMMO: pChr->GetWeaponStat()[TWS_WEAPON_GUN].m_Ammo += Item.m_GiveNum;break;
			case ITEM_SHOTGUN_AMMO: pChr->GetWeaponStat()[TWS_WEAPON_SHOTGUN].m_Ammo += Item.m_GiveNum;break;
			case ITEM_GRENADE_AMMO: pChr->GetWeaponStat()[TWS_WEAPON_GRENADE].m_Ammo += Item.m_GiveNum;break;
			case ITEM_RIFLE_AMMO: pChr->GetWeaponStat()[TWS_WEAPON_RIFLE].m_Ammo += Item.m_GiveNum;break;
		}
	}
	else if(Item.m_GiveType == ITEMTYPE_WEAPON)
	{
		CCharacter *pChr = pPlayer->GetCharacter();
		if(!pChr)
			return;

		switch (Item.m_GiveID)
		{
			case ITEM_GUN: pChr->GetWeaponStat()[TWS_WEAPON_GUN].m_Got = true;break;
			case ITEM_SHOTGUN: pChr->GetWeaponStat()[TWS_WEAPON_SHOTGUN].m_Got = true;break;
			case ITEM_GRENADE: pChr->GetWeaponStat()[TWS_WEAPON_GRENADE].m_Got = true;break;
			case ITEM_RIFLE: pChr->GetWeaponStat()[TWS_WEAPON_RIFLE].m_Got = true;break;
		}	
	}

	if(Item.m_GiveNum)
		GameServer()->SendChatTarget_Locazition(ClientID, _("Make finish, you get %d %s"), 
			Item.m_GiveNum, Item.m_aName);
	else GameServer()->SendChatTarget_Locazition(ClientID, _("Make finish, you get %s"), 
			Item.m_aName);
	
	for(int i = 0; i < NUM_RESOURCES;i ++)
	{
		int Need = Item.m_NeedResource.GetResource(i);
		int Have = pPlayer->m_Resource.GetResource(i);
		pPlayer->m_Resource.SetResource(i, Have - Need);
	}
}

// Find Item, if it in the json.return the FOUND(and the Item info)
bool CItemCore::FindItem(const char *pMakeItem, CItemData *ItemInfo)
{
	// read file data into buffer
	const char *pFilename = "./data/json/make.json";
	IOHANDLE File = GameServer()->Storage()->OpenFile(pFilename, IOFLAG_READ, IStorage::TYPE_ALL);
	if(!File)
	{
		dbg_msg("Item", "can't open 'data/json/make.json'");
		return false;
	}
	
	int FileSize = (int)io_length(File);
	char *pFileData = new char[FileSize+1];
	io_read(File, pFileData, FileSize);
	pFileData[FileSize] = 0;
	io_close(File);

	// parse json data
	json_settings JsonSettings;
	mem_zero(&JsonSettings, sizeof(JsonSettings));
	char aError[256];
	json_value *pJsonData = json_parse_ex(&JsonSettings, pFileData, aError);
	if(pJsonData == 0)
	{
		delete[] pFileData;
		return false;
	}

	const json_value &rStart = (*pJsonData)["json"];
	bool Found = false;
	if(rStart.type == json_array)
	{
		for(unsigned i = 0; i < rStart.u.array.length; ++i)
		{
			if(str_comp((const char *)rStart[i]["name"], pMakeItem) == 0)
			{
				str_copy(ItemInfo->m_aName, (const char *)rStart[i]["name"], sizeof(ItemInfo->m_aName));
				ItemInfo->m_GiveType = (long)rStart[i]["give_type"];
				ItemInfo->m_GiveID = (long)rStart[i]["give_id"];
				ItemInfo->m_GiveNum = (long)rStart[i]["give_num"];
				ItemInfo->m_NeedResource.m_Metal = (long)rStart[i]["metal"];
				ItemInfo->m_NeedResource.m_Wood = (long)rStart[i]["wood"];
				Found = true;
				break;
			}
		}
	}

	// clean up
	json_value_free(pJsonData);
	delete[] pFileData;

	return Found;
}

void CItemCore::ShowNeed(CItemData ItemInfo, int ClientID)
{
	CPlayer *pPlayer = GameServer()->m_apPlayers[ClientID];
	if(!pPlayer)
		return;


	const char *pLanguageCode = pPlayer->GetLanguage();

	std::string Buffer;
	Buffer.clear();
	
	bool First=true;	
	for(int i = 0; i < NUM_RESOURCES;i ++)
	{
		if(ItemInfo.m_NeedResource.GetResource(i))
		{
			if(!First)
				Buffer.append(", ");
			else First = false;
			Buffer.append(std::to_string(ItemInfo.m_NeedResource.GetResource(i)));
			Buffer.append(" ");
			Buffer.append(GameServer()->Localize(pLanguageCode, GetResourceName(i)));
		}
	}

	GameServer()->SendChatTarget_Locazition(ClientID, _("%s requires %s."), ItemInfo.m_aName, Buffer.c_str());
}

// Public
void CItemCore::ShowNeed(const char *pMakeItem, int ClientID)
{
	CItemData ItemInfo;
	if(!FindItem(pMakeItem, &ItemInfo))
	{
		GameServer()->SendChatTarget_Locazition(ClientID, _("No this item!"));
		return;
	}

	ShowNeed(ItemInfo, ClientID);
}

void CItemCore::ShowMakeList(int ClientID)
{
	// read file data into buffer
	const char *pFilename = "./data/json/make.json";
	IOHANDLE File = GameServer()->Storage()->OpenFile(pFilename, IOFLAG_READ, IStorage::TYPE_ALL);
	if(!File)
	{
		dbg_msg("Item", "can't open 'data/json/make.json'");
		return;
	}
	
	int FileSize = (int)io_length(File);
	char *pFileData = new char[FileSize+1];
	io_read(File, pFileData, FileSize);
	pFileData[FileSize] = 0;
	io_close(File);

	// parse json data
	json_settings JsonSettings;
	mem_zero(&JsonSettings, sizeof(JsonSettings));
	char aError[256];
	json_value *pJsonData = json_parse_ex(&JsonSettings, pFileData, aError);
	if(pJsonData == 0)
	{
		delete[] pFileData;
		return;
	}

	const json_value &rStart = (*pJsonData)["json"];
	bool Found = false;

	std::string Buffer;
	if(rStart.type == json_array)
	{
		bool First = true;
		for(unsigned i = 0; i < rStart.u.array.length; ++i)
		{
			if(!First)
				Buffer.append(", ");
			else First = false;
			Buffer.append((const char *)rStart[i]["name"]);
		}
	}

	dbg_msg("make", Buffer.c_str());

	// clean up
	json_value_free(pJsonData);
	delete[] pFileData;

	GameServer()->SendChatTarget_Locazition(ClientID, _("You can make: %s"), Buffer.c_str());

	return;
}