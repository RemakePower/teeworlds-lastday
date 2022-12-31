#include <game/server/gamecontext.h>
#include "weapon.h"

CWeapon::CWeapon(CGameContext *pGameServer, int WeaponID, int ShowType, int FireDelay, int Damage) :
    IWeapon(pGameServer, WeaponID, ShowType, FireDelay, Damage)
{
}

void CWeapon::OnFire(int Owner, vec2 Dir, vec2 Pos)
{
    Fire(Owner, Dir, Pos);
}