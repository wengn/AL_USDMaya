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


#include "Skeleton.h"
#include "SkeletonUtils.h"

#include "pxr/usd/usdSkel/skeleton.h"
#include "pxr/usd/usdSkel/skeletonQuery.h"


#include "AL/usdmaya/utils/DgNodeHelper.h"
#include "AL/usdmaya/fileio/AnimationTranslator.h"
#include "AL/usdmaya/fileio/translators/DgNodeTranslator.h"

#include "maya/MFnIkJoint.h"

namespace AL {
namespace usdmaya {
namespace fileio {
namespace translators {

AL_USDMAYA_DEFINE_TRANSLATOR(Skeleton, PXR_NS::UsdSkelSkeleton)

//----------------------------------------------------------------------------------------------------------------------
MStatus Skeleton::initialize()
{
    MStatus status = MS::kSuccess;
    return status;
}

//----------------------------------------------------------------------------------------------------------------------
MStatus Skeleton::import(const UsdPrim& prim, MObject& parent, MObject& createObj)
{
  MStatus status = MS::kSuccess;

  UsdSkelSkeleton skel(prim);
  if(!prim.IsValid())
  {
    TF_DEBUG(ALUSDMAYA_TRANSLATORS).Msg("SkeletonTranslator::import prim invalid\n");
    return MS::kFailure;
  }

  TranslatorContextPtr ctx = context();
  if(UsdSkelSkeletonQuery skelQuery = m_cache.GetSkelQuery(skel))
  {
    MObjectHandle parentNode;
    ctx->getMObject(prim.GetPath().GetParentPath(), parentNode, MFn::kInvalid); //It needs to find a valid parent, which is UsdSkelRoot
    if(!parentNode.isValid())
    {
      // Theoretically there should be always a SkelRoot node above a SkelSkeleton node
      // This is solved by adding a schema translator for UsdSkelRoot
      MGlobal::displayError("Could not find a valid root node for skeleton node" + MString(skel.GetPrim().GetPath().GetName().c_str()));
    }

    std::vector<MObject> joints;
    if(SkeletonUtils::createJointHierarchy(skelQuery, parentNode.object(), ctx, joints))
    {
      // Add a dagPose node to hold the rest pose.
      // This is not necessary for skinning in Maya, but is necessary in order to properly
      // round-trip the Skeleton's restTransforms, and is a requirement of some exporters.
      // The dagPose command also will not work without this.
      MObject bindPose;
      if(SkeletonUtils::createBindPose(skelQuery, joints, ctx, bindPose))
        return status;
    }
  }

  return MS::kFailure;
}

//----------------------------------------------------------------------------------------------------------------------
MStatus Skeleton::update(const UsdPrim& prim)
{
  MStatus status = MS::kSuccess;
  return status;
}

//----------------------------------------------------------------------------------------------------------------------
MStatus Skeleton::tearDown(const SdfPath& path)
{
  MStatus status = MS::kSuccess;
  return status;
}


//----------------------------------------------------------------------------------------------------------------------
} // translators
} // fileio
} // usdmaya
} // AL
//----------------------------------------------------------------------------------------------------------------------
