
#pragma once 

#include "glm/glm.hpp"

class CCamera
{
public:
    
    CCamera();
    ~CCamera();

    void MoveFront(float aLambda);
    void MoveUp(float aLambda);
    void MoveRight(float aLambda);
    void Pan(float aHorizontal, float aVertical);
    void UpdateAspect(float aViewWidth, float aViewHeight);

    glm::vec3 mOrigin;
    glm::vec3 mLookAt;
    glm::vec3 mViewUp;
    float mFOV;
    float mAspect;

    void Update();
};