/*
 * @file    File.h
 * @author  David Gallardo Moreno
 */

#ifndef __SBX_FILE_H__
#define __SBX_FILE_H__

#include <string>
#include <functional>

namespace sbx
{
    struct TPath
    {
    public:
        TPath()
        {
        };

        explicit TPath(const char* aPath)
        {
            AppendMe(aPath);
        }

        TPath& operator+(const char * aPath)
        {
            return Append(aPath);
        }

        operator const char*() const
        {
            return mStr.c_str();
        }

        const char* GetStr() const
        {
            return mStr.c_str();
        }

        const char* GetExtensionStr () const;
        TUniqueIdCS GetExtension    () const;
        const char* GetFileNameStr  () const;
        TUniqueIdCS GetFileName     () const;
        TUniqueIdCS GetFileNameNoExt() const;
        bool        HaveExtension   (const char* aExtension) const;
        bool        IsFile          () const;
        bool        IsDirectory     () const;
        bool        IsEmpty         () const;
        uint64_t    GetLastWriteTime() const;

        TPath       Append          (const char * aPathStr) const;
        TPath &     AppendMe        (const char * aPathStr);
        TPath       AppendExtension (const char * aExtension) const;
        TPath       GetParentDirectory() const;
    private:
        void        UpdateExtensionAndFilename ();

    private:
        std::string mStr;
        TUniqueIdCS mExtension;
        TUniqueIdCS mFileName;
        TUniqueIdCS mFileNameNoExt;
        //TODO: change this implementation to store paths similar to TUniqueId
    };

    class CFileSystem
    {
        typedef std::function<void(TPath const &)> TIterateDirFnc;
    public:
        static TPath    WorkingPath         ();
        static void     IterateDirectory    (TPath const & aPath, TIterateDirFnc const & aFnc);
    };
};




#endif //__SBX_FILE_H__