/*
 * @file    CTextureUtils.cpp
 * @author  David Gallardo Moreno
 */

#include <sbx/Common.h>
#include "CTextureUtils.h"

#include <fstream>
#include <sstream>

#   define STB_IMAGE_IMPLEMENTATION
#   include <stb_image.h>

#   define STB_IMAGE_WRITE_IMPLEMENTATION
#   include <stb_image_write.h>

//#include <DataStructures/CRawBuffer.h>

namespace sbx
{
    bool CTextureUtils::IsHDR (TPath const & aPath)
    {
        return stbi_is_hdr(aPath);
    }

    void CTextureUtils::LoadFromFile (TPath const & aPath, TTexture & aTexture)
    {
        int32_t lWidth = 0, lHeight = 0, lComponents = 0;
        if (IsHDR(aPath))
        {
            float* lTextureBuffer = stbi_loadf(aPath, &lWidth, &lHeight, &lComponents, 4);
            if(lTextureBuffer)
            {
                aTexture.Init(lWidth, lHeight, 1, ETextureFormat::RGBA32F);
                ::memcpy(aTexture.mBuffer.GetByteArray(), lTextureBuffer, lWidth * lHeight * sizeof(TColor32F));
                stbi_image_free(lTextureBuffer);
            }
        }
        else
        {
            uint8_t* lTextureBuffer = stbi_load(aPath, &lWidth, &lHeight, &lComponents, 4);
            if(lTextureBuffer)
            {
                aTexture.Init(lWidth, lHeight, 1, ETextureFormat::RGBA8);
                ::memcpy(aTexture.mBuffer.GetByteArray(), lTextureBuffer, lWidth * lHeight * sizeof(TColor8U));
                stbi_image_free(lTextureBuffer);
            }
        }
    }

    void CTextureUtils::SaveToFile (TPath const & aPath, TTexture const & aTexture, EFileType aFileType, uint32_t aSlice)
    {
        switch(aFileType)
        {
        case EFileType::ePNGFile:
            stbi_write_png(aPath, aTexture.mWidth, aTexture.mHeight, aTexture.GetComponents(), aTexture.AsR8Buffer() + (aTexture.mWidth * aTexture.mHeight * aSlice * aTexture.GetComponents()), 0);
            break;
        case EFileType::eTGAFile:
            stbi_write_tga(aPath, aTexture.mWidth, aTexture.mHeight, 4, aTexture.AsRGBA8Buffer() + (aTexture.mWidth * aTexture.mHeight * aSlice));
            break;
        case EFileType::eHDRFile:
            stbi_write_hdr(aPath, aTexture.mWidth, aTexture.mHeight, 4, (float*)(aTexture.AsRGBA32FBuffer() + (aTexture.mWidth * aTexture.mHeight * aSlice)));
            break;
        default:
            SBX_ERROR("[CTextureUtils::SaveToFile] Invalid texture file type.");
        }
    }

    void CTextureUtils::LoadFromFile(TPath const & aPath, CGPUTexture::Ref & aTexture)
    {
        int32_t lWidth = 0, lHeight = 0, lComponents = 0;
        void* lData = stbi_load(aPath, &lWidth, &lHeight, &lComponents, 4);
        if(lData)
        {
            CRawBuffer::Ref lRawBuffer = CRawBuffer::CreateInstance(lWidth * lHeight * lComponents, lData);
            stbi_image_free(lData);
            CGPUTexture::Ref lTexture = CGPUTexture::CreateInstance(TTextureDesc(lWidth, lHeight, lComponents, 1));
            lTexture->CopyFromBuffer(lRawBuffer);
            aTexture = lTexture;
        }
        else
        {
            aTexture = nullptr;
        }
    };

    /*void CTextureUtils::SaveToFile(const char* aPath, CGPUTexture::Ref const & aTexture)
    {
        TTextureDesc lTexDesc = aTexture->GetDesc();
        CRawBuffer::Ref lRawBuffer = CRawBuffer::CreateInstance(lTexDesc->GetBufferSize());
        std::string lPath = aPath;
        aTexture->CopyToBuffer(lRawBuffer, [lTexDesc, lRawBuffer, lPath, aPath]()
        {
            stbi_write_png(lPath.c_str(), lTexDesc->mWidth, lTexDesc->mHeight, lTexDesc->mComponents, lRawBuffer->GetByteArray(), 0);
        });
    }*/




    bool CTextureUtils::StoreTexturePack(TPath const & aPath, FVector< TTexture > const & aTexturePack)
    {
        std::ofstream lOutput( aPath, std::ios::binary );

        uint8_t lNumTextures = aTexturePack.size() & 0xFF;
        lOutput.write((char*)&lNumTextures, sizeof(uint8_t));

        for(uint32_t iTextureIndex = 0; iTextureIndex < aTexturePack.size(); iTextureIndex++)
        {
            const TTexture& lTexture = aTexturePack[iTextureIndex];
            uint32_t lTexWidth = lTexture.mWidth;
            lOutput.write((char*)&lTexWidth, sizeof(uint32_t));
        
            uint32_t lTexHeight = lTexture.mHeight;
            lOutput.write((char*)&lTexHeight, sizeof(uint32_t));

            uint32_t lTexSlices = lTexture.mSlices;
            lOutput.write((char*)&lTexSlices, sizeof(uint32_t));

            uint32_t lTexFormat = lTexture.mFormat;
            lOutput.write((char*)&lTexFormat, sizeof(uint32_t));

            uint64_t lBufferSize = lTexture.GetSliceSize() * lTexture.mSlices;
            lOutput.write((char*)&lBufferSize, sizeof(uint64_t));

            //write all texture
            lOutput.write((char*)lTexture.mBuffer.GetByteArray(), lBufferSize);
        }

        char lEnd = 0;
        lOutput.write(&lEnd, sizeof(char));
        lOutput.close();

        return true;
    }

    bool CTextureUtils::LoadTexturePack(TPath const & aPath, FVector< TTexture > & aTexturePack)
    {
        //aTexturePack.clear();
        std::ifstream lInput( aPath, std::ios::binary );
        if(lInput.good())
        {
            uint8_t lTexNum = 0;
            lInput.read((char*)&lTexNum, sizeof(uint8_t));

            for(int32_t iTextureIndex = 0; iTextureIndex <  lTexNum; iTextureIndex++)
            {
                uint32_t lTexWidth = 0;
                lInput.read((char*)&lTexWidth, sizeof(uint32_t));

                uint32_t lTexHeight = 0;
                lInput.read((char*)&lTexHeight, sizeof(uint32_t));

                uint32_t lTexSlices = 0;
                lInput.read((char*)&lTexSlices, sizeof(uint32_t));

                uint32_t lTexFormat = 0;
                lInput.read((char*)&lTexFormat, sizeof(uint32_t));

                uint64_t lBufferSize = 0;
                lInput.read((char*)&lBufferSize, sizeof(uint64_t));

                aTexturePack.emplace_back();
                TTexture& lTexture = aTexturePack.back();
                lTexture.Init(lTexWidth, lTexHeight, lTexSlices, ETextureFormat::Type(lTexFormat));

                //Read all the texture
                lInput.read((char*)lTexture.mBuffer.GetByteArray(), lBufferSize);
            }
        
            lInput.close();
            return true;
        }
    
        lInput.close();
        return false;
    }

}