#include <game/server/define.h>
#include <game/server/gamecontext.h>
#include "bullets/projectile.h"
#include "gun.h"

CWeaponGun::CWeaponGun(CGameContext *pGameServer)
    : CWeapon(pGameServer, LD_WEAPON_GUN, WEAPON_GUN, 125, 2)
{
}

void CWeaponGun::Fire(int Owner, vec2 Dir, vec2 Pos)
{
    CProjectile *pProj = new CProjectile(GameWorld(), WEAPON_GUN,
    Owner,
    Pos,
    Dir,
    (int)(GameServer()->Server()->TickSpeed()*GameServer()->Tuning()->m_GunLifetime),
    GetDamage(), 0, 0, -1, WEAPON_GUN, 0);

    GameServer()->CreateSound(Pos, SOUND_GUN_FIRE);
    
    return;
}