// Copyright (c) 2022 David Gallardo and SDFEditor Project

#pragma once 

#include "glm/glm.hpp"

class CCamera
{
public:
    
    CCamera();
    ~CCamera();

    CCamera(const CCamera&) = delete;
    CCamera(const CCamera&&) = delete;

    void MoveFront(float aLambda);
    void MoveUp(float aLambda);
    void MoveRight(float aLambda);
    void Pan(float aHorizontal, float aVertical);
    void UpdateAspect(float aViewWidth, float aViewHeight);

    glm::mat4 GetProjectionMatrix() const;
    glm::mat4 GetViewMatrix() const;

    glm::vec3 mOrigin;
    glm::vec3 mLookAt;
    glm::vec3 mViewUp;
    float mFOV;
    float mAspect;

    float mViewWidth;
    float mViewHeight;
};