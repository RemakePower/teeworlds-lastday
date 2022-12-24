/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */
#include <game/generated/protocol.h>
#include <game/server/gamecontext.h>
#include "pickup.h"

CPickup::CPickup(CGameWorld *pGameWorld, vec2 Pos, vec2 Dir, int Type, int SubType, int Num)
: CEntity(pGameWorld, CGameWorld::ENTTYPE_PICKUP)
{
	m_Pos = Pos;
	m_StartPos = Pos;
	m_Direction = Dir;
	m_Type = Type;
	m_Subtype = SubType;
	m_Num = Num;
	m_ProximityRadius = PickupPhysSize;
	m_StartTick = Server()->Tick();

	GameWorld()->InsertEntity(this);
	
	for(int i=0; i<PickupAmmoNum; i++)
	{
		m_AmmoIDs[i] = Server()->SnapNewID();
	}
}

CPickup::~CPickup()
{
	for(int i=0; i<PickupAmmoNum; i++)
	{
		Server()->SnapFreeID(m_AmmoIDs[i]);
	}
}

vec2 CPickup::GetPos(float Time)
{
	float Curvature = GameServer()->Tuning()->m_GrenadeCurvature;
	float Speed = GameServer()->Tuning()->m_GrenadeSpeed;

	return CalcPos(m_StartPos, m_Direction, Curvature, Speed, Time);
}

void CPickup::Tick()
{
	float Pt = (Server()->Tick()-m_StartTick-1)/(float)Server()->TickSpeed();
	float Ct = (Server()->Tick()-m_StartTick)/(float)Server()->TickSpeed();
	vec2 PrevPos = GetPos(Pt);
	vec2 CurPos = GetPos(Ct);

	if(GameLayerClipped(CurPos))
	{
		GameServer()->m_World.DestroyEntity(this);
		return;
	}
	
	m_Pos = CurPos;

	vec2 LastPos;
	int Collide = GameServer()->Collision()->IntersectLine(PrevPos, CurPos, NULL, &LastPos);
	if(Collide)
	{
		//Thanks to TeeBall 0.6
		vec2 CollisionPos;
		CollisionPos.x = LastPos.x;
		CollisionPos.y = CurPos.y;
		int CollideY = GameServer()->Collision()->IntersectLine(PrevPos, CollisionPos, NULL, NULL);
		CollisionPos.x = CurPos.x;
		CollisionPos.y = LastPos.y;
		int CollideX = GameServer()->Collision()->IntersectLine(PrevPos, CollisionPos, NULL, NULL);
		
		m_StartPos = LastPos;
		m_Pos = m_StartPos;
		vec2 vel;
		vel.x = m_Direction.x;
		vel.y = m_Direction.y + 2*GameServer()->Tuning()->m_GrenadeCurvature/10000*Ct*GameServer()->Tuning()->m_GrenadeSpeed;
		
		if (CollideX && !CollideY)
		{
			m_Direction.x = -vel.x;
			m_Direction.y = vel.y;
		}
		else if (!CollideX && CollideY)
		{
			m_Direction.x = vel.x;
			m_Direction.y = -vel.y;
		}
		else
		{
			m_Direction.x = -vel.x;
			m_Direction.y = -vel.y;
		}
		
		m_Direction.x *= (100 - 50) / 100.0;
		m_Direction.y *= (100 - 50) / 100.0;
		m_StartTick = Server()->Tick();
	}

	// Check if a player intersected us
	CCharacter *pChr = GameServer()->m_World.ClosestCharacter(m_Pos, 32.0f, 0);
	if(pChr && pChr->IsAlive() && !pChr->GetPlayer()->m_IsBot)
	{
		bool Destroy = false;
		switch (m_Type)
		{
			case PICKUP_AMMO:
			{
				if(pChr->GetWeaponStat()[m_Subtype].m_Got)
				{
					pChr->GetWeaponStat()[m_Subtype].m_Ammo += m_Num;
					GameServer()->SendChatTarget_Locazition(pChr->GetCID(), _("You got %d %s"),
						m_Num, GetAmmoType(m_Subtype));
					GameServer()->CreateSound(m_Pos, SOUND_PICKUP_SHOTGUN);
					Destroy = true;
				}
				break;
			}
			case PICKUP_RESOURCE:
			{
				GameServer()->AddResource(pChr->GetCID(), m_Subtype, m_Num);
				Destroy = true;
				break;
			}
		}

		if(Destroy)
			GameServer()->m_World.DestroyEntity(this);
	}

	if((Server()->Tick() - m_StartTick)/Server()->TickSpeed() >= 60)
		GameServer()->m_World.DestroyEntity(this);
}

void CPickup::TickPaused()
{
	m_StartTick++;
}

void CPickup::Snap(int SnappingClient)
{
	if(NetworkClipped(SnappingClient))
		return;

	if(m_Type == PICKUP_RESOURCE)
	{
		CNetObj_Pickup *pP = static_cast<CNetObj_Pickup *>(Server()->SnapNewItem(NETOBJTYPE_PICKUP, m_ID, sizeof(CNetObj_Pickup)));
		if(!pP)
			return;

		pP->m_X = (int)m_Pos.x;
		pP->m_Y = (int)m_Pos.y;
		pP->m_Type = POWERUP_WEAPON;
		pP->m_Subtype = WEAPON_HAMMER;
	}
	else
	{
		int Degres = 0;

		for(int i = 0;i < PickupAmmoNum;i++)
		{
			vec2 Pos = m_Pos + (GetDir(Degres*pi/180) * m_ProximityRadius);

			CNetObj_Projectile *pP = static_cast<CNetObj_Projectile *>(Server()->SnapNewItem(NETOBJTYPE_PROJECTILE, m_AmmoIDs[i], sizeof(CNetObj_Projectile)));
			if(!pP)
				return;

			pP->m_X = (int)Pos.x;
			pP->m_Y = (int)Pos.y;
			pP->m_Type = WEAPON_SHOTGUN;
			pP->m_VelX = 0;
			pP->m_VelY = 0;
			pP->m_StartTick = Server()->Tick()-2;
			Degres -= 360 / PickupAmmoNum;
		}

		CNetObj_Projectile *pP = static_cast<CNetObj_Projectile *>(Server()->SnapNewItem(NETOBJTYPE_PROJECTILE, m_ID, sizeof(CNetObj_Projectile)));
		if(!pP)
			return;

		pP->m_X = (int)m_Pos.x;
		pP->m_Y = (int)m_Pos.y;
		pP->m_Type = m_Subtype;
		pP->m_VelX = 0;
		pP->m_VelY = 0;
		pP->m_StartTick = Server()->Tick()-1;
	}
}
