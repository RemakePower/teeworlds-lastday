/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */
#ifndef GAME_SERVER_ENTITIES_PICKUP_H
#define GAME_SERVER_ENTITIES_PICKUP_H

#include <game/server/entity.h>

const int PickupPhysSize = 16;
const int PickupAmmoNum = 8;

class CPickup : public CEntity
{
public:
	CPickup(CGameWorld *pGameWorld, vec2 Pos, vec2 Dir, int Type, int SubType = 0);
	~CPickup();

	vec2 GetPos(float Time);

	virtual void Tick();
	virtual void Snap(int SnappingClient);

private:
	vec2 m_Direction;
	vec2 m_StartPos;
	int m_AmmoIDs[PickupAmmoNum];
	int m_Type;
	int m_Subtype;
	int m_StartTick;
};

#endif
