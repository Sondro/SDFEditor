
#include <sbx/Common.h>
#include <sbx/FileSystem/ResourceUtils.h>
#include <cJSON/cJSON.h>
#include <sbx/FileSystem/CJsonRes.h>

#include <iostream>
#include <sstream>
#include <fstream>
#include <iomanip>

namespace
{
    void __IterateResLogToStream(sbx::CResource const & aRes, std::ostringstream & aStream, bool aPrettyPrint, uint32_t aTabs)
    {
        SBX_ASSERT(aTabs <= 64, "[sbxDumpRes] Too many nested dictionaries in the Resource");
        if(aTabs > 64) return;

        //TODO:: add spaces, continue here
        char lBuff[256];
        char lBuff2[256];
        if(aPrettyPrint) 
        {
            memset(lBuff, ' ', aTabs * 4);
            lBuff[aTabs * 4] = '\0';
            memset(lBuff2, ' ', (aTabs + 1) * 4);
            lBuff2[(aTabs + 1) * 4] = '\0';
        }

        switch ( aRes.GetType() )
        {
        case sbx::EResourceType::eArray:
            aStream << "[";
            if(aPrettyPrint) aStream << "\n";
            for(uint32_t i = 0; i < aRes.Count(); ++i)
            {
                if(aPrettyPrint) aStream << lBuff2;
                __IterateResLogToStream(aRes.ArrayAt(i), aStream, aPrettyPrint, aTabs + 1);
                if((i + 1) < aRes.Count())
                {
                    aStream << ",";
                    //TODO: jump only in arrays / dicts
                }
                if(aPrettyPrint) aStream << "\n";
            }
            
            if(aPrettyPrint) aStream << lBuff;
            aStream << "]";
            break;
        case sbx::EResourceType::eDictionary:
            aStream << "{";
            if(aPrettyPrint) aStream << "\n";
            for(uint32_t i = 0; i < aRes.Count(); ++i)
            {
                if(aPrettyPrint) aStream << lBuff2;
                aStream << "\"";
                aStream << aRes.DictKeyAt(i).GetStr();
                aStream << "\":";
                if(aPrettyPrint) aStream << " ";
                __IterateResLogToStream(aRes.DictValueAt(i), aStream, aPrettyPrint, aTabs + 1);
                if((i + 1) < aRes.Count())
                {
                    aStream << ",";
                    //TODO: jump only in arrays / dicts
                }
                if(aPrettyPrint) aStream << "\n";
            }
            if(aPrettyPrint) aStream << lBuff;
            aStream << "}";
            break;
        case sbx::EResourceType::eString:   aStream << "\""; aStream << aRes.AsConstStr(); aStream << "\""; break;
        case sbx::EResourceType::eUInt32:   sprintf(lBuff, "%u",    aRes.AsUInt32());   aStream << lBuff;  break;
        case sbx::EResourceType::eInt32:    sprintf(lBuff, "%d",    aRes.AsInt32());    aStream << lBuff;   break;
        case sbx::EResourceType::eFloat:    sprintf(lBuff, "%f",    aRes.AsFloat());    aStream << lBuff;  break;
        case sbx::EResourceType::eUInt64:   sprintf(lBuff, "%llu",  aRes.AsUInt64());   aStream << lBuff; break;
        case sbx::EResourceType::eInt64:    sprintf(lBuff, "%lld",  aRes.AsInt64());    aStream << lBuff; break;
        case sbx::EResourceType::eDouble:   sprintf(lBuff, "%f",    aRes.AsDouble());   aStream << lBuff; break;
        case sbx::EResourceType::eBoolean:  sprintf(lBuff, "%s",    aRes.AsBool() ? "true" : "false" ); aStream << lBuff; break;
        }
    }

    void __IterateResLog(sbx::CResource const & aRes, std::string & aLog, bool aPrettyPrint, uint32_t aTabs)
    {
        SBX_ASSERT(aTabs <= 64, "[sbxDumpRes] Too many nested dictionaries in the Resource");
        if(aTabs > 64) return;

        //TODO:: add spaces, continue here
        char lBuff[256];
        char lBuff2[256];
        if(aPrettyPrint) 
        {
            memset(lBuff, ' ', aTabs * 4);
            lBuff[aTabs * 4] = '\0';
            memset(lBuff2, ' ', (aTabs + 1) * 4);
            lBuff2[(aTabs + 1) * 4] = '\0';
        }

        switch ( aRes.GetType() )
        {
        case sbx::EResourceType::eArray:
            aLog += "[";
            if(aPrettyPrint) aLog += "\n";
            for(uint32_t i = 0; i < aRes.Count(); ++i)
            {
                if(aPrettyPrint) aLog += lBuff2;
                __IterateResLog(aRes.ArrayAt(i), aLog, aPrettyPrint, aTabs + 1);
                if((i + 1) < aRes.Count())
                {
                    aLog += ",";
                    //TODO: jump only in arrays / dicts
                }
                if(aPrettyPrint) aLog += "\n";
            }
            
            if(aPrettyPrint) aLog += lBuff;
            aLog += "]";
            break;
        case sbx::EResourceType::eDictionary:
            aLog += "{";
            if(aPrettyPrint) aLog += "\n";
            for(uint32_t i = 0; i < aRes.Count(); ++i)
            {
                if(aPrettyPrint) aLog += lBuff2;
                aLog += "\"";
                aLog += aRes.DictKeyAt(i).GetStr();
                aLog += "\":";
                if(aPrettyPrint) aLog += " ";
                __IterateResLog(aRes.DictValueAt(i), aLog, aPrettyPrint, aTabs + 1);
                if((i + 1) < aRes.Count())
                {
                    aLog += ",";
                    //TODO: jump only in arrays / dicts
                }
                if(aPrettyPrint) aLog += "\n";
            }
            if(aPrettyPrint) aLog += lBuff;
            aLog += "}";
            break;
        case sbx::EResourceType::eString: aLog += "\""; aLog += aRes.AsConstStr(); aLog += "\""; break;
        case sbx::EResourceType::eUInt32: sprintf(lBuff, "%u", aRes.AsUInt32()); aLog += lBuff;  break;
        case sbx::EResourceType::eInt32:  sprintf(lBuff, "%d", aRes.AsInt32()); aLog += lBuff;   break;
        case sbx::EResourceType::eFloat:  sprintf(lBuff, "%f", aRes.AsFloat()); aLog += lBuff;  break;
        case sbx::EResourceType::eUInt64: sprintf(lBuff, "%llu", aRes.AsUInt64()); aLog += lBuff; break;
        case sbx::EResourceType::eInt64:  sprintf(lBuff, "%lld", aRes.AsInt64()); aLog += lBuff; break;
        case sbx::EResourceType::eDouble: sprintf(lBuff, "%f", aRes.AsDouble()); aLog += lBuff; break;
        case sbx::EResourceType::eBoolean: sprintf(lBuff, "%s", aRes.AsBool() ? "true" : "false" ); aLog += lBuff; break;
        }
    }

    bool __sbxIsCharNumber(char aChar)
    {
        static const char sNumbers[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9' , '.', '-'};
        for(uint32_t lNumIdx = 0; lNumIdx < sizeof(sNumbers); ++lNumIdx)
        {
            if(aChar == sNumbers[lNumIdx])
            {
                return true;
            }
        }

        return false;
    }

    const char* __IterateStringToRes(const char* aStr, sbx::CResource & aOutRes)
    {
        using namespace sbx;
        //TODO: dgallardo, algo mal quí
        if((aStr == nullptr) || (*aStr == '\0')) return aStr;

        while(*aStr != '\0')
        {
            /*
            if(*aStr == '/' && *(aStr+1) == '/') //commented line
            {
                do{ aStr++; } while(*aStr != '\n' && *aStr != '\0');
                if(*aStr == '\n')
                {
                    aStr++;
                }
            }
            else if(*aStr == '/' && *(aStr+1) == '*') //commented block
            {
                do{ aStr++; } while( *aStr != '\0' && (*aStr != '*' || *(aStr+1) != '/'));
                if(*aStr == '*' && *(aStr+1) == '/')
                {   
                    aStr += 2;
                }
            }
            else */
            if(*aStr == ' ' || *aStr == '\t' || *aStr == '\n' || *aStr == ',' ||  *aStr == ':') //space or new line
            {
                ++aStr;
            }
            else if(*aStr == '{') 
            { //start dictionary
                aOutRes = CResource::CreateDictionary();
                do
                {
                    TUniqueIdCS lKey;
                    while((*aStr != '"' || *(aStr-1) == '\\') && (*aStr != '\0') && (*aStr != '}')) ++aStr;
                    if(*aStr == '"')
                    {
                        const char* lStrStart = aStr+1;
                        do{ ++aStr; }while((*aStr != '"' || *(aStr-1) == '\\') && (*aStr != '\0') && (*aStr != '}'));
                        if(*aStr == '"')
                        {
                            std::string lKeyStr(lStrStart, aStr - lStrStart);
                            lKey = TUniqueIdCS(lKeyStr.c_str());
                        }
                    }
                    
                    ++aStr;
                    if(!lKey.IsEmpty())
                    {
                        CResource lEntry;
                        aStr = __IterateStringToRes(aStr, lEntry);
                        if(lEntry.IsOk())
                        {
                            aOutRes.AddPair(lKey, lEntry);
                        }
                    }
                }while(*aStr != '}' && (*aStr != '\0'));
                return aStr;
            }
            else if(*aStr == '}') 
            { //end dictionary
                return aStr+1;
            }
            else if(*aStr == '[') 
            { //start array
                aOutRes = CResource::CreateArray();
                do
                {
                    ++aStr;
                    CResource lEntry;
                    aStr = __IterateStringToRes(aStr, lEntry);
                    if(lEntry.IsOk())
                    {
                        aOutRes.PushBack(lEntry);
                    }
                }
                while((*aStr != ']') && (*aStr != '\0'));
                return aStr;
            }
            else if(*aStr == ']') 
            { //end array
                return aStr+1;
            }
            else if(*aStr == '"') 
            { //string
                const char* lStrStart = aStr+1;
                do{ ++aStr; }while((*aStr != '"' || *(aStr-1) == '\\') && (*aStr != '\0'));
                if(*aStr == '"')
                {
                    //std::string lStr(lStrStart, aStr - lStrStart);
                    aOutRes = CResource(lStrStart, size_t(aStr - lStrStart));
                    return aStr+1;
                }
            }
            else if(*aStr == 'n') //null
            {
                aOutRes = CResource();
                return aStr + 4;
            }
            else if(__sbxIsCharNumber(*aStr))
            { //number or bool
                double lNumber = 0;
                ::sscanf(aStr, "%lf", &lNumber);

                if(lNumber == double(uint64_t(lNumber)))
                {
                    if(uint32_t(lNumber) == uint64_t(lNumber))
                    {
                        aOutRes = uint32_t(lNumber);
                    }
                    else
                    {
                        aOutRes = uint64_t(lNumber);
                    }
                }
                else if(::fabs(lNumber - float(lNumber)) < 0.0000001)
                {
                    aOutRes = float(lNumber);
                }
                else
                {
                    aOutRes = lNumber;
                }

                //aOutRes = std::stod (aStr);
                const char* lNumberStart = aStr;
                do{ aStr++; } while(__sbxIsCharNumber(*aStr));
                return aStr;
            } 
            else if (*aStr == 't' || *aStr == 'f')
            { // booleans
                if(strncmp("true", aStr, 4) == 0)
                {
                    aOutRes = true;
                    return aStr + 4;
                }
                else if(strncmp("false", aStr, 5) == 0)
                {
                    aOutRes = false;
                    return aStr + 5;
                }
                else
                {
                    SBX_ERROR("Error parsing JSON file, expected true or false");
                }
                return aStr;
            }
            else
            { //ERROR
                SBX_ERROR("Error parsing JSON file, unexpected character %c", *aStr);
                return aStr;
            }
        }

        return aStr;
    }

    void __JsonToRes(sbx::CJsonRes const & aJson, sbx::CResource & aRes)
    {
        using namespace sbx;

        if(aJson.is_object())
        {
            for(CJsonRes::const_iterator lIt = aJson.begin(); lIt != aJson.end(); lIt++)
            {
                CResource lRes;
                __JsonToRes(lIt.value(), lRes);
                aRes.AddPair(lIt.key().c_str(), lRes);
            }
        }
        else if(aJson.is_array())
        {
            for(CJsonRes::const_iterator lIt = aJson.begin(); lIt != aJson.end(); lIt++)
            {
                CResource lRes;
                __JsonToRes(*lIt, lRes);
                aRes.PushBack(lRes);
            }
        }
        else if(aJson.is_string())
        {
            aRes = aJson.get<std::string>().c_str();
        }
        else if(aJson.is_number())
        {
            //dgallardo: 0.3.3 -> 0.3.5 fix
            double lDouble = aJson.get<double>();
            if((lDouble > 0.0) && (lDouble < 0.0000001))
            {
                float lFloat = aJson.get<float>();
                aRes = *reinterpret_cast<uint32_t*>(&lFloat);
            }
            else
            {
                aRes = lDouble;
            }
        }
        else if(aJson.is_boolean())
        {
            aRes = aJson.get<bool>();
        }
        else
        {
            //is null
        }
    }
}

namespace sbx
{
    void sbxDumpRes(CResource const & aRes)
    {
        std::ostringstream lLog;
        __IterateResLogToStream(aRes, lLog, true, 0);
        SBX_LOG(lLog.str().c_str());
    }

    void sbxResToJson(CResource const & aInRes, std::string & aOutJson, bool aPrettyPrint)
    {
        std::ostringstream lLog;
        __IterateResLogToStream(aInRes, lLog, true, 0);
        aOutJson = lLog.str();
    }

    const char* sbxJsonToRes(const char* aStr, CResource & aOutRes)
    {
        return __IterateStringToRes(aStr, aOutRes);
    }

    void sbxJsonFileToRes(TPath const & aPath, CResource & aOutRes)
    {
        std::ifstream lFileStream(aPath.GetStr());
        /*std::string lJsonStr( (std::istreambuf_iterator<char>(lFileStream) ),
                                (std::istreambuf_iterator<char>()    ) );*/
        CJsonRes lJsonRes;
        lFileStream >> lJsonRes;
        lFileStream.close();
        __JsonToRes(lJsonRes, aOutRes);
    }

    void sbxResToJsonFile(TPath const & aPath, CResource const & aRes)
    {
        std::ostringstream lString;
        __IterateResLogToStream(aRes, lString, true, 0);
        std::ofstream lJsonFile(aPath.GetStr());
        lJsonFile << std::setw(4) << lString.str() << std::endl;
        lJsonFile.close();
    }


} // sbx