/*
 * @file    CApplication.h
 * @author  David Gallardo Moreno
 */

#ifndef SBX_APPLICATION
#define SBX_APPLICATION

#include <sbx/Utils/LangUtils.h>
#include <sbx/System/SystemMessages.h>
#include <sbx/System/TWindowStatus.h>
#include <sbx/FileSystem/FileTypes.h>

namespace sbx
{
    class CChannel;
    class CApplication : public CSingleton< CApplication >
    {
    public:
                        CApplication    ();
                       ~CApplication    ();

        bool            Init            (void* aNativeHandle);
        void            End             ();

        CChannel*       GetChannel          () const;
        const char*     GetWorkingDirectory () const;

        void            Update          (double aElapsedFrameMillis);
        void            Render          ();

        static bool     IsMainThread    ();

        void            ShutDown            ();
        bool            IsRunning           () const;

        void*           GetNativeHandle     () const;
        TWindowStatus*  GetMainWindowStatus ();

        bool            GetBoolConfigParam      (TUniqueId const & aParam, bool aDefault) const;
        int32_t         GetIntegerConfigParam   (TUniqueId const & aParam, int32_t aDefault) const;
        float           GetFloatConfigParam     (TUniqueId const & aParam, float aDefault) const;
        TUniqueIdCS     GetStringConfigParam    (TUniqueId const & aParam, TUniqueIdCS aDefault) const;

        template <typename T>
        T           GetConfigParam              (TUniqueId const & aParam, T aDefault) const { return T(); };

    private:
        struct TData;
        TData* mData;
    };

    template <> bool        CApplication::GetConfigParam<bool>          (TUniqueId const & aParam, bool         aDefault) const;
    template <> int32_t     CApplication::GetConfigParam<int32_t>       (TUniqueId const & aParam, int32_t      aDefault) const;
    template <> float       CApplication::GetConfigParam<float>         (TUniqueId const & aParam, float        aDefault) const;
    template <> TUniqueIdCS CApplication::GetConfigParam<TUniqueIdCS>   (TUniqueId const & aParam, TUniqueIdCS  aDefault) const;} //sbx

#endif
