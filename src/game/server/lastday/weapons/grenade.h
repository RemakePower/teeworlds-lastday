#ifndef GAME_SERVER_LASTDAY_WEAPONS_GRENADE_H
#define GAME_SERVER_LASTDAY_WEAPONS_GRENADE_H

#include <game/server/lastday/weapons-core/weapon.h>

class CWeaponGrenade : public CWeapon
{
public:
    CWeaponGrenade(CGameContext *pGameServer);

    virtual void Fire(int Owner, vec2 Dir, vec2 Pos);
};

#endif