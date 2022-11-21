#include "item.h"

long Resource::GetResource(int ID)
{
    switch (ID)
    {
        case RESOURCE_METAL: return m_Metal;
        case RESOURCE_WOOD: return m_Wood;
        
        default: return 0;
    }
}

void Resource::ResetResource()
{
    m_Metal = 100;
    m_Wood = 100;
}