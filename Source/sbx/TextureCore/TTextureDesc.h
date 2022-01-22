/*
 * @file    TTextureDesc.h
 * @author  David Gallardo Moreno
 */
 
namespace sbx
{
    struct TTextureDesc
    {
    public:
        enum EFormat
        {
            eFormat_R = 0,
            eFormat_RGBA,
            eFormat_DEPTH,
            eFormat_Count,
            eFormat_Invalid = -1,
        };

        enum EChannelSize
        {
            eSize_UnsignedByte = 0,
            eSize_Float16,
            eSize_Float32,
            eSize_Count,
            eSize_Invalid = -1,
        };

        enum EInternalFormat
        {
            eInternalFormat_R8 = 0,
            eInternalFormat_R16F,
            eInternalFormat_R32F,
            eInternalFormat_RGBA8,
            eInternalFormat_RGBA16F,
            eInternalFormat_RGBA32F,
            eInternalFormat_DEPTH32,
            eInternalFormat_Count,
            eInternalFormat_Invalid = -1,
        };
        
        enum ETexFlags
        {
            eFlag_None           = 0 << 0,
            eFlag_Depth          = 1 << 0,
            eFlag_Multisample    = 1 << 1,
        };

        struct TTextureFormatSizeInfo
        {
            EFormat         mFormat;
            EChannelSize    mChannelSize;
            EInternalFormat mInternalFormat;
        };

        uint32_t        mFlags;
        uint32_t        mWidth;
        uint32_t        mHeight;
        uint8_t         mComponents;
        uint8_t         mComponentBytes;
        uint8_t         mSamples; //multisample
            
        TTextureDesc ()
            : mFlags(0)
            , mWidth(1)
            , mHeight(1)
            , mComponents(4)
            , mComponentBytes(1)
            , mSamples(1)
        {;}

        TTextureDesc (uint32_t          aWidth,
                      uint32_t          aHeight,
                      uint8_t           aComponents,
                      uint8_t           aComponentBytes,
                      uint32_t          aFlags = 0,
                      uint8_t           aSamples = 1)
            : mWidth(aWidth)
            , mHeight(aHeight)
            , mComponents(aComponents)
            , mComponentBytes(aComponentBytes)
            , mFlags(aFlags)
            , mSamples(aSamples)
        {;}

        static EFormat GetFormat(EInternalFormat aInternalFormat)
        {
            static const EFormat sFormats[] =
            {
                eFormat_R,
                eFormat_R,
                eFormat_R,
                eFormat_RGBA,
                eFormat_RGBA,
                eFormat_RGBA,
                eFormat_DEPTH,
            };

            return sFormats[aInternalFormat];
        }

        size_t GetBufferSize() const
        {
            return mWidth * mHeight * mComponents * mComponentBytes;
        }

        void GetFormatSizeInfo(TTextureFormatSizeInfo & aOutFormatSizeInfo) const
        {
            aOutFormatSizeInfo.mFormat = eFormat_Invalid;
            aOutFormatSizeInfo.mChannelSize = eSize_Invalid;
            aOutFormatSizeInfo.mInternalFormat = eInternalFormat_Invalid;

            if (1 == mComponentBytes)
            {
                aOutFormatSizeInfo.mChannelSize = eSize_UnsignedByte;
                aOutFormatSizeInfo.mInternalFormat = (1 == mComponents) ? eInternalFormat_R8 : eInternalFormat_RGBA8;
            }
            else if (2 == mComponentBytes)
            {
                aOutFormatSizeInfo.mChannelSize = eSize_Float16;
                aOutFormatSizeInfo.mInternalFormat = (1 == mComponents) ? eInternalFormat_R16F : eInternalFormat_RGBA16F;
            }
            else if (4 == mComponentBytes)
            {
                aOutFormatSizeInfo.mChannelSize = eSize_Float32;
                aOutFormatSizeInfo.mInternalFormat = (1 == mComponents) ? eInternalFormat_R32F : eInternalFormat_RGBA32F;
                aOutFormatSizeInfo.mInternalFormat = (mFlags & eFlag_Depth) ? eInternalFormat_DEPTH32 : aOutFormatSizeInfo.mInternalFormat;
            }

            aOutFormatSizeInfo.mFormat = GetFormat(aOutFormatSizeInfo.mInternalFormat);
        }
    };
} // sbx