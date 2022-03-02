// Copyright (c) 2022 David Gallardo and SDFEditor Project

#include "SceneDocument.h"
#include "Scene.h"

#include <SDFEditor/Utils/FileIO.h>

#include <fstream>
#include <iomanip>
#include <iostream>

#include <ThirdParty/nlohmann/json.hpp>
#include <sbx/Core/ErrorHandling.h>


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
        return sOpNames[aOperation & EStrokeOp::OpsMaskMode];
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
        using namespace nlohmann;

        ordered_json lDoc;
        lDoc["version"] = 0.1;
        ordered_json& lDocStrokes = lDoc["strokes"];

        for (auto& lStroke : mScene.mStorkesArray)
        {
            lDocStrokes.emplace_back();
            ordered_json& lDocStroke = lDocStrokes.back();

            lDocStroke["type"] = "stroke";
            lDocStroke["name"] = lStroke.mName;
            lDocStroke["position"] = ordered_json::array({lStroke.posb.x, lStroke.posb.y, lStroke.posb.z});
            lDocStroke["rotation"] = ordered_json::array({ lStroke.mEulerAngles.x, lStroke.mEulerAngles.y, lStroke.mEulerAngles.z });
            lDocStroke["scale"] = ordered_json::array({ lStroke.param0.x, lStroke.param0.y, lStroke.param0.z });
            lDocStroke["blend"] = lStroke.posb.w;
            lDocStroke["round"] = lStroke.param0.w;
            lDocStroke["primitive_id"] = GetPrimitiveNameByCode(lStroke.id.x);
            lDocStroke["operation"] = GetOperationNameByCode(lStroke.id.y & EStrokeOp::OpsMaskMode);
            lDocStroke["mirror_x"] = bool(lStroke.id.y & EStrokeOp::OpMirrorX);
            lDocStroke["mirror_y"] = bool(lStroke.id.y & EStrokeOp::OpMirrorY);
        }

        //save json
        std::ofstream lOutputFile(mFilePath);
        lOutputFile << std::setw(4) << lDoc << std::endl;
        SetPendingChanges(false, true);
    }
}

void CSceneDocument::Load()
{
    if (HasFilePath())
    {
        using namespace nlohmann;

        // read a JSON file
        std::ifstream lInputFile(mFilePath);
        json lJsonData;

        try
        {
            lInputFile >> lJsonData;
        }
        catch (detail::exception e)
        {
            SBX_ERROR("Error reading file [%s]: %s", mFilePath.c_str(), e.what());
        }

        auto lStrokesIt = lJsonData.find("strokes");
        if (lStrokesIt != lJsonData.end() && lStrokesIt->is_array())
        {
            // json deserialization
            const json& lJsonStrokes = *lStrokesIt;

            mScene.mStorkesArray.clear();
            mScene.mSelectedItems.clear();
            mScene.mStorkesArray.reserve(lJsonStrokes.size());

            for (size_t i = 0, l = lJsonStrokes.size(); i < l; i++ )
            {
                const json& lJsonStroke = lJsonStrokes[i];

                TStrokeInfo lStroke;

#               define JSON_CHECK(_field, _exp) try{ if(lJsonStroke.find(_field) != lJsonStroke.end()){ _exp; } } catch(detail::exception e){ SBX_ERROR("Error reading entry [%u], field [%s]: %s", i, _field, e.what()); }

                JSON_CHECK("name", ::strncpy(lStroke.mName, (lJsonStroke["name"].get<std::string>().length() > 0) ? lJsonStroke["name"].get<std::string>().c_str() : "EmptyName", TStrokeInfo::MAX_NAME_SIZE));
                JSON_CHECK("position", ::memcpy(&lStroke.posb, lJsonStroke["position"].get<std::array<float, 3>>().data(), sizeof(float) * 3));
                JSON_CHECK("rotation", ::memcpy(&lStroke.mEulerAngles, lJsonStroke["rotation"].get<std::array<float, 3>>().data(), sizeof(float) * 3));
                JSON_CHECK("scale", ::memcpy(&lStroke.param0, lJsonStroke["scale"].get<std::array<float, 3>>().data(), sizeof(float) * 3));
                JSON_CHECK("blend", lStroke.posb.w = lJsonStroke["blend"].get<float>());
                JSON_CHECK("round", lStroke.param0.w = lJsonStroke["round"].get<float>());
                JSON_CHECK("primitive_id", lStroke.id.x = GetPrimitiveCodeByName(lJsonStroke["primitive_id"].get<std::string>()));
                lStroke.id.y = 0;
                JSON_CHECK("operation", lStroke.id.y |= GetOperationCodeByName(lJsonStroke["operation"].get<std::string>()));
                JSON_CHECK("mirror_x", lStroke.id.y |= (lJsonStroke["mirror_x"].get<bool>()) ? EStrokeOp::OpMirrorX : 0);
                JSON_CHECK("mirror_y", lStroke.id.y |= (lJsonStroke["mirror_y"].get<bool>()) ? EStrokeOp::OpMirrorY : 0);

                mScene.mStorkesArray.emplace_back(lStroke);
            }

            mScene.SetDirty();
            mScene.mStack->Reset();
            SetPendingChanges(false, true);
        }
    }
}
