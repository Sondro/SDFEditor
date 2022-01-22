/*
 * @file    File.cpp
 * @author  David Gallardo Moreno
 */

#include <sbx/Common.h>
#include "File.h"

#include <sbx/System/CApplication.h>
#include <filesystem>

namespace fs = std::filesystem;

namespace 
{
    void sbxReplaceAllPathBars(char* aPath)
    {
        //TODO: UTF-8 support
        while(*aPath != 0)
        {
            if(*aPath == '\\')
            {
                *aPath = '/';
            }
            aPath++;
        }
    }
}

namespace sbx
{
    void TPath::UpdateExtensionAndFilename()
    {
        const char* lStr = mStr.c_str();
        const char* lDot = ::strrchr(lStr, '.');
        const char* lFirstSlash = ::strrchr(lStr, '/');
        if(lDot && (lDot != lStr) && ((lFirstSlash == nullptr) || (lFirstSlash < lDot)))
        {
            mExtension = TUniqueIdCS(lDot + 1);
        }
        else
        {
            mExtension = TUniqueIdCS();
        }
        
        const char* lPathBar = ::strrchr(lStr, '/');
        if(lPathBar && (lPathBar != lStr) && (lPathBar != (lStr + mStr.length() - 1)))
        {
            mFileName = TUniqueIdCS(lPathBar + 1);
            //if(mFileName.GetLength() > 0)
            {
                static char lBuff[255];
                ::sprintf(lBuff, "%s", mFileName.GetStr());
                uint32_t lSubs = (mExtension.GetLength() > 0) ? 1 : 0;
                lBuff[mFileName.GetLength() - mExtension.GetLength() - lSubs] = '\0';
                mFileNameNoExt = TUniqueIdCS(lBuff);
            }
        }
        else
        {
            mFileName = TUniqueIdCS();
            mFileNameNoExt = TUniqueIdCS();
        }
    }

    const char* TPath::GetExtensionStr() const
    {
        return mExtension.GetStr();
    }

    TUniqueIdCS TPath::GetExtension() const
    {
        return mExtension;
    }

    const char* TPath::GetFileNameStr () const
    {
        return mFileName.GetStr();
    }

    TUniqueIdCS TPath::GetFileName () const
    {
        return mFileName;
    }

    TUniqueIdCS TPath::GetFileNameNoExt () const
    {
        return mFileNameNoExt;
    }

    bool TPath::HaveExtension(const char * aExtension) const
    {
        return TUniqueId(aExtension) == TUniqueId(mExtension.GetStr());
    }

    bool TPath::IsFile() const
    {
        return fs::is_regular_file(mStr.c_str());
    }
        
    bool TPath::IsDirectory() const
    {
        return fs::is_directory(mStr.c_str());
    }

    bool TPath::IsEmpty() const
    {
        return mStr.length() == 0;
    }

    uint64_t TPath::GetLastWriteTime() const
    {
        try
        {
            if(fs::exists(mStr.c_str()))
            {
                return fs::last_write_time(mStr.c_str()).time_since_epoch().count();
            }
        }
        catch (fs::filesystem_error const &)
        {
            SBX_ERROR("Error checking %s", mStr.c_str());
        }

        return 0;
    }

    TPath TPath::Append(const char * aPathStr) const
    {
        return TPath(*this).AppendMe(aPathStr);
    }

    TPath & TPath::AppendMe(const char * aPathStr)
    {
        if( (aPathStr != 0) && (*aPathStr != 0) )
        {
            //Remove extra path separators
            if( mStr.length() > 0 && 
                ( 
                    (*aPathStr == '/') || 
                    (*aPathStr == '\\') 
                ) && 
                ( 
                    (mStr.back() == '/') || 
                    (mStr.back() == '\\')
                )
            )
            {
                mStr.pop_back();
            }

            //Add path separators
            if( mStr.length() > 0 && 
                ( 
                    (*aPathStr != '/') &&
                    (*aPathStr != '\\') 
                ) && 
                ( 
                    (mStr.back() != '/') &&
                    (mStr.back() != '\\')
                )
            )
            {
                mStr += "/";
            }

            mStr += aPathStr;
            
            sbxReplaceAllPathBars(mStr.data());
            UpdateExtensionAndFilename();
        }

        return *this;
    };

    TPath TPath::AppendExtension (const char * aExtension) const
    {
        TPath lNewPath = *this;
        if((aExtension == 0) || (aExtension[0] == '\0'))
        {
            return *this;
        }

        const char* lExtensionToCheck = aExtension;

        if(lExtensionToCheck[0] == '.')
        {
            ++lExtensionToCheck;
        }

        if(HaveExtension(lExtensionToCheck))
        {
            return *this;
        }

        if((mStr.back() == '.') || (mStr.back() == '/') || (mStr.back() == '\\'))
        {
            lNewPath.mStr.pop_back();
        }

        if(aExtension[0] != '.')
        {
            lNewPath.mStr += ".";
        }

        lNewPath.mStr += aExtension;
        
        sbxReplaceAllPathBars(lNewPath.mStr.data());
        lNewPath.UpdateExtensionAndFilename();

        return lNewPath;
    }
    
    TPath TPath::GetParentDirectory() const
    {
        TPath lCopy = *this;
        if((lCopy.mStr.back() == '/') || (lCopy.mStr.back() == '\\'))
        {
            lCopy.mStr.pop_back();
        }

        while((lCopy.mStr.length()) > 0 && (lCopy.mStr.back() != '/') && (lCopy.mStr.back() != '\\'))
        {
            lCopy.mStr.pop_back();
        }

        sbxReplaceAllPathBars(lCopy.mStr.data());
        lCopy.UpdateExtensionAndFilename();

        return lCopy;
    }

    TPath CFileSystem::WorkingPath()
    {
        return TPath(CApplication::GetInstance()->GetWorkingDirectory());
    }

    void CFileSystem::IterateDirectory(TPath const & aPath, TIterateDirFnc const & aFnc)
    {
        if(!aFnc)
        {
            return;
        }
        
        fs::path lStdPath(aPath.GetStr());
        for (fs::directory_entry const & lEntry : fs::directory_iterator(lStdPath))
        {
            aFnc(TPath(lEntry.path().string().c_str()));
        }
    }
};