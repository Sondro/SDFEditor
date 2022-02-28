
#include "SceneDocument.h"
#include "SceneData.h"

#include <SDFEditor/Utils/FileIO.h>

#include <fstream>
#include <iomanip>

#include <ThirdParty/nlohmann/json.hpp>

namespace
{
    //stroke serialization
    static const char* sOpNames[] =
    {
        "add",
        "subtract",
        "intersect",
        "replace"
    };

    static const std::map<std::string, EStrokeOp::Type> sOpMap
    {
        { sOpNames[EStrokeOp::OpAdd], EStrokeOp::OpAdd },
        { sOpNames[EStrokeOp::OpSubtract], EStrokeOp::OpSubtract },
        { sOpNames[EStrokeOp::OpIntersect], EStrokeOp::OpIntersect },
        { sOpNames[EStrokeOp::OpReplace], EStrokeOp::OpReplace },
    };

    static const char* sPrimitiveNames[] =
    {
        "box", 
        "ellipsoid", 
        "torus", 
        "capsule"
    };

    static const std::map<std::string, EPrimitive::Type> sPrimitiveMap
    {
        { sPrimitiveNames[EPrimitive::PrEllipsoid], EPrimitive::PrEllipsoid },
        { sPrimitiveNames[EPrimitive::PrBox], EPrimitive::PrBox },
        { sPrimitiveNames[EPrimitive::PrTorus], EPrimitive::PrTorus },
        { sPrimitiveNames[EPrimitive::PrCapsule], EPrimitive::PrCapsule },
    };

    const char* GetOperationNameByCode(int32_t aOperation)
    {
        return sOpNames[aOperation & EStrokeOp::OpsMaskAll];
    }

    int32_t GetOperationCodeByName(std::string& aOperationName)
    {
        const auto& lOpPair = sOpMap.find(aOperationName);
        if (lOpPair != sOpMap.end())
        {
            return lOpPair->second;
        }

        return EStrokeOp::OpAdd;
    }

    const char* GetPrimitiveNameByCode(int32_t aPrimitive)
    {
        return sPrimitiveNames[aPrimitive];
    }

    int32_t GetPrimitiveCodeByName(std::string& aPrimitiveName)
    {
        const auto& lPrimitivePair = sPrimitiveMap.find(aPrimitiveName);
        if (lPrimitivePair != sPrimitiveMap.end())
        {
            return lPrimitivePair->second;
        }

        return EPrimitive::PrBox;
    }
}

CSceneDocument::CSceneDocument(CScene& aScene)
    : mScene(aScene)
{
}

void CSceneDocument::SetPendingChanges(bool aPendingChanges, bool aForceCallback)
{ 
    if (mPendingChanges != aPendingChanges || aForceCallback)
    {
        mPendingChanges = aPendingChanges;
        if (mDocStateChangeCallback)
        {
            mDocStateChangeCallback(aPendingChanges);
        }
    }
}

void CSceneDocument::Save()
{
    if (HasFilePath())
    {
        /*size_t lStrokesBytesSize = mScene.mStorkesArray.size() * sizeof(TStrokeInfo);
        uint32_t lNumElements = mScene.mStorkesArray.size() & 0xFFFFFFFF;

        std::vector<char> lData(lStrokesBytesSize + sizeof(lNumElements));

        ::memcpy(lData.data(), &lNumElements, sizeof(lNumElements));
        ::memcpy(lData.data() + sizeof(lNumElements), mScene.mStorkesArray.data(), lStrokesBytesSize);
        */

        using namespace nlohmann;

        json lDoc;
        lDoc["version"] = 0.1;
        //lDoc["strokes"] = json::object();
        json& lDocStrokes = lDoc["strokes"];

        for (auto& lStroke : mScene.mStorkesArray)
        {
            lDocStrokes.emplace_back();
            json& lDocStroke = lDocStrokes.back();

            lDocStroke["type"] = "stroke";
            lDocStroke["name"] = lStroke.mName;
            lDocStroke["position"] = json::array({lStroke.posb.x, lStroke.posb.y, lStroke.posb.z});
            lDocStroke["rotation"] = json::array({ lStroke.mEulerAngles.x, lStroke.mEulerAngles.y, lStroke.mEulerAngles.z });
            lDocStroke["scale"] = json::array({ lStroke.param0.x, lStroke.param0.y, lStroke.param0.z });
            lDocStroke["blend"] = lStroke.posb.w;
            lDocStroke["primitive_id"] = GetPrimitiveNameByCode(lStroke.id.x); //"box|ellipsoid|torus|capsule";
            lDocStroke["operation"] = GetOperationNameByCode(lStroke.id.y & EStrokeOp::OpsMaskAll);
            lDocStroke["mirrorX"] = false;
            lDocStroke["mirrorY"] = false;
        }

        //save json
        std::ofstream lOutputFile(mFilePath);
        lOutputFile << std::setw(4) << lDoc << std::endl;
        //WriteFile(mFilePath, lData);
        SetPendingChanges(false, true);
    }
}

void CSceneDocument::Load()
{
    if (HasFilePath())
    {
        std::vector<char> lData = ReadFile(mFilePath);

        if (lData.size() > 0)
        {
            uint32_t lNumElements = 0;
            ::memcpy(&lNumElements, lData.data(), sizeof(lNumElements));

            mScene.mStorkesArray.clear();
            mScene.mSelectedItems.clear();
            mScene.mStorkesArray.resize(lNumElements);
            //TODO: json
            ::memcpy(mScene.mStorkesArray.data(), lData.data() + sizeof(lNumElements), lNumElements * sizeof(TStrokeInfo));
            mScene.SetDirty();
            mScene.mStack->Reset();
            SetPendingChanges(false, true);
        }
    }
}
