#include <game/server/define.h>
#include <game/server/gamecontext.h>
#include "bullets/projectile.h"
#include "grenade.h"

CWeaponGrenade::CWeaponGrenade(CGameContext *pGameServer)
    : CWeapon(pGameServer, TWS_WEAPON_GRENADE, WEAPON_GRENADE, 500)
{
}

void CWeaponGrenade::Fire(int Owner, vec2 Dir, vec2 Pos)
{
    CProjectile *pProj = new CProjectile(GameWorld(), WEAPON_GRENADE,
        Owner,
        Pos,
        Dir,
        (int)(GameServer()->Server()->TickSpeed()*GameServer()->Tuning()->m_GrenadeLifetime),
        1, true, 0, SOUND_GRENADE_EXPLODE, GetShowType(), 0);

    GameServer()->CreateSound(Pos, SOUND_GRENADE_FIRE);
    
    return;
}