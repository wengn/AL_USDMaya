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

#include "pxr/base/tf/token.h"

#include "maya/MStatus.h"
#include "maya/MFnIkJoint.h"
#include "maya/MFnNumericAttribute.h"

namespace AL {
namespace usdmaya {
namespace fileio {
namespace translators {

//----------------------------------------------------------------------------------------------------------------------
bool SkeletonUtils::createJointHierarchy(const UsdSkelSkeletonQuery& skelQuery, const MObject& parent, TranslatorContextPtr ctx, std::vector<MObject>& joints)
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

  const char* const errorString = "CreateJointHierarchy: Error setting value on attribute of maya joint.";
  AL_MAYA_CHECK_ERROR(DgNodeTranslator::setBool(jointContainer, attrObj, jointContainerIsSkeleton), errorString);

  // Set the draw style of this joint so that this extra joint is not drawn
  MObject drawStyleObj = fnJoint.attribute(MString("drawStyle"),&status);
  AL_MAYA_CHECK_ERROR2(status, "CreateJointHierarchy: Failed to find joint attribute: drawStyle. ");
  AL_MAYA_CHECK_ERROR(DgNodeTranslator::setInt32(jointContainer, drawStyleObj, 2), errorString);


  createJointChains(skelQuery, jointContainerPath, ctx, joints);

  //NOT-finished
  return true;

}

bool SkeletonUtils::createJointChains(const UsdSkelSkeletonQuery& skelQuery, const SdfPath& containerPath, TranslatorContextPtr ctx, std::vector<MObject>& joints)
{
  VtTokenArray jointNames = skelQuery.GetJointOrder();

  auto numJoints = jointNames.size();
  joints.resize(numJoints);

  for (size_t i = 0; i < numJoints; ++i)
  {
    const SdfPath jointPath = makeJointPath(containerPath, jointNames[i]);

    //But all the joints are not really prims? This should always return false?
    if(!jointPath.IsPrimPath())
      continue;

    MObjectHandle parentJoint;
    ctx->getMObject(jointPath.GetParentPath(), parentJoint, MFn::kJoint);
    if (!parentJoint.isValid())
    {
      MGlobal::displayError("Could not find parent node for joint "+ MString(jointPath.GetText()));
      return false;
    }

    if (!createMayaJointNode(skelQuery.GetPrim().GetStage()->GetPrimAtPath(jointPath), parentJoint.object(), ctx, joints[i])) {
      return false;
  }
}
  return true;
}

SdfPath SkeletonUtils::makeJointPath(const SdfPath& parentPath, const TfToken& joint)
{
  SdfPath jointPath(joint);
  if(jointPath.IsAbsolutePath())
  {
    jointPath = jointPath.MakeRelativePath(SdfPath::AbsoluteRootPath());
  }

  if(!jointPath.IsEmpty())
  {
    return parentPath.AppendPath(jointPath);
  }
  return SdfPath();
}


//----------------------------------------------------------------------------------------------------------------------
MStatus SkeletonUtils::createMayaJointNode(const UsdPrim& prim, const MObject& parent, TranslatorContextPtr ctx, MObject& createObj)
{
  MStatus status = MS::kSuccess;

  MFnIkJoint fnJoint;
  createObj = fnJoint.create(parent, &status);
  AL_MAYA_CHECK_ERROR2(status, MString("unable to create joint."));
  if (status == MS::kFailure)
      return status;

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
