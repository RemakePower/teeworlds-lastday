#include "weapon.h"


#include <game/server/lastday/Components/Weapon/WeaponData/weaponlist.h>

void WeaponInit::InitWeapons(CGameContext *pGameServer)
{
	g_Weapons.InitWeapon(TWS_WEAPON_HAMMER, new CWeaponHammer(pGameServer));
	g_Weapons.InitWeapon(TWS_WEAPON_NINJA, new CWeaponNinja(pGameServer));
	g_Weapons.InitWeapon(TWS_WEAPON_GUN, new CWeaponGun(pGameServer));
	g_Weapons.InitWeapon(TWS_WEAPON_SHOTGUN, new CWeaponShotgun(pGameServer));
	g_Weapons.InitWeapon(TWS_WEAPON_GRENADE, new CWeaponGrenade(pGameServer));
	g_Weapons.InitWeapon(TWS_WEAPON_RIFLE, new CWeaponRifle(pGameServer));
}