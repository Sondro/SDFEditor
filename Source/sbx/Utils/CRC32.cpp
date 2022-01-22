//
//  CRC32.cpp
//  gtf
//
//  Created by David Gallardo on 31/05/16.
//  Copyright (c) 2016 GTF Development Group. All rights reserved.
//

#include <sbx/Common.h>
#include "CRC32.h"

namespace sbx { namespace CRC32
{
  
  //Non constexpr implementation from http://www.opensource.apple.com/source/xnu/xnu-1456.1.26/bsd/libkern/crc32.c
  uint32_t crc32Lower(const char* aBuffer, uint32_t aSize, uint32_t aCRC)
  {
    const char *p = aBuffer;
    
    while (aSize--)
    {
        //TODO: implement a case sensitive version
        aCRC = crc32_tab[(aCRC ^ ::tolower(*p++)) & 0xFF] ^ (aCRC >> 8);
    }
    
    return aCRC ^ ~0U;
  }

  uint32_t crc32(const char* aBuffer, uint32_t aSize, uint32_t aCRC)
  {
    const char *p = aBuffer;
    
    while (aSize--)
    {
        aCRC = crc32_tab[(aCRC ^ *p++) & 0xFF] ^ (aCRC >> 8);
    }
    
    return aCRC ^ ~0U;
  }
  
  uint32_t str2crc32(const char* aStr, uint32_t& aOutLen, bool aCaseSensitive)
  {      
    aOutLen = static_cast< uint32_t >(::strlen(aStr));
    //TODO: implement a case sensitive version for uniqueIdCS
    return (aCaseSensitive) ? crc32(aStr, aOutLen, ~0) : crc32Lower(aStr, aOutLen, ~0);
  }
  
}} // sbx::CRC32