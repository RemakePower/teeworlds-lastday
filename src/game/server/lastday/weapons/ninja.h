#ifndef GAME_SERVER_LASTDAY_WEAPONS_NINJA_H
#define GAME_SERVER_LASTDAY_WEAPONS_NINJA_H

#include <game/server/lastday/weapons-core/weapon.h>

class CWeaponNinja : public CWeapon
{
public:
    CWeaponNinja(CGameContext *pGameServer);

    virtual void Fire(int Owner, vec2 Dir, vec2 Pos);
};

#endif