/*
 * @file    Funtional.h
 * @author  David Gallardo Moreno
 */

namespace sbx
{
    
    /*
    struct TFunctionBindingBase
    {
        virtual void operator()(){};
        virtual void* GetObject(){ return nullptr; };
    };

    template <typename Fnc> struct TFunctionBinding : public TFunctionBindingBase
    {
        TFunctionBinding(Fnc* aFunctionPtr)
            : mFunctionPointer(aFunctionPtr)
        {;}

        virtual void operator()()
        {
            mFunctionPointer();
        };

        virtual void* GetObject(){ return nullptr; };

        Fnc mFunctionPointer;
    };
    
    template <typename T, typename T::Fnc*> struct TMemberFunctionBinding : public TFunctionBindingBase
    {
        TMemberFunctionBinding(T* aObject, Fnc const & aFunctionPtr) 
            : TFunctionBinding(aFunctionPtr)
            , mObject(aObject)
        {;}

        virtual void operator()()
        {
                mObject->*mFunctionPointer();
        };

        virtual void* GetObject(){ return mObject; };

        T* mObject;
        T::Fnc* mFunctionPointer;
    };

    */

    template <typename R, typename... Args> class FBinding
    {
    public:
        typedef R(*TFnc)(Args...);

        FBinding(TFnc aFnc)
            : mFnc(aFnc)
        {};

        R operator()(Args... aArgs)
        {
            return mFnc(aArgs...);
        }

    private:
        TFnc mFnc;
    };

    template <typename R, typename... Args> FBinding<R, Args...> FBind( R(*aFnc)(Args...) )
    {
        return FBinding<R, Args...>(aFnc);
    }

} // sbx