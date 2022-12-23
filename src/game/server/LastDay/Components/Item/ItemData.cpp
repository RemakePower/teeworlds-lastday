#include "game/server/define.h"
#include "ItemData.h"

int64 Resource::GetResource(int ID)
{
    switch (ID)
    {
        case RESOURCE_METAL: return m_Metal;
        case RESOURCE_WOOD: return m_Wood;
        
        default: return 0;
    }
}

void Resource::SetResource(int ID, int64 Num)
{
    switch (ID)
    {
        case RESOURCE_METAL: m_Metal = Num;return;
        case RESOURCE_WOOD: m_Wood = Num;return;
        
        default: return;
    }
}

void Resource::ResetResource()
{
    m_Metal = 0;
    m_Wood = 0;
}

CItemData::CItemData()
{
	m_GiveType = 0;
	m_GiveID = 0;
	m_GiveNum = 0;
	m_NeedResource.ResetResource();
}