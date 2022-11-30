#ifndef GAME_SERVER_LASTDAY_ITEM_LIST_H
#define GAME_SERVER_LASTDAY_ITEM_LIST_H

#include <base/system.h>

enum ItemList
{
    ITEM_SHOTGUN=0,
    ITEM_GRENADE,
    ITEM_RIFLE,

    NUM_ITEMS,
};

enum ResourceList
{
    RESOURCE_METAL=0,
    RESOURCE_WOOD,

    NUM_RESOURCES,
};

struct Resource
{
    int64 GetResource(int ID);
    void SetResource(int ID, int64 Num);
    void ResetResource();
    int64 m_Metal;
    int64 m_Wood;
};


#endif