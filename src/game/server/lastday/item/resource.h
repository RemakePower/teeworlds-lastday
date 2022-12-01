#ifndef GAME_SERVER_LASTDAY_ITEM_LIST_H
#define GAME_SERVER_LASTDAY_ITEM_LIST_H

#include <base/system.h>


struct Resource
{
    int64 GetResource(int ID);
    void SetResource(int ID, int64 Num);
    void ResetResource();
    int64 m_Metal;
    int64 m_Wood;
};


#endif