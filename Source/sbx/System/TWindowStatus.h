/*
 * @file    TWindowStatus.h
 * @author  David Gallardo Moreno
 */

#ifndef __SBX_WINDOW_STATUS_H__
#define __SBX_WINDOW_STATUS_H__

#include <sbx/FileSystem/File.h>

namespace sbx
{
    struct TWindowStatus
    {
        //Stored
        int32_t     mPosX   {200};
        int32_t     mPosY   {200};
        int32_t     mSizeX  {1420};
        int32_t     mSizeY  {720};
        bool        mIsMaximized    {false};

        //Current Status
        int32_t     mPrevSizeX  {1420};
        int32_t     mPrevSizeY  {720};
        bool        mPosChanged     {false};
        bool        mWantsClose     {false};
        bool        mWantsMinimize  {false};
        bool        mWantsMaximize  {false};
    
        bool        mHadUnsavedActions  {false};
        bool        mTitleSet           {false};
        TUniqueIdCS mOpenedPath;
        std::string mTitle;
        

        void    LoadFromFile    (TPath const & aPath);
        void    SaveToFile      (TPath const & aPath);

        bool    SizeChanged     () const    { return (mSizeX != mPrevSizeX) || (mSizeY != mPrevSizeY); };
        void    UpdateSize      ()          { mPrevSizeX = mSizeX; mPrevSizeY = mSizeY; };

    };
}

#endif //#ifndef __SBX_WINDOW_STATUS_H__