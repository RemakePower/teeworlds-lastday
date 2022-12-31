#ifndef GAME_SERVER_DEFINE_H
#define GAME_SERVER_DEFINE_H

#include <base/system.h>

#define MENU_CLOSETICK 200

char* format_int64_with_commas(char commas, int64 n);

const char *GetAmmoName(int WeaponID);
const char *GetWeaponName(int WeaponID);

enum ItemList
{
    ITEM_GUN_AMMO,
    ITEM_SHOTGUN_AMMO,
    ITEM_GRENADE_AMMO,
    ITEM_RIFLE_AMMO,

    ITEM_GUN,
    ITEM_SHOTGUN,
    ITEM_GRENADE,
    ITEM_RIFLE,
    ITEM_NINJA,
};

enum OptionType
{
    MENUOPTION_OPTIONS=0,
    MENUOPTION_ITEMS,

    NUM_MENUOPTIONS,
};

enum LastDayWeapons
{
    LD_WEAPON_HAMMER=0,
    LD_WEAPON_GUN,
    LD_WEAPON_SHOTGUN,
    LD_WEAPON_GRENADE,
    LD_WEAPON_RIFLE,
    LD_WEAPON_NINJA,

    NUM_LASTDAY_WEAPONS,
};

enum MenuPages
{
    MENUPAGE_MAIN=0,
    MENUPAGE_NOTMAIN,
    MENUPAGE_ITEM,
};

class CBotPower
{
public:
    CBotPower() {};
    char m_SkinName[64];
    bool m_TeamDamage;
    bool m_Hammer;
    bool m_Gun;
    bool m_Hook;
};

#endif