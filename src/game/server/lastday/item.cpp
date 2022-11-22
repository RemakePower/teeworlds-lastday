#include "item.h"

int64 Resource::GetResource(int ID)
{
    switch (ID)
    {
        case RESOURCE_METAL: return m_Metal;
        case RESOURCE_WOOD: return m_Wood;
        
        default: return 0;
    }
}

void Resource::SetResource(int ID, int Num)
{
    switch (ID)
    {
        case RESOURCE_METAL: m_Metal = Num;
        case RESOURCE_WOOD: m_Wood = Num;
        
        default: return;
    }
}

void Resource::ResetResource()
{
    m_Metal = 100;
    m_Wood = 100;
}