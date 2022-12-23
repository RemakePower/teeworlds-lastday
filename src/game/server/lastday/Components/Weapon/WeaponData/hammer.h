#ifndef GAME_SERVER_LASTDAY_WEAPONS_HAMMER_H
#define GAME_SERVER_LASTDAY_WEAPONS_HAMMER_H

#include <game/server/lastday/Components/Weapon/WeaponCore/weapon.h>

class CWeaponHammer : public CWeapon
{
public:
    CWeaponHammer(CGameContext *pGameServer);

    void Fire(int Owner, vec2 Dir, vec2 Pos) override;
};

#endif