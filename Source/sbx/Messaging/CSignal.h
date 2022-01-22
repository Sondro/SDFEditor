/*
 * @file Signal.h
 * @author http://simmesimme.github.io/tutorials/2015/09/20/signal-slot
 */

#ifndef SIGNAL_HPP
#define SIGNAL_HPP

#include <sbx/Utils/LangUtils.h>
#include <functional>
#include <map>

// A signal object may call multiple slots with the
// same signature. You can connect functions to the signal
// which will be called when the emit() method on the
// signal object is invoked. Any argument passed to emit()
// will be passed to the given functions.

    template <typename... Args>
    class CSignal 
    {
    public:

        CSignal() : mCurrentId(0) {}

        // avoid copies
        CSignal(CSignal const& aOther) = delete;
        CSignal& operator=(CSignal const& aOther) = delete;

        // connects a member function to this Signal
        template <typename T>
        int ConnectMember(T *aInstance, void (T::*aFunction)(Args...)) 
        {
            return Connect([=](Args... args) 
            { 
                (aInstance->*aFunction)(args...); 
            });
        }

        // connects a const member function to this Signal
        template <typename T>
        int ConnectMember(T *aInstance, void (T::*aFunction)(Args...) const) 
        {
            return Connect([=](Args... args) 
            {
                (aInstance->*aFunction)(args...); 
            });
        }

        // connects a std::function to the signal. The returned
        // value can be used to disconnect the function again
        int Connect(std::function<void(Args...)> const& aSlot) const 
        {
            mSlots.insert(MakePair(++mCurrentId, aSlot));
            return mCurrentId;
        }

        // disconnects a previously connected function
        void Disconnect(uint32_t aId) const 
        {
            mSlots.erase(aId);
        }

        // disconnects all previously connected functions
        void DisconnectAll() const 
        {
            mSlots.clear();
        }

        // calls all connected functions
        void Emit(Args... p) 
        {
            for(auto lIt : mSlots) 
            {
                lIt.second(p...);
            }
        }

    private:
        mutable std::map< uint32_t, std::function<void(Args...)> > mSlots;
        mutable uint32_t mCurrentId;
    };

#endif /* SIGNAL_HPP */
