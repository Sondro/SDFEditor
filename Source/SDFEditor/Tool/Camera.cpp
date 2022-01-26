#include "Camera.h"

#define GLM_ENABLE_EXPERIMENTAL
#include "glm/glm.hpp"
#include "glm/gtx/quaternion.hpp"
#include <glm/gtc/matrix_transform.hpp>

//using vec3_t = glm::vec3;
//using quat_t = glm::quaternion;

glm::vec3 rotate(const glm::vec3& vect, const glm::quat& quat)
{
    glm::vec3  outVect;

    glm::vec3  u(quat.x, quat.y, quat.z);
    float   s = quat.w;
    outVect = glm::vec3(u * 2.f * glm::dot(u, vect) + vect * (s * s - glm::dot(u, u)) + glm::cross(u, vect) * 2.f * s);
    return  outVect;
}

CCamera::CCamera()
    : mOrigin(0.0f, 1.5f, 2.5f)
    , mLookAt(0.0f, 1.0f, 0.0f)
    , mViewUp(0.0f, 1.0f, 0.0f)
    , mFOV(45.0f)
    , mAspect(16.0f/9.0f)
{
    //mOrigin = glm::vec3(0.0f, -1.0f, 2.0f);
    //mLookAt = glm::vec3(0.0f, 0.0f, 0.0f);
    //mViewUp = glm::vec3(0.0f, 1.0f, 0.0f);
    //mFOV = 45.0f;
}

CCamera::~CCamera()
{
}

void CCamera::MoveFront(float lambda)
{
    const glm::vec3 front3d = glm::normalize(mLookAt - mOrigin);
    mOrigin += front3d * lambda;
    mLookAt += front3d * lambda;

    Update();
}

void CCamera::MoveUp(float lambda)
{
    const glm::vec3 wup = glm::vec3(0, 1, 0);
    glm::quat      quat = glm::angleAxis(glm::half_pi<float>(), wup);

    const glm::vec3 front3d = glm::normalize(mLookAt - mOrigin);
    const glm::vec3 right2d = rotate(front3d * glm::vec3(1, 0, 1), glm::normalize(quat));
    const glm::vec3 up2d = glm::normalize(cross(front3d, right2d));

    mOrigin += up2d * lambda;
    mLookAt += up2d * lambda;

    Update();
}

void CCamera::MoveRight(float lambda)
{
    const glm::vec3 wup = mViewUp;
    glm::quat       quat = glm::angleAxis(glm::half_pi<float>(), wup);

    const glm::vec3 front2d = glm::normalize(glm::normalize(mLookAt - mOrigin) * glm::vec3(1, 0, 1));
    const glm::vec3 right2d = rotate(front2d, glm::normalize(quat));

    mOrigin -= right2d * lambda;
    mLookAt -= right2d * lambda;

    Update();
}

/*
void CCamera::Pan(float lambda_x, float lambda_y)
{
    //vec3_t *a = &mOrigin;
    //vec3_t *b = &mLookAt;
    const glm::vec3 lookdir = glm::normalize(mLookAt - mOrigin);

    glm::quat xrot = glm::angleAxis(glm::half_pi<float>(), glm::vec3(0, 1, 0));
    glm::quat yrot = glm::angleAxis(lambda_y * glm::half_pi<float>(), glm::cross(glm::vec3(0, -1, 0), lookdir));


    float d = glm::dot(glm::vec3(0, 1, 0), lookdir);
    if ((lambda_y > 0.f) && (d > .98f))
    {
        yrot = glm::quat_identity<float, glm::highp >();
    }
    else if ((lambda_y < 0.f) && (d < -.98f))
    {
        yrot = glm::quat_identity<float, glm::highp >();
    }

    const glm::vec3 prev = glm::normalize(mLookAt - mOrigin);
    const glm::vec3 ldir = rotate(prev, glm::normalize(yrot * xrot));
    mLookAt = mOrigin + ldir * length(mLookAt - mOrigin);

    Update();
}*/

void CCamera::Pan(float lambda_x, float lambda_y)
{
    //vec3_t *a = &mOrigin;
    //vec3_t *b = &mLookAt;
    const glm::vec3 lookdir = glm::normalize(mLookAt - mOrigin);

    glm::quat xrot, yrot;
    xrot = glm::angleAxis(lambda_x * glm::half_pi<float>(), glm::vec3(0, 1, 0));
    yrot = glm::angleAxis(lambda_y * glm::half_pi<float>(), glm::cross(glm::vec3(0, -1, 0), lookdir));

    float d = glm::dot(glm::vec3(0, 1, 0), lookdir);
    if ((lambda_y > 0.f) && (d > .98f)) yrot = glm::quat(); else
        if ((lambda_y < 0.f) && (d < -.98f)) yrot = glm::quat();

    const  glm::vec3 prev = glm::normalize(mLookAt - mOrigin);
    const  glm::vec3 ldir = rotate(prev, glm::normalize(yrot * xrot));
    mLookAt = mOrigin + ldir * length(mLookAt - mOrigin);

    Update();
}

void CCamera::UpdateAspect(float aViewWidth, float aViewHeight)
{
    if (aViewWidth > 0 && aViewHeight > 0)
    {
        mViewWidth = aViewWidth;
        mViewHeight = aViewHeight;
        mAspect = aViewWidth / aViewHeight;
    }
}

glm::mat4 CCamera::GetProjectionMatrix() const
{
    return glm::perspective(mFOV, mAspect, 0.1f, 100.0f);
}

glm::mat4 CCamera::GetViewMatrix() const
{
    return glm::lookAt(mOrigin, mLookAt, mViewUp);
}

void CCamera::Update()
{
    /*float lTheta = mFOV * float(M_PI) / 180.f;
    float lHalfHeight = tanf(lTheta * 0.5f);
    float lHalfWidth = mAspect * lHalfHeight;
    mLensRadius = mAperture * 0.5f;
    mVecW = normalized(mOrigin - mLookAt);
    mVecU = normalized(cross(mViewUp, mVecW));
    mVecV = cross(mVecW, mVecU);
    mLowerLeftCorner = mOrigin - (lHalfWidth * mFocusDist * mVecU) - (lHalfHeight * mFocusDist * mVecV) - (mFocusDist * mVecW);
    mHorizontal = 2.0f * lHalfWidth * mFocusDist * mVecU;
    mVertical = 2.0f * lHalfHeight * mFocusDist * mVecV;

    mUpdated = true;*/
}
