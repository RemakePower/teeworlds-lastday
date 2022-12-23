#ifndef GAME_SERVER_LASTDAY_WEAPONS_GUN_H
#define GAME_SERVER_LASTDAY_WEAPONS_GUN_H

#include <game/server/LastDay/Components/Weapon/WeaponCore/weapon.h>

class CWeaponGun : public CWeapon
{
public:
    CWeaponGun(CGameContext *pGameServer);

    void Fire(int Owner, vec2 Dir, vec2 Pos) override;
};

#endif