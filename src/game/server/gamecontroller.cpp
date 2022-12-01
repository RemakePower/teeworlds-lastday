/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */
#include <engine/shared/config.h>
#include <game/mapitems.h>
#include <game/version.h>

#include <game/generated/protocol.h>

#include <engine/external/json-parser/json.h>

#include "entities/pickup.h"

#include "gamecontroller.h"
#include "gamecontext.h"

#include <string.h>


CGameController::CGameController(class CGameContext *pGameServer)
{
	m_pGameServer = pGameServer;
	m_pServer = m_pGameServer->Server();
	m_pGameType = MOD_NAME;

	//
	m_UnpauseTimer = 0;
	m_GameOverTick = -1;
	m_SuddenDeath = 0;
	m_RoundStartTick = Server()->Tick();
	m_RoundCount = 0;
	m_GameFlags = 0;
	m_aMapWish[0] = 0;

	m_UnbalancedTick = -1;
	m_ForceBalanced = false;
	
	m_SpawnPoints.clear();
	
	WeaponIniter.InitWeapons(pGameServer);
}

CGameController::~CGameController()
{
}

vec2 CGameController::GetSpawnPos()
{
	vec2 Pos;
	for(int i = 0;i < m_SpawnPoints.size(); i++)
	{
		Pos = m_SpawnPoints[random_int(0, m_SpawnPoints.size()-1)];
		if(!GameServer()->m_World.ClosestCharacter(Pos, 64.0f, 0x0))
			break;
	}
	return Pos;
}

void CGameController::InitSpawnPos()
{
	// create all entities from the game layer
	CMapItemLayerTilemap *pTileMap = GameServer()->Layers()->GameLayer();
	CTile *pTiles = GameServer()->m_pTiles;

	for(int y = 0; y < pTileMap->m_Height; y++)
	{
		for(int x = 0; x < pTileMap->m_Width; x++)
		{
			int Index = pTiles[y*pTileMap->m_Width+x].m_Index;

			if(Index&CCollision::COLFLAG_SOLID || Index&CCollision::COLFLAG_DEATH) continue;
			int GroudIndex = pTiles[(y+1)*pTileMap->m_Width+x].m_Index;
			if(GroudIndex&CCollision::COLFLAG_SOLID)
			{
				vec2 Pos(x*32.0f+16.0f, y*32.0f+16.0f);
				m_SpawnPoints.add(Pos);
			}
		}
	}
}

bool CGameController::OnEntity(int Index, vec2 Pos)
{
	int Type = -1;
	int SubType = 0;
	return false;
}

void CGameController::EndRound()
{
	GameServer()->m_World.m_Paused = true;
	m_GameOverTick = Server()->Tick();
	m_SuddenDeath = 0;
}

void CGameController::ResetGame()
{
	GameServer()->m_World.m_ResetRequested = true;
}

const char *CGameController::GetTeamName(int Team)
{
	if(IsTeamplay())
	{
		if(Team == TEAM_RED)
			return "red team";
		else if(Team == TEAM_BLUE)
			return "blue team";
	}
	else
	{
		if(Team == 0)
			return "game";
	}

	return "spectators";
}

static bool IsSeparator(char c) { return c == ';' || c == ' ' || c == ',' || c == '\t'; }

void CGameController::StartRound()
{
	ResetGame();

	m_RoundStartTick = Server()->Tick();
	m_SuddenDeath = 0;
	m_GameOverTick = -1;
	GameServer()->m_World.m_Paused = false;
	m_ForceBalanced = false;
	Server()->DemoRecorder_HandleAutoStart();
	char aBuf[256];
	str_format(aBuf, sizeof(aBuf), "start round type='%s' teamplay='%d'", m_pGameType, m_GameFlags&GAMEFLAG_TEAMS);
	GameServer()->Console()->Print(IConsole::OUTPUT_LEVEL_DEBUG, "game", aBuf);
}

void CGameController::ChangeMap(const char *pToMap)
{
	str_copy(m_aMapWish, pToMap, sizeof(m_aMapWish));
	EndRound();
}

void CGameController::CycleMap()
{
	if(m_aMapWish[0] == 0)
	{
		return;
	}

	char aBuf[256];
	str_format(aBuf, sizeof(aBuf), "rotating map to %s", m_aMapWish);
	GameServer()->Console()->Print(IConsole::OUTPUT_LEVEL_DEBUG, "game", aBuf);
	str_copy(g_Config.m_SvMap, m_aMapWish, sizeof(g_Config.m_SvMap));
	m_aMapWish[0] = 0;
	m_RoundCount = 0;
	return;
}

void CGameController::PostReset()
{
	for(int i = 0; i < MAX_CLIENTS; i++)
	{
		if(GameServer()->m_apPlayers[i])
		{
			GameServer()->m_apPlayers[i]->Respawn();
			GameServer()->m_apPlayers[i]->m_Score = 0;
			GameServer()->m_apPlayers[i]->m_ScoreStartTick = Server()->Tick();
			GameServer()->m_apPlayers[i]->m_RespawnTick = Server()->Tick()+Server()->TickSpeed()/2;
		}
	}
}

void CGameController::OnPlayerInfoChange(class CPlayer *pP)
{
	const int aTeamColors[2] = {65387, 10223467};
	if(IsTeamplay())
	{
		pP->m_TeeInfos.m_UseCustomColor = 1;
		if(pP->GetTeam() >= TEAM_RED && pP->GetTeam() <= TEAM_BLUE)
		{
			pP->m_TeeInfos.m_ColorBody = aTeamColors[pP->GetTeam()];
			pP->m_TeeInfos.m_ColorFeet = aTeamColors[pP->GetTeam()];
		}
		else
		{
			pP->m_TeeInfos.m_ColorBody = 12895054;
			pP->m_TeeInfos.m_ColorFeet = 12895054;
		}
	}
}


int CGameController::OnCharacterDeath(class CCharacter *pVictim, class CPlayer *pKiller, int Weapon)
{
	// do scoreing
	if(!pKiller || Weapon == WEAPON_GAME)
		return 0;
	if(pKiller == pVictim->GetPlayer())
		pVictim->GetPlayer()->m_Score--; // suicide
	else
	{
		if(IsTeamplay() && pVictim->GetPlayer()->GetTeam() == pKiller->GetTeam())
			pKiller->m_Score--; // teamkill
		else
			pKiller->m_Score++; // normal kill
	}
	if(Weapon == WEAPON_SELF)
		pVictim->GetPlayer()->m_RespawnTick = Server()->Tick()+Server()->TickSpeed()*3.0f;

	return 0;
}

void CGameController::OnCharacterSpawn(class CCharacter *pChr)
{
	// default health
	pChr->IncreaseHealth(10);

	// give default weapons
	pChr->GiveWeapon(WEAPON_HAMMER, -1);
}

void CGameController::TogglePause()
{
	if(IsGameOver())
		return;

	if(GameServer()->m_World.m_Paused)
	{
		GameServer()->m_World.m_Paused = false;
		m_UnpauseTimer = 0;
	}
	else
	{
		// pause
		GameServer()->m_World.m_Paused = true;
		m_UnpauseTimer = 0;
	}
}

bool CGameController::IsFriendlyFire(int ClientID1, int ClientID2)
{
	if(ClientID1 == ClientID2)
		return false;

	if(IsTeamplay())
	{
		if(!GameServer()->m_apPlayers[ClientID1] || !GameServer()->m_apPlayers[ClientID2])
			return false;

		if(GameServer()->m_apPlayers[ClientID1]->GetTeam() == GameServer()->m_apPlayers[ClientID2]->GetTeam())
			return true;
	}

	return false;
}

bool CGameController::IsForceBalanced()
{
	if(m_ForceBalanced)
	{
		m_ForceBalanced = false;
		return true;
	}
	else
		return false;
}

bool CGameController::CanBeMovedOnBalance(int ClientID)
{
	return true;
}

void CGameController::Tick()
{
	if(m_GameOverTick != -1)
	{
		// game over.. wait for restart
		if(Server()->Tick() > m_GameOverTick+Server()->TickSpeed()*10)
		{
			CycleMap();
			StartRound();
			m_RoundCount++;
		}
	}
	else if(GameServer()->m_World.m_Paused && m_UnpauseTimer)
	{
		--m_UnpauseTimer;
		if(!m_UnpauseTimer)
			GameServer()->m_World.m_Paused = false;
	}

	if(m_GameOverTick == -1)
	{
		if(GameServer()->GetBotNum() < MAX_BOTS)
			OnCreateBot();
	}

	// game is Paused
	if(GameServer()->m_World.m_Paused)
		++m_RoundStartTick;

	// check for inactive players
	if(g_Config.m_SvInactiveKickTime > 0)
	{
		for(int i = 0; i < MAX_CLIENTS; ++i)
		{
		#ifdef CONF_DEBUG
			if(g_Config.m_DbgDummies)
			{
				if(i >= MAX_CLIENTS-g_Config.m_DbgDummies)
					break;
			}
		#endif
			if(GameServer()->m_apPlayers[i] && !GameServer()->m_apPlayers[i]->m_IsBot && GameServer()->m_apPlayers[i]->GetTeam() != TEAM_SPECTATORS && !Server()->IsAuthed(i))
			{
				if(Server()->Tick() > GameServer()->m_apPlayers[i]->m_LastActionTick+g_Config.m_SvInactiveKickTime*Server()->TickSpeed()*60)
				{
					switch(g_Config.m_SvInactiveKick)
					{
					case 0:
						{
							// move player to spectator
							GameServer()->m_apPlayers[i]->SetTeam(TEAM_SPECTATORS);
						}
						break;
					case 1:
						{
							// move player to spectator if the reserved slots aren't filled yet, kick him otherwise
							int Spectators = 0;
							for(int j = 0; j < MAX_CLIENTS; ++j)
								if(GameServer()->m_apPlayers[j] && GameServer()->m_apPlayers[j]->GetTeam() == TEAM_SPECTATORS)
									++Spectators;
							if(Spectators >= g_Config.m_SvSpectatorSlots)
								Server()->Kick(i, "Kicked for inactivity");
							else
								GameServer()->m_apPlayers[i]->SetTeam(TEAM_SPECTATORS);
						}
						break;
					case 2:
						{
							// kick the player
							Server()->Kick(i, "Kicked for inactivity");
						}
					}
				}
			}
		}
	}

	DoWincheck();
}


bool CGameController::IsTeamplay() const
{
	return m_GameFlags&GAMEFLAG_TEAMS;
}

void CGameController::Snap(int SnappingClient)
{
	CNetObj_GameInfo *pGameInfoObj = (CNetObj_GameInfo *)Server()->SnapNewItem(NETOBJTYPE_GAMEINFO, 0, sizeof(CNetObj_GameInfo));
	if(!pGameInfoObj)
		return;

	pGameInfoObj->m_GameFlags = m_GameFlags;
	pGameInfoObj->m_GameStateFlags = 0;
	if(m_GameOverTick != -1)
		pGameInfoObj->m_GameStateFlags |= GAMESTATEFLAG_GAMEOVER;
	if(m_SuddenDeath)
		pGameInfoObj->m_GameStateFlags |= GAMESTATEFLAG_SUDDENDEATH;
	if(GameServer()->m_World.m_Paused)
		pGameInfoObj->m_GameStateFlags |= GAMESTATEFLAG_PAUSED;
	pGameInfoObj->m_RoundStartTick = m_RoundStartTick;
	pGameInfoObj->m_WarmupTimer = 0;

	pGameInfoObj->m_ScoreLimit = 0;
	pGameInfoObj->m_TimeLimit = 0;

	pGameInfoObj->m_RoundNum = 0;
	pGameInfoObj->m_RoundCurrent = 1;
}

int CGameController::GetAutoTeam(int NotThisID)
{
	// this will force the auto balancer to work overtime aswell
	if(g_Config.m_DbgStress)
		return 0;

	int aNumplayers[2] = {0,0};
	for(int i = 0; i < MAX_CLIENTS; i++)
	{
		if(GameServer()->m_apPlayers[i] && i != NotThisID)
		{
			if(GameServer()->m_apPlayers[i]->GetTeam() >= TEAM_RED && GameServer()->m_apPlayers[i]->GetTeam() <= TEAM_BLUE)
				aNumplayers[GameServer()->m_apPlayers[i]->GetTeam()]++;
		}
	}

	int Team = 0;
	if(IsTeamplay())
		Team = aNumplayers[TEAM_RED] > aNumplayers[TEAM_BLUE] ? TEAM_BLUE : TEAM_RED;

	if(CanJoinTeam(Team, NotThisID))
		return Team;
	return -1;
}

bool CGameController::CanJoinTeam(int Team, int NotThisID)
{
	if(Team == TEAM_SPECTATORS || (GameServer()->m_apPlayers[NotThisID] && GameServer()->m_apPlayers[NotThisID]->GetTeam() != TEAM_SPECTATORS))
		return true;

	int aNumplayers[2] = {0,0};
	for(int i = 0; i < MAX_CLIENTS; i++)
	{
		if(GameServer()->m_apPlayers[i] && i != NotThisID)
		{
			if(GameServer()->m_apPlayers[i]->GetTeam() >= TEAM_RED && GameServer()->m_apPlayers[i]->GetTeam() <= TEAM_BLUE)
				aNumplayers[GameServer()->m_apPlayers[i]->GetTeam()]++;
		}
	}

	return (aNumplayers[0] + aNumplayers[1]) < Server()->MaxClients()-g_Config.m_SvSpectatorSlots;
}

bool CGameController::CanChangeTeam(CPlayer *pPlayer, int JoinTeam)
{
	int aT[2] = {0, 0};

	if (!IsTeamplay() || JoinTeam == TEAM_SPECTATORS)
		return true;

	for(int i = 0; i < MAX_CLIENTS; i++)
	{
		CPlayer *pP = GameServer()->m_apPlayers[i];
		if(pP && pP->GetTeam() != TEAM_SPECTATORS)
			aT[pP->GetTeam()]++;
	}

	// simulate what would happen if changed team
	aT[JoinTeam]++;
	if (pPlayer->GetTeam() != TEAM_SPECTATORS)
		aT[JoinTeam^1]--;

	// there is a player-difference of at least 2
	if(absolute(aT[0]-aT[1]) >= 2)
	{
		// player wants to join team with less players
		if ((aT[0] < aT[1] && JoinTeam == TEAM_RED) || (aT[0] > aT[1] && JoinTeam == TEAM_BLUE))
			return true;
		else
			return false;
	}
	else
		return true;
}

void CGameController::DoWincheck()
{
}

int CGameController::ClampTeam(int Team)
{
	if(Team < 0)
		return TEAM_SPECTATORS;
	if(IsTeamplay())
		return Team&1;
	return 0;
}

double CGameController::GetTime()
{
	return static_cast<double>(Server()->Tick() - m_RoundStartTick)/Server()->TickSpeed();
}

const char* CGameController::GetResourceName(int ID)
{
	switch (ID)
	{
		case RESOURCE_METAL: return "Metal";
		case RESOURCE_WOOD: return "Wood";
	}
}
/*********************MAKE ITEM*********************/ 
// TODO: Move this to item make module
CGameController::CItem::CItem()
{
	m_GiveID = 0;
	m_GiveNum = 0;
	m_NeedResource.ResetResource();
}

void CGameController::OnItemMake(const char *pMakeItem, int ClientID)
{
	CItem ItemInfo;
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

	//dbg_msg("item", "make %s, need %d metal and %d wood, this player have %d metal, and %d wood", pMakeItem,
	//	 ItemInfo.m_NeedResource.m_Metal, ItemInfo.m_NeedResource.m_Wood, pPResource->m_Metal, pPResource->m_Wood);

	if(!CanMake)
	{
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

		GameServer()->SendChatTarget_Locazition(ClientID, _("This item requires {STR}."), Buffer.c_str());
		return;
	}

	bool CanMakeGun=true;
	if(ItemInfo.m_GiveID <= ITEM_RIFLE)
	{
		CCharacter *pChr = pPlayer->GetCharacter();

		switch (ItemInfo.m_GiveID)
		{
			case ITEM_SHOTGUN: if(pChr->GetWeaponStat()[WEAPON_SHOTGUN].m_Got) CanMakeGun = false;break;
			case ITEM_GRENADE: if(pChr->GetWeaponStat()[WEAPON_GRENADE].m_Got) CanMakeGun = false;break;
			case ITEM_RIFLE: if(pChr->GetWeaponStat()[WEAPON_RIFLE].m_Got) CanMakeGun = false;break;
		}
	}

	if(!CanMakeGun)
	{
		GameServer()->SendChatTarget_Locazition(ClientID, _("You had {STR}"), 
			GameServer()->Localize(pLanguageCode, pMakeItem));
		return;
	}

	GameServer()->SendChatTarget_Locazition(ClientID, _("Making {STR}..."), 
		GameServer()->Localize(pLanguageCode, pMakeItem));

	ReturnItem(ItemInfo, ClientID);
	
}

void CGameController::ReturnItem(CItem Item, int ClientID)
{
	CPlayer *pPlayer = GameServer()->m_apPlayers[ClientID];
	if(!pPlayer)
		return;


	const char *pLanguageCode = pPlayer->GetLanguage();

	if(Item.m_GiveID <= ITEM_RIFLE)
	{
		CCharacter *pChr = pPlayer->GetCharacter();
		if(!pChr)
			return;

		switch (Item.m_GiveID)
		{
			case ITEM_GUN: pChr->GetWeaponStat()[WEAPON_GUN].m_Got = true;break;
			case ITEM_SHOTGUN: pChr->GetWeaponStat()[WEAPON_SHOTGUN].m_Got = true;break;
			case ITEM_GRENADE: pChr->GetWeaponStat()[WEAPON_GRENADE].m_Got = true;break;
			case ITEM_RIFLE: pChr->GetWeaponStat()[WEAPON_RIFLE].m_Got = true;break;
		}	
	}

	if(Item.m_GiveNum)
		GameServer()->SendChatTarget_Locazition(ClientID, _("Make finish, you get {INT} {STR}"), 
			Item.m_GiveNum, Item.m_aName);
	else GameServer()->SendChatTarget_Locazition(ClientID, _("Make finish, you get {STR}"), 
			Item.m_aName);
	
	for(int i = 0; i < NUM_RESOURCES;i ++)
	{
		int Need = Item.m_NeedResource.GetResource(i);
		int Have = pPlayer->m_Resource.GetResource(i);
		pPlayer->m_Resource.SetResource(i, Have - Need);
	}
}

// Find Item, if it in the json.return the FOUND(and the Item info)
bool CGameController::FindItem(const char *pMakeItem, CItem *ItemInfo)
{
	// read file data into buffer
	const char *pFilename = "./data/item.json";
	IOHANDLE File = GameServer()->Storage()->OpenFile(pFilename, IOFLAG_READ, IStorage::TYPE_ALL);
	if(!File)
	{
		dbg_msg("Item", "can't open 'data/item.json'");
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

	const json_value &rStart = (*pJsonData)["items"];
	bool Found = false;
	if(rStart.type == json_array)
	{
		for(unsigned i = 0; i < rStart.u.array.length; ++i)
		{
			if(str_comp((const char *)rStart[i]["name"], pMakeItem) == 0)
			{
				str_copy(ItemInfo->m_aName, (const char *)rStart[i]["name"], sizeof(ItemInfo->m_aName));
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

void CGameController::ShowMakeList(int ClientID)
{
	// read file data into buffer
	const char *pFilename = "./data/item.json";
	IOHANDLE File = GameServer()->Storage()->OpenFile(pFilename, IOFLAG_READ, IStorage::TYPE_ALL);
	if(!File)
	{
		dbg_msg("Item", "can't open 'data/item.json'");
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

	const json_value &rStart = (*pJsonData)["items"];
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

	// clean up
	json_value_free(pJsonData);
	delete[] pFileData;

	GameServer()->SendChatTarget_Locazition(ClientID, _("You can make: {STR}"), Buffer.c_str());

	return;
}
/* Item Make End*/

static char* format_int64_with_commas(char commas, int64 n)
{
	char _number_array[64] = { '\0' };
	str_format(_number_array, sizeof(_number_array), "%lld", n); // %ll

	const char* _number_pointer = _number_array;
	int _number_of_digits = 0;
	while (*(_number_pointer + _number_of_digits++));
	--_number_of_digits;

	/*
	*	count the number of digits
	*	calculate the position for the first comma separator
	*	calculate the final length of the number with commas
	*
	*	the starting position is a repeating sequence 123123... which depends on the number of digits
	*	the length of the number with commas is the sequence 111222333444...
	*/
	const int _starting_separator_position = _number_of_digits < 4 ? 0 : _number_of_digits % 3 == 0 ? 3 : _number_of_digits % 3;
	const int _formatted_number_length = _number_of_digits + _number_of_digits / 3 - (_number_of_digits % 3 == 0 ? 1 : 0);

	// create formatted number array based on calculated information.
	char* _formatted_number = new char[20 * 3 + 1];

	// place all the commas
	for (int i = _starting_separator_position; i < _formatted_number_length - 3; i += 4)
		_formatted_number[i] = commas;

	// place the digits
	for (int i = 0, j = 0; i < _formatted_number_length; i++)
		if (_formatted_number[i] != commas)
			_formatted_number[i] = _number_pointer[j++];

	/* close the string */
	_formatted_number[_formatted_number_length] = '\0';
	return _formatted_number;
}

void CGameController::ShowStatus(int ClientID)
{
	CPlayer *pPlayer = GameServer()->m_apPlayers[ClientID];

	if(!pPlayer)
		return;

	if(!pPlayer->GetCharacter())
		return;

	const char *pLanguageCode = pPlayer->GetLanguage();

	Resource *pPResource = &pPlayer->m_Resource;
	std::string Buffer;
	Buffer.clear();
	
	bool First=true;	
	for(int i = 0; i < NUM_RESOURCES;i ++)
	{
		if(!First)
			Buffer.append(", ");
		else First = false;
		Buffer.append(GameServer()->Localize(pLanguageCode, GetResourceName(i)));
		Buffer.append(": ");
		Buffer.append(format_int64_with_commas(',', pPResource->GetResource(i)));
		Buffer.append(" ");
	}

	GameServer()->SendChatTarget(ClientID, Buffer.c_str());
}

void CGameController::OnCreateBot()
{
	for(int i = BOT_CLIENTS_START; i < BOT_CLIENTS_END; i ++)
	{
		if(GameServer()->m_apPlayers[i]) continue;
		int Power = RandomPower();
		GameServer()->CreateBot(i, Power);
	}
}

int CGameController::RandomPower()
{
	int PowerNum = random_int(1, NUM_BOTPOWERS);
	int Power = 0;
	for(int i = 0;i < PowerNum;i ++)
	{
		Power |= 1<<random_int(0, NUM_BOTPOWERS);
	}
	return Power;
}

void CGameController::CreateZombiePickup(vec2 Pos, vec2 Dir)
{
	int PickupType, PickupSubtype;
	PickupType = random_int(0, NUM_PICKUPS-1);
	
	if(PickupType == PICKUP_AMMO)
	{
		PickupSubtype = random_int(WEAPON_GUN, WEAPON_RIFLE);
	}
	else if(PickupType == PICKUP_RESOURCE)
	{
		PickupSubtype = random_int(RESOURCE_METAL, NUM_RESOURCES-1);
	}

	new CPickup(&GameServer()->m_World, Pos, Dir, PickupType, PickupSubtype);
}