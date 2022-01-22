/*
 * @file    TTransform2D.h
 * @author  David Gallardo Moreno
 */

#ifndef __SBX_TRANSFORM_2D_H__
#define __SBX_TRANSFORM_2D_H__

namespace sbx { namespace gfx
{
    
    struct TTransform2D
    {
        enum EPropType
        {
            eFloat,
            eVec2,
            eAngle,
        };

        float x, y;
        float sx, sy;
        float Rotation;
                   
        TTransform2D() { reset(); }
        TTransform2D(float aX, float aY,
                     float aSX, float aSY,
                     float aRotation)
            : x(aX)
            , y(aY)
            , sx(aSX)
            , sy(aSY)
            , Rotation(aRotation)
        {};

        void reset()
        {
            x = y = Rotation = 0.0f;
            sx = sy = 1.0f;
        }
    };

#   define SBX_TRANSFORM2D_PROPERTIES() \
    SBX_T2D_PROP(Position,  TTransform2D::eVec2,    &TTransform2D::x) \
    SBX_T2D_PROP(Scale,     TTransform2D::eVec2,    &TTransform2D::sx) \
    SBX_T2D_PROP(Rotation,  TTransform2D::eAngle,   &TTransform2D::Rotation)
}} // sbx::gfx

#endif //__SBX_TRANSFORM_2D_H__