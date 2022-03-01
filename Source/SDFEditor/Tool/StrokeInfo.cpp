// Copyright (c) 2022 David Gallardo and SDFEditor Project

#include "StrokeInfo.h"

void TStrokeInfo::UpdateRotation()
{
    glm::quat q = glm::conjugate(glm::quat(glm::radians(mEulerAngles)));
    //glm::quat q = glm::conjugate(glm::toQuat(glm::orientate3(glm::radians(mEulerAngles))));
    quat = glm::vec4(q.x, q.y, q.z, q.w);
}
