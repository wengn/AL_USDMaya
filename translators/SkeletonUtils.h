//
// Copyright 2017 Animal Logic
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.//
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

#pragma once 


#include "pxr/usd/usdSkel/skeleton.h"
#include "pxr/usd/usdSkel/skeletonQuery.h"
#include "pxr/usd/usdSkel/animation.h"

#include "AL/usdmaya/fileio/translators/TranslatorContext.h"

#include "maya/MObjectHandle.h"

namespace AL {
namespace usdmaya {
namespace fileio {
namespace translators {


//----------------------------------------------------------------------------------------------------------------------
/// \brief Helper Class for Skeleton Translator
//----------------------------------------------------------------------------------------------------------------------

class SkeletonUtils
{
public:
  static bool createJointHierarchy(const UsdSkelSkeletonQuery& skelQuery, const MObject& parent, TranslatorContextPtr ctx, std::vector<MObject>& joints);

  static bool isSkelMayaGenerated(const UsdSkelSkeleton& skel);
  static MStatus createMayaJointNode(const SdfPath& path, const MObject& parent, TranslatorContextPtr ctx, MObject& createObj);
  static bool createBindPose(const UsdSkelSkeletonQuery& skelQuery, const std::vector<MObject>& joints, TranslatorContextPtr ctx, MObject& bindPoseNode);

private:
  static bool createJointChains(const UsdSkelSkeletonQuery& skelQuery, const SdfPath& containerPath, TranslatorContextPtr ctx, std::vector<MObject>& joints);
  static SdfPath makeJointPath(const SdfPath& containerPath, const TfToken& joint);
  static bool copyJointStatesFromSkel(const UsdSkelSkeletonQuery& skelQuery, std::vector<MObject>& joints);
  static bool copyAnimFromSkel(const UsdSkelSkeletonQuery& skelQuery, const MObject& jointContainer, const SdfPath& containerPath, const std::vector<MObject>& joints, bool containerIsSkeleton, TranslatorContextPtr ctx);
  static MTimeArray GetTimeSamples(const UsdSkelSkeletonQuery& skelQuery, std::vector<double>& usdTimes);
  static bool setAnimationOnTransform(const MObject& transformObj, const std::vector<GfMatrix4d>& xforms, MTimeArray mayaTimes, TranslatorContextPtr ctx);
  static bool convertUsdMatrixToComponents(const GfMatrix4d& matrix, GfVec3d& translation, GfVec3d& rotation, GfVec3d& scale);
  static bool setAnimPlugData(const MObject& obj, const MString& attr, MDoubleArray& values, MTimeArray& times, TranslatorContextPtr ctx);
  static bool createDagPose(const SdfPath& path, const std::vector<MObject>& joints, const VtIntArray& parentIndices, const VtMatrix4dArray& localXforms, const VtMatrix4dArray& worldXforms, TranslatorContextPtr ctx, MObject& dagPoseNode);
};

} // translators
} // fileio
} // usdmaya
} // AL
//-----------------------------------------------------------------------------
