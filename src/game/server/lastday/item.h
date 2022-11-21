#ifndef GAME_SERVER_LASTDAY_ITEM_LIST_H
#define GAME_SERVER_LASTDAY_ITEM_LIST_H

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
    long GetResource(int ID);
    void ResetResource();
    long m_Metal;
    long m_Wood;
};


#endif