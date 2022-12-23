#include <game/server/gamecontext.h>
#include "weapon.h"

IWeapon::IWeapon(CGameContext *pGameServer, int WeaponID, int ShowType, int FireDelay) :
    m_pGameServer(pGameServer),
    m_WeaponID(WeaponID),
    m_ShowType(ShowType),
    m_FireDelay(FireDelay)
{
}

CGameContext *IWeapon::GameServer() const
{
    return m_pGameServer;
}

CGameWorld *IWeapon::GameWorld() const
{
    return &m_pGameServer->m_World;
}

int IWeapon::GetWeaponID() const
{
    return m_WeaponID;
}

int IWeapon::GetShowType() const
{
    return m_ShowType;
}

int IWeapon::GetFireDelay() const
{
    return m_FireDelay;
}