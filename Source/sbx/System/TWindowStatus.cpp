/*
 * @file    TWindowStatus.cpp
 * @author  David Gallardo Moreno
 */

#include <sbx/Common.h>
#include "TWindowStatus.h"

#include <sbx/FileSystem/CResource.h>
#include <sbx/FileSystem/ResourceUtils.h>

namespace sbx
{
    void TWindowStatus::LoadFromFile(sbx::TPath const & aPath)
    {
        if(aPath.IsFile())
        {
            sbx::CResource lRes;
            sbx::sbxJsonFileToRes(aPath, lRes);
            if(lRes.IsOk())
            {
                mPosX           = lRes["posx"].AsUInt32(mPosX);
                mPosY           = lRes["posy"].AsUInt32(mPosY);
                mSizeX          = lRes["sizex"].AsUInt32(mSizeX);
                mSizeY          = lRes["sizey"].AsUInt32(mSizeY);
                mIsMaximized    = lRes["maximized"].AsBool(mIsMaximized);
                UpdateSize();
            }
        } 
    }

    void TWindowStatus::SaveToFile(sbx::TPath const & aPath)
    {
        sbx::CResource lRes;
        lRes.AddPair("posx", mPosX);
        lRes.AddPair("posy", mPosY);
        lRes.AddPair("sizex", mSizeX);
        lRes.AddPair("sizey", mSizeY);
        lRes.AddPair("maximized", mIsMaximized);
        sbx::sbxResToJsonFile(aPath, lRes);
    }
} // sbx