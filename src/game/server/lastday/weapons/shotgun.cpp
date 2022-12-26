#include <game/server/define.h>
#include <game/server/gamecontext.h>
#include "bullets/projectile.h"
#include "shotgun.h"

CWeaponShotgun::CWeaponShotgun(CGameContext *pGameServer)
    : CWeapon(pGameServer, TWS_WEAPON_SHOTGUN, WEAPON_SHOTGUN, 500)
{
}

void CWeaponShotgun::Fire(int Owner, vec2 Dir, vec2 Pos)
{
    int ShotSpread = 2;

    for(int i = -ShotSpread; i <= ShotSpread; ++i)
    {
        float Spreading[] = {-0.185f, -0.070f, 0, 0.070f, 0.185f};
        float a = GetAngle(Dir);
        a += Spreading[i+2];
        float v = 1-(absolute(i)/(float)ShotSpread);
        float Speed = mix((float)GameServer()->Tuning()->m_ShotgunSpeeddiff, 1.0f, v);
        CProjectile *pProj = new CProjectile(GameWorld(), WEAPON_SHOTGUN,
            Owner,
            Pos,
            vec2(cosf(a), sinf(a))*Speed,
            (int)(GameServer()->Server()->TickSpeed()*GameServer()->Tuning()->m_ShotgunLifetime),
            1, 0, 0, -1, GetShowType(), 0);
    }

    GameServer()->CreateSound(Pos, SOUND_SHOTGUN_FIRE);
    
    return;
}