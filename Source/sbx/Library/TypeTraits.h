/*
 * @file    TypeTraits.h
 * @author  David Gallardo Moreno
 * @brief   Library type traits
 */

#ifndef __SBX_LIBRARY_TYPE_TRAITS_H__
#define __SBX_LIBRARY_TYPE_TRAITS_H__

namespace __nstl_impl
{
    template <typename T>
	struct FRemoveRef
	{
	    using Type = T;
	};

    template <typename T>
	struct FRemoveRef<T&>
    {
	    using Type = T;
	};

    template <typename T>
	struct FRemoveRef<T&&>
	{
	    using Type = T;
	};
} // __nstl_impl

namespace sbx
{
    //Removes reference from a type
    template <typename T> 
    using RemoveRef = typename __nstl_impl::FRemoveRef<T>::Type;

    
    // FUNCTION TEMPLATE forward
    template<class _Ty>
	constexpr _Ty&& Forward(RemoveRef<_Ty>& _Arg) noexcept
	{   // forward an lvalue as either an lvalue or an rvalue
	    return (static_cast<_Ty&&>(_Arg));
	}

    template<class _Ty>
	constexpr _Ty&& Forward(RemoveRef<_Ty>&& _Arg) noexcept
	{   // forward an rvalue as an rvalue
	    return (static_cast<_Ty&&>(_Arg));
	}

    //Converts to move
    template <typename T> 
    RemoveRef<T>&& MoveCast(T&& aObj){ return static_cast<RemoveRef<T>&&>(aObj); }

    template <typename T>
    void Swap(T&& aObjA, T&& aObjB)
    {
        RemoveRef<T> lTemp(MoveCast(aObjA));
        aObjA = MoveCast(aObjB);
        aObjB = MoveCast(lTemp);
    }

     
    //dgallardo: te intention here is use this in if constexpr in FVector to make a Pod/NoPod implementation of copy functions.
    template <typename T> constexpr bool is_pod  ()  { return false; };
    template <> constexpr bool is_pod<uint8_t>   ()  { return true;  };
    template <> constexpr bool is_pod<uint16_t>  ()  { return true;  };
    template <> constexpr bool is_pod<uint32_t>  ()  { return true;  };
    template <> constexpr bool is_pod<uint64_t>  ()  { return true;  };
    template <> constexpr bool is_pod<float>     ()  { return true;  };
    template <> constexpr bool is_pod<double>    ()  { return true;  };
    
} // sbx

#endif // __SBX_LIBRARY_TYPE_TRAITS_H__