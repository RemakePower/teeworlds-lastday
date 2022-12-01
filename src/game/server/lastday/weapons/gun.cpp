#include <game/server/define.h>
#include <game/server/gamecontext.h>
#include "bullets/projectile.h"
#include "gun.h"

CWeaponGun::CWeaponGun(CGameContext *pGameServer)
    : CWeapon(pGameServer, TWS_WEAPON_GUN, WEAPON_GUN, 125)
{
}

void CWeaponGun::Fire(int Owner, vec2 Dir, vec2 Pos)
{
    CProjectile *pProj = new CProjectile(GameWorld(), WEAPON_GUN,
    Owner,
    Pos,
    Dir,
    (int)(GameServer()->Server()->TickSpeed()*GameServer()->Tuning()->m_GunLifetime),
    1, 0, 0, -1, WEAPON_GUN);

    GameServer()->CreateSound(Pos, SOUND_GUN_FIRE);
    
    return;
}