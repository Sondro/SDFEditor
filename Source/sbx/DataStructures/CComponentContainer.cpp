/*
 * @file    CComponentContainer.cpp
 * @author  David Gallardo Moreno
 */

#include <sbx/Common.h>
#include "CComponentContainer.h"

namespace sbx
{
    CComponentContainer::CComponentContainer()
    {
    }

    CComponentContainer::~CComponentContainer()
    {
        for(auto lIterator = mComponents.begin(); lIterator != mComponents.end(); lIterator++)
        {
            lIterator->second.mDeleter( lIterator->second.mObject );
        }
    }
}