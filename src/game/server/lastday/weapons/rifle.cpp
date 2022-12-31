#include <game/server/define.h>
#include <game/server/gamecontext.h>
#include "bullets/tws-laser.h"
#include "rifle.h"

CWeaponRifle::CWeaponRifle(CGameContext *pGameServer)
    : CWeapon(pGameServer, LD_WEAPON_RIFLE, WEAPON_RIFLE, 800, 6)
{
}

void CWeaponRifle::Fire(int Owner, vec2 Dir, vec2 Pos)
{
    CTWSLaser *pLaser = new CTWSLaser(GameWorld(),
        Pos,
        Dir,
        GameServer()->Tuning()->m_LaserReach,
        Owner,
        GetDamage());

    GameServer()->CreateSound(Pos, SOUND_RIFLE_FIRE);
    
    return;
}