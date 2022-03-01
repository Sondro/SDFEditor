// Copyright (c) 2022 David Gallardo and SDFEditor Project
 
#pragma once

#include <glm/glm.hpp>

class SBox
{
    enum EBoxPlane
    {
        PLANE_X = 0,
        PLANE_Y = 1,
        PLANE_Z = 2
    };
public:
    
    SBox(glm::mat4 const& aModelMatrix)
        : mMin(-1.0f, -1.0f, -1.0f)
        , mMax(1.0f, 1.0f, 1.0f)
        , mModelMatrix(aModelMatrix)
    {}

    SBox(glm::vec3 const& aScale, glm::mat4 const& aModelMatrix)
        : mMin(-aScale)
        , mMax(aScale)
        , mModelMatrix(aModelMatrix)
    {}

    SBox(glm::vec3 const & aMin, glm::vec3 const & aMax, glm::mat4 const & aModelMatrix)
        : mMin(aMin)
        , mMax(aMax)
        , mModelMatrix(aModelMatrix)
    {}

    bool CheckRayIntersection(glm::vec3 aRayOrigin, glm::vec3 aRayDirection, float* aOutDist);

private:
    glm::vec3 mMin;
    glm::vec3 mMax;
    glm::mat4 mModelMatrix;
};