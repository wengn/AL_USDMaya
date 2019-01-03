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


#include "SkelRoot.h"

#include "pxr/usd/usdSkel/skeleton.h"
#include "pxr/usd/usdSkel/root.h"
#include "pxr/usd/usdSkel/skeletonQuery.h"


#include "AL/usdmaya/utils/DgNodeHelper.h"
#include "AL/usdmaya/fileio/AnimationTranslator.h"
#include "AL/usdmaya/fileio/translators/DgNodeTranslator.h"

#include "maya/MFnIkJoint.h"

namespace AL {
namespace usdmaya {
namespace fileio {
namespace translators {

AL_USDMAYA_DEFINE_TRANSLATOR(SkelRoot, PXR_NS::UsdSkelRoot)

//----------------------------------------------------------------------------------------------------------------------
MStatus SkelRoot::initialize()
{
    MStatus status = MS::kSuccess;
    return status;
}


//----------------------------------------------------------------------------------------------------------------------
MStatus SkelRoot::import(const UsdPrim& prim, MObject& parent, MObject& createObj)
{
  MStatus status = MS::kSuccess;

  UsdSkelRoot root(prim);
  if(!prim.IsValid())
  {
    TF_DEBUG(ALUSDMAYA_TRANSLATORS).Msg("SkeletonRootTranslator::import prim invalid\n");
    return MS::kFailure;
  }

  MFnTransform fnTransform;
  createObj = fnTransform.create(parent, &status);
  fnTransform.setName(MString(prim.GetName().GetText()), &status);

  TranslatorContextPtr ctx = context();
  if(ctx)
  {
    ctx->insertItem(prim, createObj);
  }


  return status;
}

//----------------------------------------------------------------------------------------------------------------------
MStatus SkelRoot::update(const UsdPrim& prim)
{
  MStatus status = MS::kSuccess;
  return status;
}

//----------------------------------------------------------------------------------------------------------------------
MStatus SkelRoot::tearDown(const SdfPath& path)
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
