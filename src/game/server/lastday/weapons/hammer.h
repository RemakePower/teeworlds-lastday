#ifndef GAME_SERVER_LASTDAY_WEAPONS_HAMMER_H
#define GAME_SERVER_LASTDAY_WEAPONS_HAMMER_H

#include <game/server/lastday/weapons-core/weapon.h>

class CWeaponHammer : public CWeapon
{
public:
    CWeaponHammer(CGameContext *pGameServer);

    virtual void Fire(int Owner, vec2 Dir, vec2 Pos);
};

#endif