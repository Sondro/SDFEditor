
 
#pragma once

#include <glm/glm.hpp>

/*
* 
vec3 cubenormal(vec3 v) // position relative to cube center
{
    vec3 s = sign(v);
    vec3 a = abs(v);

    //vec3 n = (a.z > a.y)
    // ?
    // (a.z > a.x) ? vec3(0.0, 0.0, s.z) : vec3(s.x, 0.0, 0.0)
    // :
    // (a.y > a.x) ? vec3(0.0, s.y, 0.0) : vec3(s.x, 0.0, 0.0);

    vec3 n = mix(
        mix(vec3(0.0, 0.0, s.z), vec3(s.x, 0.0, 0.0), step(a.z, a.x)),
        mix(vec3(0.0, s.y, 0.0), vec3(s.x, 0.0, 0.0), step(a.y, a.x)),
        step(a.z, a.y));

    return n;
}

bool rayboxintersect(vec3 raypos, vec3 raydir, vec3 boxmin, vec3 boxmax, out vec3 normal, out vec2 distances)
{
    float t1 = (boxmin.x - raypos.x) / raydir.x;
    float t2 = (boxmax.x - raypos.x) / raydir.x;
    float t3 = (boxmin.y - raypos.y) / raydir.y;
    float t4 = (boxmax.y - raypos.y) / raydir.y;
    float t5 = (boxmin.z - raypos.z) / raydir.z;
    float t6 = (boxmax.z - raypos.z) / raydir.z;

    float tmin = max(max(min(t1, t2), min(t3, t4)), min(t5, t6));
    float tmax = min(min(max(t1, t2), max(t3, t4)), max(t5, t6));

    distances = vec2(tmin, tmax);

    if (tmax < 0.0) // box on ray but behind ray origin
    {
        return false;
    }

    if (tmin > tmax) // ray doesn't intersect box
    {
        return false;
    }

    normal = cubenormal(raypos + raydir * tmin - (boxmin + boxmax) / 2.0);
    return true;
}
*/

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

    bool CheckRayIntersection(glm::vec3 aRayOrigin, glm::vec3 aRayDirection, float* aOutDist)
    {
        float tMin = 0.0f;
        float tMax = 100000.0f;

        glm::vec3 lObbPosition = glm::vec3(mModelMatrix[3].x, mModelMatrix[3].y, mModelMatrix[3].z);
        glm::vec3 lDelta = lObbPosition - aRayOrigin;


        bool lResult = 
            CheckIntersectionPlane(aRayOrigin, aRayDirection, PLANE_X, lDelta, tMin, tMax) &&
            CheckIntersectionPlane(aRayOrigin, aRayDirection, PLANE_Y, lDelta, tMin, tMax) &&
            CheckIntersectionPlane(aRayOrigin, aRayDirection, PLANE_Z, lDelta, tMin, tMax);
        
        if (lResult && aOutDist)
        {
            *aOutDist = tMin;
        }

        return lResult;
    }

    bool CheckRayIntersection2(glm::vec3 aRayOrigin, glm::vec3 aRayDirection, float* aOutDist)
    {
        // Intersection method from Real-Time Rendering and Essential Mathematics for Games

        float tMin = 0.0f;
        float tMax = 100000.0f;

        glm::vec3 OBBposition_worldspace(mModelMatrix[3].x, mModelMatrix[3].y, mModelMatrix[3].z);

        glm::vec3 delta = OBBposition_worldspace - aRayOrigin;

        // Test intersection with the 2 planes perpendicular to the OBB's X axis
        {
            glm::vec3 xaxis(mModelMatrix[0].x, mModelMatrix[0].y, mModelMatrix[0].z);
            float e = glm::dot(xaxis, delta);
            float f = glm::dot(aRayDirection, xaxis);

            if (fabs(f) > 0.001f) { // Standard case

                float t1 = (e + mMin.x) / f; // Intersection with the "left" plane
                float t2 = (e + mMax.x) / f; // Intersection with the "right" plane
                // t1 and t2 now contain distances betwen ray origin and ray-plane intersections

                // We want t1 to represent the nearest intersection, 
                // so if it's not the case, invert t1 and t2
                if (t1 > t2) {
                    float w = t1; t1 = t2; t2 = w; // swap t1 and t2
                }

                // tMax is the nearest "far" intersection (amongst the X,Y and Z planes pairs)
                if (t2 < tMax)
                    tMax = t2;
                // tMin is the farthest "near" intersection (amongst the X,Y and Z planes pairs)
                if (t1 > tMin)
                    tMin = t1;

                // And here's the trick :
                // If "far" is closer than "near", then there is NO intersection.
                // See the images in the tutorials for the visual explanation.
                if (tMax < tMin)
                    return false;

            }
            else { // Rare case : the ray is almost parallel to the planes, so they don't have any "intersection"
                if (-e + mMin.x > 0.0f || -e + mMax.x < 0.0f)
                    return false;
            }
        }


        // Test intersection with the 2 planes perpendicular to the OBB's Y axis
        // Exactly the same thing than above.
        {
            glm::vec3 yaxis(mModelMatrix[1].x, mModelMatrix[1].y, mModelMatrix[1].z);
            float e = glm::dot(yaxis, delta);
            float f = glm::dot(aRayDirection, yaxis);

            if (fabs(f) > 0.001f) {

                float t1 = (e + mMin.y) / f;
                float t2 = (e + mMax.y) / f;

                if (t1 > t2) { float w = t1; t1 = t2; t2 = w; }

                if (t2 < tMax)
                    tMax = t2;
                if (t1 > tMin)
                    tMin = t1;
                if (tMin > tMax)
                    return false;

            }
            else {
                if (-e + mMin.y > 0.0f || -e + mMax.y < 0.0f)
                    return false;
            }
        }


        // Test intersection with the 2 planes perpendicular to the OBB's Z axis
        // Exactly the same thing than above.
        {
            glm::vec3 zaxis(mModelMatrix[2].x, mModelMatrix[2].y, mModelMatrix[2].z);
            float e = glm::dot(zaxis, delta);
            float f = glm::dot(aRayDirection, zaxis);

            if (fabs(f) > 0.001f) {

                float t1 = (e + mMin.z) / f;
                float t2 = (e + mMax.z) / f;

                if (t1 > t2) { float w = t1; t1 = t2; t2 = w; }

                if (t2 < tMax)
                    tMax = t2;
                if (t1 > tMin)
                    tMin = t1;
                if (tMin > tMax)
                    return false;

            }
            else {
                if (-e + mMin.z > 0.0f || -e + mMax.z < 0.0f)
                    return false;
            }
        }

        if (aOutDist)
        {
            *aOutDist = tMin;
        }

        return true;
    }
private:
    bool CheckIntersectionPlane(glm::vec3 aRayOrigin, glm::vec3 aRayDirection, EBoxPlane aPlane, glm::vec3 const & aDelta, float& tMin, float& tMax)
    {
        const float threshold = 0.0000001f;

        glm::vec3 lAxis(mModelMatrix[aPlane].x, mModelMatrix[aPlane].y, mModelMatrix[aPlane].z);
        float e = glm::dot(lAxis, aDelta);
        float f = glm::dot(aRayDirection, lAxis);

        if (glm::abs(f) > threshold)
        {
            // Beware, don't do the division if f is near 0 ! See full source code for details.
            float t1 = (e + mMin[aPlane]) / f; // Intersection with the "left" plane
            float t2 = (e + mMax[aPlane]) / f; // Intersection with the "right" plane

            // if wrong order
            if (t1 > t2)
            {
                float w = t1; t1 = t2; t2 = w; // swap t1 and t2
            }

            // tMax is the nearest "far" intersection (amongst the X,Y and Z planes pairs)
            if (t2 < tMax) tMax = t2;
            // tMin is the farthest "near" intersection (amongst the X,Y and Z planes pairs)
            if (t1 > tMin) tMin = t1;

            if (tMax < tMin)
                return false;
        }
        else 
        {
            if (-e + mMin[aPlane] > 0.0 || -e + mMax[aPlane] < 0.0)
                return false;
        }

        return true;
    }
private:
    glm::vec3 mMin;
    glm::vec3 mMax;
    glm::mat4 mModelMatrix;
};