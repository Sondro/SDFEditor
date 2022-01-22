/*
 * @file CChannel.cpp
 * @author David Gallardo Moreno
 */

#include <sbx/Common.h>
#include <sbx/Messaging/CChannel.h>

namespace sbx
{
    CChannel::CChannel()
    {;}

    CChannel::~CChannel()
    {
        UnregisterAllCallbacks();
    }

    void CChannel::UnregisterAllCallbacks()
    {
        for(auto lIterator = mBroadcasters.begin(); lIterator != mBroadcasters.end(); lIterator++)
        {
            lIterator->second.mDeleter(lIterator->second.mBroadcaster);
            lIterator->second.mBroadcaster = 0;
        }

        mBroadcasters.clear();
    }
} // sbx
