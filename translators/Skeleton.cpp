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
    MObject parentNode;
 /*   if(ctx->getMObject(skel.GetPrim().GetPath().GetParentPath(), parentNode, MFn::kJoint))  //TODO:This needs to be tested, what if its parent is root of the scene or skelRoot
    {
      //Build out a joint hierarchy
      std::vector<MObject> joints;
      if(SkeletonUtils::createJointHierarchy(skelQuery, parentNode, ctx, &joints))
      {
          //Create Bind pose
      }
    }
*/
  }




 // status = updateMayaAttributes(createObj, prim);

  return status;
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
