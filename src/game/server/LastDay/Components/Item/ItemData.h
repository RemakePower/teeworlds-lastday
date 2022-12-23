#ifndef GAME_SERVER_LASTDAY_ITEM_DATA_H
#define GAME_SERVER_LASTDAY_ITEM_DATA_H

#include <base/system.h>

struct Resource
{
    int64 GetResource(int ID);
    void SetResource(int ID, int64 Num);
    void ResetResource();
    int64 m_Metal;
    int64 m_Wood;
};

class CItemData
{
public:
	CItemData();
	char m_aName[64];
	int m_GiveType;
	int m_GiveID;
	int m_GiveNum;
	Resource m_NeedResource;
};


#endif