
#include "Box.h"

bool SBox::CheckRayIntersection(glm::vec3 aRayOrigin, glm::vec3 aRayDirection, float* aOutDist)
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