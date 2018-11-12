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

#include "SkeletonUtils.h"

#include "AL/usdmaya/utils/DgNodeHelper.h"
#include "AL/usdmaya/fileio/translators/DgNodeTranslator.h"

#include "maya/MStatus.h"
#include "maya/MFnIkJoint.h"
#include "maya/MFnNumericAttribute.h"

namespace AL {
namespace usdmaya {
namespace fileio {
namespace translators {

//----------------------------------------------------------------------------------------------------------------------
bool SkeletonUtils::createJointHierarchy(const UsdSkelSkeletonQuery& skelQuery, MObject& parent, TranslatorContextPtr ctx, std::vector<MObjectHandle>& joints)
{
  MStatus status = MS::kSuccess;
  if(!skelQuery)
  {
      TF_DEBUG(ALUSDMAYA_TRANSLATORS).Msg("'skelQuery' is invalid\n");
      return false;
  }

  SdfPath jointContainerPath;
  bool jointContainerIsSkeleton = false;
  MObject jointContainer;

  if(isSkelMayaGenerated(skelQuery.GetSkeleton()))
  {
    // If a joint hierarchy was originally exported from Maya, then do not add a node to represent the Skeleton in Maya.
    // We prefer to maintain the original joint hierarchy: instead of creating a joint for the Skeleton, we will put joints
    // beneath the parent of the Skeleton.
    jointContainerPath = skelQuery.GetPrim().GetPath().GetParentPath();
    jointContainer = parent;
  }
  else
  {
    jointContainerPath = skelQuery.GetPrim().GetPath();
    jointContainerIsSkeleton = true;

    if(createMayaJointNode(skelQuery.GetPrim(), parent, ctx, jointContainer) == MS::kFailure)  //TODO: It looks like DGNodeTranslator is half implemented,probably don't use its createNode() for now.
      return false;
  }

  // Create an attribute to indicate that this joint represents UsdSkelSkeleton's transform
  MObject attrObj = MFnNumericAttribute().create("isUsdSkeleton", "ius", MFnNumericData::kBoolean, true, &status);
  AL_MAYA_CHECK_ERROR2(status, "CreateJointHierarchy: Failed to create dynamic attribute: isUsdSkeleton. ");

  MFnDependencyNode fnJoint(jointContainer, &status);
  AL_MAYA_CHECK_ERROR2(status, "CreateJointHierarchy: Failed to apply function set on created joint.");

  status = fnJoint.addAttribute(attrObj);
  AL_MAYA_CHECK_ERROR2(status, "CreateJointHierarchy: Failed to add dynamic attribute: isUsdSkeleton. ");

  const char* const errorString = "CameraTranslator: error setting value on attribute of maya joint.";
  AL_MAYA_CHECK_ERROR(DgNodeTranslator::setBool(jointContainer, attrObj, true), errorString);

  MObject drawStyleObj = fnJoint.attribute(MString("drawSyle"),&status);
  AL_MAYA_CHECK_ERROR2(status, "CreateJointHierarchy: Failed to find joint attribute: drawStyle. ");
  AL_MAYA_CHECK_ERROR(DgNodeTranslator::setInt32(jointContainer, drawStyleObj, 2), errorString); //TODO: This needs to be tested too.

  //NOT-finished
  return true;

}

//----------------------------------------------------------------------------------------------------------------------
MStatus SkeletonUtils::createMayaJointNode(const UsdPrim& prim, MObject& parent, TranslatorContextPtr ctx, MObject& createObj)
{
  MStatus status = MS::kSuccess;

  MFnIkJoint fnJoint;
  createObj = fnJoint.create(parent, &status);
  AL_MAYA_CHECK_ERROR2(status, MString("unable to create joint."));

  //Rename the node to have Sdf path as its name
  MFnDependencyNode fnDepNode(createObj, &status);
  fnDepNode.setName(MString(prim.GetPath().GetName().c_str(),prim.GetPath().GetName().size()), &status);

  if(ctx)
  {
    ctx->insertItem(prim, createObj);
  }

  return status;
}

//----------------------------------------------------------------------------------------------------------------------
bool SkeletonUtils::isSkelMayaGenerated(const UsdSkelSkeleton& skel)
{
  VtValue mayaData = skel.GetPrim().GetCustomDataByKey(TfToken("Maya"));
  if(mayaData.IsHolding<VtDictionary>())
  {
    const VtDictionary& mayaDict = mayaData.UncheckedGet<VtDictionary>();
    const VtValue* val = mayaDict.GetValueAtPath(TfToken("generated"));
    if(val && val->IsHolding<bool>())
        return val->UncheckedGet<bool>();
  }
  return false;
}



} // translators
} // fileio
} // usdmaya
} // AL
//-----------------------------------------------------------------------------
