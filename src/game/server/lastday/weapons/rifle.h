#ifndef GAME_SERVER_LASTDAY_WEAPONS_RIFLE_H
#define GAME_SERVER_LASTDAY_WEAPONS_RIFLE_H

#include <game/server/lastday/weapons-core/weapon.h>

class CWeaponRifle : public CWeapon
{
public:
    CWeaponRifle(CGameContext *pGameServer);

    virtual void Fire(int Owner, vec2 Dir, vec2 Pos);
};

#endif