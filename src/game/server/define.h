#ifndef GAME_SERVER_DEFINE_H
#define GAME_SERVER_DEFINE_H

#include <base/system.h>

char* format_int64_with_commas(char commas, int64 n);

const char* GetResourceName(int ID);
const char *GetAmmoType(int WeaponID);

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

enum ResourceList
{
    RESOURCE_METAL=0,
    RESOURCE_WOOD,

    NUM_RESOURCES,
};

enum OptionType
{
    MENUOPTION_OPTIONS=0,
    MENUOPTION_ITEMS,

    NUM_MENUOPTIONS,
};

enum PickupType
{
    PICKUP_AMMO=0,
    PICKUP_RESOURCE,
    PICKUP_GUN,
    
    NUM_PICKUPS,
};

enum LastDayWeapons
{
    TWS_WEAPON_HAMMER=0,
    TWS_WEAPON_GUN,
    TWS_WEAPON_SHOTGUN,
    TWS_WEAPON_GRENADE,
    TWS_WEAPON_RIFLE,
    TWS_WEAPON_NINJA,

    NUM_LASTDAY_WEAPONS,
};

enum BotPower
{
    BOTPOWER_HAMMER=1<<0,
    BOTPOWER_HOOK=1<<1,
    BOTPOWER_GUN=1<<2,
    BOTPOWER_TEAMDAMAGE=1<<3,
    //BOTPOWER_SHOTGUN=1<<4,
    //BOTPOWER_GRENADE=1<<5,
    //BOTPOWER_RIFLE=1<<6,

    NUM_BOTPOWERS=4,
};

enum MenuPages
{
    MENUPAGE_MAIN=0,
    MENUPAGE_NOTMAIN,
    MENUPAGE_ITEM,
};

#endif