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

        for (auto& lStroke : mScene.mStrokesArray)
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

        //mScene.mGlobalMaterial.surfaceColor
        ordered_json& lDocMaterial = lDoc["material"];

        lDocMaterial["mat_surface"] = ordered_json::array({ mScene.mGlobalMaterial.surfaceColor.x,
                                                            mScene.mGlobalMaterial.surfaceColor.y, 
                                                            mScene.mGlobalMaterial.surfaceColor.z,
                                                            mScene.mGlobalMaterial.surfaceColor.w });

        lDocMaterial["mat_fresnel"] = ordered_json::array({ mScene.mGlobalMaterial.fresnelColor.x,
                                                            mScene.mGlobalMaterial.fresnelColor.y,
                                                            mScene.mGlobalMaterial.fresnelColor.z,
                                                            mScene.mGlobalMaterial.fresnelColor.w });

        lDocMaterial["mat_ao"] = ordered_json::array({  mScene.mGlobalMaterial.aoColor.x,
                                                        mScene.mGlobalMaterial.aoColor.y,
                                                        mScene.mGlobalMaterial.aoColor.z,
                                                        mScene.mGlobalMaterial.aoColor.w });

        lDocMaterial["mat_background"] = ordered_json::array({  mScene.mGlobalMaterial.backgroundColor.x,
                                                                mScene.mGlobalMaterial.backgroundColor.y,
                                                                mScene.mGlobalMaterial.backgroundColor.z,
                                                                mScene.mGlobalMaterial.backgroundColor.w });

        lDocMaterial["mat_pbr"] = ordered_json::array({ mScene.mGlobalMaterial.pbr.x,
                                                        mScene.mGlobalMaterial.pbr.y,
                                                        mScene.mGlobalMaterial.pbr.z,
                                                        mScene.mGlobalMaterial.pbr.w });

        lDocMaterial["mat_light_a"] = ordered_json::array({ mScene.mGlobalMaterial.lightAColor.x,
                                                            mScene.mGlobalMaterial.lightAColor.y,
                                                            mScene.mGlobalMaterial.lightAColor.z,
                                                            mScene.mGlobalMaterial.lightAColor.w });

        lDocMaterial["mat_light_b"] = ordered_json::array({ mScene.mGlobalMaterial.lightBColor.x,
                                                            mScene.mGlobalMaterial.lightBColor.y,
                                                            mScene.mGlobalMaterial.lightBColor.z,
                                                            mScene.mGlobalMaterial.lightBColor.w });

        //save json
        std::ofstream lOutputFile(mFilePath);
        lOutputFile << std::setw(4) << lDoc << std::endl;
        SetPendingChanges(false, true);
    }
}

void CSceneDocument::Load()
{
    using namespace nlohmann;

    if (!HasFilePath())
    {
        SBX_ERROR("Trying to load a document with no file path!");
        return;
    }

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

#   define JSON_STROKE_CHECK(_obj, _field, _exp) try{ if(_obj.find(_field) != _obj.end()){ _exp; } } catch(detail::exception e){ SBX_ERROR("Error reading entry [%u], field [%s]: %s", i, _field, e.what()); }

    auto lStrokesIt = lJsonData.find("strokes");
    if (lStrokesIt != lJsonData.end() && lStrokesIt->is_array())
    {
        // json deserialization
        const json& lJsonStrokes = *lStrokesIt;

        mScene.mStrokesArray.clear();
        mScene.mSelectedItems.clear();
        mScene.mStrokesArray.reserve(lJsonStrokes.size());

        for (size_t i = 0, l = lJsonStrokes.size(); i < l; i++ )
        {
            const json& lJsonStroke = lJsonStrokes[i];

            TStrokeInfo lStroke;

            JSON_STROKE_CHECK(lJsonStroke, "name", ::strncpy(lStroke.mName, (lJsonStroke["name"].get<std::string>().length() > 0) ? lJsonStroke["name"].get<std::string>().c_str() : "EmptyName", TStrokeInfo::MAX_NAME_SIZE));
            JSON_STROKE_CHECK(lJsonStroke, "position", ::memcpy(&lStroke.posb, lJsonStroke["position"].get<std::array<float, 3>>().data(), sizeof(float) * 3));
            JSON_STROKE_CHECK(lJsonStroke, "rotation", ::memcpy(&lStroke.mEulerAngles, lJsonStroke["rotation"].get<std::array<float, 3>>().data(), sizeof(float) * 3));
            JSON_STROKE_CHECK(lJsonStroke, "scale", ::memcpy(&lStroke.param0, lJsonStroke["scale"].get<std::array<float, 3>>().data(), sizeof(float) * 3));
            JSON_STROKE_CHECK(lJsonStroke, "blend", lStroke.posb.w = lJsonStroke["blend"].get<float>());
            JSON_STROKE_CHECK(lJsonStroke, "round", lStroke.param0.w = lJsonStroke["round"].get<float>());
            JSON_STROKE_CHECK(lJsonStroke, "primitive_id", lStroke.id.x = GetPrimitiveCodeByName(lJsonStroke["primitive_id"].get<std::string>()));
            lStroke.id.y = 0;
            JSON_STROKE_CHECK(lJsonStroke, "operation", lStroke.id.y |= GetOperationCodeByName(lJsonStroke["operation"].get<std::string>()));
            JSON_STROKE_CHECK(lJsonStroke, "mirror_x", lStroke.id.y |= (lJsonStroke["mirror_x"].get<bool>()) ? EStrokeOp::OpMirrorX : 0);
            JSON_STROKE_CHECK(lJsonStroke, "mirror_y", lStroke.id.y |= (lJsonStroke["mirror_y"].get<bool>()) ? EStrokeOp::OpMirrorY : 0);

            mScene.mStrokesArray.emplace_back(lStroke);
        }
    }

#   define JSON_MAT_CHECK(_obj, _field, _exp) try{ if(_obj.find(_field) != _obj.end()){ _exp; } } catch(detail::exception e){ SBX_ERROR("Error reading entry material field [%s]: %s", _field, e.what()); }

    auto lMatIt = lJsonData.find("material");
    if (lMatIt != lJsonData.end() && lMatIt->is_object())
    {
        json& lMatJson = *lMatIt;
        auto& lMat = mScene.mGlobalMaterial;
        JSON_MAT_CHECK(lMatJson, "mat_surface", ::memcpy(&lMat.surfaceColor, lMatJson["mat_surface"].get<std::array<float, 4>>().data(), sizeof(float) * 4));
        JSON_MAT_CHECK(lMatJson, "mat_fresnel", ::memcpy(&lMat.fresnelColor, lMatJson["mat_fresnel"].get<std::array<float, 4>>().data(), sizeof(float) * 4));
        JSON_MAT_CHECK(lMatJson, "mat_ao",      ::memcpy(&lMat.aoColor, lMatJson["mat_ao"].get<std::array<float, 4>>().data(), sizeof(float) * 4));
        JSON_MAT_CHECK(lMatJson, "mat_background", ::memcpy(&lMat.backgroundColor, lMatJson["mat_background"].get<std::array<float, 4>>().data(), sizeof(float) * 4));
        JSON_MAT_CHECK(lMatJson, "mat_pbr", ::memcpy(&lMat.pbr, lMatJson["mat_pbr"].get<std::array<float, 4>>().data(), sizeof(float) * 4));
        JSON_MAT_CHECK(lMatJson, "mat_light_a", ::memcpy(&lMat.lightAColor, lMatJson["mat_light_a"].get<std::array<float, 4>>().data(), sizeof(float) * 4));
        JSON_MAT_CHECK(lMatJson, "mat_light_b", ::memcpy(&lMat.lightBColor, lMatJson["mat_light_b"].get<std::array<float, 4>>().data(), sizeof(float) * 4));
    }
    else
    {
        mScene.mGlobalMaterial = TGlobalMaterialBufferData();
    }

    mScene.SetDirty();
    mScene.SetMaterialDirty();
    mScene.mStack->Reset();
    mScene.mStack->PushState(EPushStateFlags::EPE_ALL);
    SetPendingChanges(false, true);
}
