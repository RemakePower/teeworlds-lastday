#ifndef GAME_SERVER_LASTDAY_WEAPONS_RIFLE_H
#define GAME_SERVER_LASTDAY_WEAPONS_RIFLE_H

#include <game/server/LastDay/Components/Weapon/WeaponCore/weapon.h>

class CWeaponRifle : public CWeapon
{
public:
    CWeaponRifle(CGameContext *pGameServer);

    void Fire(int Owner, vec2 Dir, vec2 Pos) override;
};

#endif