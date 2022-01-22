/*
 * @file    CRawBuffer.h
 * @author  David Gallardo Moreno
 */

#ifndef _SBX_RAW_BUFFER_H_
#define _SBX_RAW_BUFFER_H_

#include <sbx/Library/FRefCounted.h>

namespace sbx
{    
    class CRawBuffer : public FRefCounted
    {
        SBX_REFCOUNTED_MAKER(CRawBuffer)
    public:
        CRawBuffer()
            : mByteArray(0)
            , mSize(0)
        {}

        CRawBuffer(size_t aSize, const void* aOtherByteArray = 0)
            : mByteArray(0)
            , mSize(0)
        {
            Init(aSize, aOtherByteArray);
        }

        CRawBuffer(CRawBuffer const & aOtherRawBuffer)
            : mByteArray(0)
            , mSize(0)
        {
            Init(aOtherRawBuffer.mSize, aOtherRawBuffer.mByteArray);
        }

        CRawBuffer(CRawBuffer && aOther)
            : mByteArray(aOther.mByteArray)
            , mSize(aOther.mSize)
        {
            aOther.mByteArray = nullptr;
            aOther.mSize = 0;
        }

        CRawBuffer& operator=(CRawBuffer const & aOtherRawBuffer)
        {
            Init(aOtherRawBuffer.mSize, aOtherRawBuffer.mByteArray);
            return *this;
        }

        ~CRawBuffer() 
        {   
            if(mByteArray)
            {
                SBX_FREE(mByteArray);
                mByteArray = 0;
            }
        }

        void Init(size_t aSize, const void* aOtherByteArray = 0)
        {
            if(0 != mByteArray)
            {
                SBX_FREE(mByteArray);
                mByteArray = 0;
            }

            mByteArray = reinterpret_cast< uint8_t* >(SBX_MALLOC(aSize));
            mSize = aSize;
            if(0 != aOtherByteArray)
            {
                ::memcpy(mByteArray, aOtherByteArray, mSize);
            }
            else
            {
                ::memset(mByteArray, 0, mSize);
            }
        }

        void Realloc(size_t aSize)
        {
            if(aSize > mSize)
            {
                mSize = aSize;
                mByteArray = reinterpret_cast< uint8_t* >(SBX_REALLOC(mByteArray, aSize));
            }
        }

        uint8_t*        GetByteArray()          { return mByteArray;    }
        uint8_t*        GetByteArray() const    { return mByteArray;    }
        size_t          GetSize     () const    { return mSize;         }
    private:
        uint8_t* mByteArray;
        size_t   mSize;
    };
} // sbx

#endif // _SBX_RAW_BUFFER_H_