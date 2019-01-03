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
#include "pxr/base/gf/rotation.h"

#include "maya/MStatus.h"
#include "maya/MFnIkJoint.h"
#include "maya/MFnNumericAttribute.h"
#include "maya/MMatrix.h"

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

    // Create an extra joint to represent UsdSkelSkeleton prim
    if(createMayaJointNode(jointContainerPath, parent, ctx, jointContainer) == MS::kFailure)
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

  // Set the draw style of this joint so that this extra joint is not drawn in the viewport
  MObject drawStyleObj = fnJoint.attribute(MString("drawStyle"),&status);
  AL_MAYA_CHECK_ERROR2(status, "CreateJointHierarchy: Failed to find joint attribute: drawStyle. ");
  AL_MAYA_CHECK_ERROR(DgNodeTranslator::setInt32(jointContainer, drawStyleObj, 2), errorString);


  createJointChains(skelQuery, jointContainerPath, ctx, joints);
  copyJointStatesFromSkel(skelQuery, joints);
  copyAnimFromSkel(skelQuery, jointContainer, jointContainerPath, joints, jointContainerIsSkeleton, ctx);

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
    std::string jointPathStr = jointPath.GetString();
    UsdPrim jointPrim = skelQuery.GetPrim().GetStage()->GetPrimAtPath(jointPath);

    // But all the joints are not real prims
    if(!jointPath.IsPrimPath())
      continue;

    MObjectHandle parentJoint;
    ctx->getMObject(jointPath.GetParentPath(), parentJoint, MFn::kJoint);
    if (!parentJoint.isValid())
    {
      MGlobal::displayError("Could not find parent node for joint "+ MString(jointPath.GetText()));
      return false;
    }

    if (!createMayaJointNode(jointPath, parentJoint.object(), ctx, joints[i])) {
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
MStatus SkeletonUtils::createMayaJointNode(const SdfPath& path, const MObject& parent, TranslatorContextPtr ctx, MObject& createObj)
{
  MStatus status = MS::kSuccess;

  MFnIkJoint fnJoint;
  createObj = fnJoint.create(parent, &status);
  AL_MAYA_CHECK_ERROR2(status, MString("unable to create joint."));
  if (status == MS::kFailure)
      return status;

  //Rename the node to have Sdf path as its name
  MFnDependencyNode fnDepNode(createObj, &status);
  fnDepNode.setName(MString(path.GetName().c_str(), path.GetName().size()), &status);

  if(ctx)
  {
    ctx->insertItem(path, TfToken("joint"), createObj);
  }

  return status;
}

//----------------------------------------------------------------------------------------------------------------------
bool SkeletonUtils::createBindPose(const UsdSkelSkeletonQuery& skelQuery, const std::vector<MObject>& joints, TranslatorContextPtr ctx, MObject& bindPoseNode)
{
    if (!skelQuery)
    {
      TF_DEBUG(ALUSDMAYA_TRANSLATORS).Msg("'skelQuery' is invalid.");
      return false;
    }

    VtMatrix4dArray localXforms, worldXforms;
    if (!skelQuery.ComputeJointLocalTransforms(&localXforms, UsdTimeCode::Default(), true))
    {
      TF_WARN("%s -- Failed reading rest transforms. No dagPose will be created for the Skeleton.", skelQuery.GetPrim().GetPath().GetText());
      return false;
    }

    if (!skelQuery.GetJointWorldBindTransforms(&worldXforms))
    {
      TF_WARN("%s -- Failed reading bind transforms. No dagPose will be created for the Skeleton.", skelQuery.GetPrim().GetPath().GetText());
      return false;
    }

    SdfPath path = skelQuery.GetPrim().GetPath().AppendChild(TfToken(skelQuery.GetPrim().GetPath().GetName() + "_bindPose"));
    return createDagPose(path, joints, skelQuery.GetTopology().GetParentIndices(), localXforms, worldXforms, ctx, bindPoseNode);
}

//----------------------------------------------------------------------------------------------------------------------
bool SkeletonUtils::createDagPose(const SdfPath& path, const std::vector<MObject>& joints, const VtIntArray& parentIndices, const VtMatrix4dArray& localXforms, const VtMatrix4dArray& worldXforms, TranslatorContextPtr ctx, MObject& dagPoseNode)
{
  MStatus status;
  MDGModifier dgMod;

  dagPoseNode = dgMod.createNode("dagPose", &status);
  AL_MAYA_CHECK_ERROR2(status, MString("unable to create dagPose node."));
  status = dgMod.renameNode(dagPoseNode, MString(path.GetName().c_str()));

  if(ctx)
    ctx->insertItem(path.GetText(), TfToken("dagPose"), dagPoseNode);

  MFnDependencyNode dagPoseDep(dagPoseNode, &status);
  MPlug membersPlug = dagPoseDep.findPlug("members", &status);
  const size_t numJoints = joints.size();
  status = membersPlug.setNumElements(numJoints);
  AL_MAYA_CHECK_ERROR2(status, MString("unable to set number of members on dagPose node."));

  MPlug worldMatrixPlug = dagPoseDep.findPlug("worldMatrix", &status);
  status = worldMatrixPlug.setNumElements(numJoints);
  AL_MAYA_CHECK_ERROR2(status, MString("unable to set number of worldMatrix on dagPose node."));

  MPlug xformMatrixPlug = dagPoseDep.findPlug("xformMatrix", &status);
  status = xformMatrixPlug.setNumElements(numJoints);
  AL_MAYA_CHECK_ERROR2(status, MString("unable to set number of xformMatrix on dagPose node."));

  MPlug parentsPlug = dagPoseDep.findPlug("parents", &status);
  status = parentsPlug.setNumElements(numJoints);
  AL_MAYA_CHECK_ERROR2(status, MString("unable to set number of parents on dagPose node."));

  // Wire up per-member connections.
  MFnDependencyNode jointDep;
  for(size_t i = 0; i < numJoints; ++i)
  {
    status = jointDep.setObject(joints[i]);

    // Connect members[i].message -> dagPose.members[i]
    MPlug memberMessagePlug = jointDep.findPlug("message", &status);
    status = dgMod.connect(memberMessagePlug, membersPlug.elementByLogicalIndex(i));
    AL_MAYA_CHECK_ERROR2(status, MString("unable to connect message attribute to dagPose node."));

    int parentIdx = parentIndices[i];
    MPlug elemParent = parentsPlug.elementByLogicalIndex(i);
    if(parentIdx >= 0 && static_cast<size_t>(parentIdx) < numJoints)
    {
      // Child joint: Connect dagPose.members[parent] -> dagPose.parents[child]
      MPlug parentMemberPlug = membersPlug.elementByLogicalIndex(parentIdx);
      status = dgMod.connect(parentMemberPlug, elemParent);
      AL_MAYA_CHECK_ERROR2(status, MString("unable to connect child members attribute to parents attribute."));
    }
    else
    {
      // Root joint: Connect dagPose.world -> dagPose.parents[i]
      MPlug worldPlug = dagPoseDep.findPlug("world", status);
      status = dgMod.connect(worldPlug, elemParent);
      AL_MAYA_CHECK_ERROR2(status, MString("unable to connect world attribute to parents attribute."));
    }

    const char* const errorString = "Skeleton Translator: error setting rest transformation on dagPose object. ";
    MPlug elemWorldMatrix = worldMatrixPlug.elementByLogicalIndex(i);
    AL_MAYA_CHECK_ERROR(DgNodeTranslator::setMatrix4x4(dagPoseNode, elemWorldMatrix, worldXforms[i].data()), errorString);

    MPlug elemXformMatrix = xformMatrixPlug.elementByLogicalIndex(i);
    AL_MAYA_CHECK_ERROR(DgNodeTranslator::setMatrix4x4(dagPoseNode, elemXformMatrix, localXforms[i].data()), errorString);

  }
  status = dgMod.doIt();
  AL_MAYA_CHECK_ERROR2(status, MString("unable to execute dgModifier operations."));

  const char* const errorString2 = "Skeleton Translator: error setting rest transformation on dagPose object. ";
  MPlug bindPosePlug = dagPoseDep.findPlug("bindPose", status);
  AL_MAYA_CHECK_ERROR(DgNodeTranslator::setBool(dagPoseNode, bindPosePlug, true), errorString2);

  return true;
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

//---------------------------------------------------------------------------------------------------------------------
bool SkeletonUtils::copyJointStatesFromSkel(const UsdSkelSkeletonQuery& skelQuery, std::vector<MObject>& joints)
{
  MStatus status = MS::kSuccess;
  auto numJoints = joints.size();

  // Compute skel-space rest xforms to store as the bindPose of each joint.
  VtMatrix4dArray restXforms;
  if (!skelQuery.ComputeJointSkelTransforms(&restXforms, UsdTimeCode::Default(), true))
    return false;

  if (restXforms.size() != numJoints)
  {
    TF_DEBUG(ALUSDMAYA_TRANSLATORS).Msg("The size of rest xform %d is not aligned with size of joints %d\n", restXforms.size(), numJoints);
    return false;
  }

  MFnDependencyNode jointDep;
  for(auto i = 0; i < numJoints; ++i)
  {
    if(jointDep.setObject(joints[i]) == MS::kSuccess)
    {
      const char* const errorString = "SkeletonTranslator: Error setting attribute value for joint";
      MObject bindPose = jointDep.attribute("bindPose", &status);
      double values[4][4];
      restXforms[i].Get(values);
      AL_MAYA_CHECK_ERROR(DgNodeTranslator::setMatrix4x4(joints[i], bindPose, MMatrix(values)), errorString);


      // Scale does not inherit as expected without disabling segmentScaleCompensate
      MObject segmentScaleCom = jointDep.attribute("segmentScaleCompensate", &status);
      AL_MAYA_CHECK_ERROR(DgNodeTranslator::setBool(joints[i], segmentScaleCom, false), errorString);
     }
  }
  return status == MS::kSuccess ? true : false;
}

//---------------------------------------------------------------------------------------------------------------------
bool SkeletonUtils::copyAnimFromSkel(const UsdSkelSkeletonQuery& skelQuery, const MObject& jointContainer, const SdfPath& containerPath, const std::vector<MObject>& joints, bool containerIsSkeleton, TranslatorContextPtr ctx)
{
  // Retrieve all time samples and convert to maya time
  std::vector<double> usdTimes;
  MTimeArray mayaTimes = GetTimeSamples(skelQuery, usdTimes);

  //Pre-sample the Skeleton's local transforms
  std::vector<GfMatrix4d> skelLocalXforms(usdTimes.size());
  UsdGeomXformable::XformQuery xfQuery(skelQuery.GetSkeleton());
  bool skeletonHasTransform = false;
  for(auto i = 0; i < usdTimes.size(); ++i)
  {
    if(xfQuery.GetLocalTransformation(&skelLocalXforms[i], usdTimes[i]) && skelLocalXforms[i] != GfMatrix4d(1.0))
        skeletonHasTransform = true;
  }

  // If skeleton prim does not have local transformation or local transformation
  // is identity matrix, there is no need to set it on container node
  if(containerIsSkeleton && skeletonHasTransform)
  {
    // The jointContainer is being used to represent the Skeleton.
    // Copy the Skeleton's local transforms onto the container.
    if (!setAnimationOnTransform(jointContainer, skelLocalXforms, mayaTimes, ctx))
    {
      return false;
    }
  }

  // Pre-sample all joint animation
  std::vector<VtMatrix4dArray> jointLocals(usdTimes.size());
  for (auto i = 0; i < jointLocals.size(); ++i)
  {
    if(!skelQuery.ComputeJointLocalTransforms(&jointLocals[i], usdTimes[i]))
         return false;
    if(!containerIsSkeleton && skeletonHasTransform)
    {
      // We do not have a node to receive the local transforms of the
      // Skeleton, so any local transforms on the Skeleton must be
      // concatened onto the root joints instead.
      for(auto j = 0; j < skelQuery.GetTopology().GetNumJoints(); ++j)
      {
        if (skelQuery.GetTopology().GetParent(j) < 0)
          // This is a root joint. Concat by the local skel xform.
          jointLocals[i][j] *= skelLocalXforms[i];

      }
    }
  }

  MFnDependencyNode jointDep;
  std::vector<GfMatrix4d> xforms(jointLocals.size());
  // If skeleton prim does not have local transformation or local transformation
  // is identity matrix, there is no need to set keys on joints node, except
  // the ones that already have time-sampled values on.
  if(!skeletonHasTransform)
  {
    // Find joints that actually have time-sampled and set keys on them
    VtTokenArray jointNames = skelQuery.GetJointOrder();
    if(UsdSkelAnimQuery animQuery = skelQuery.GetAnimQuery())
    {
      UsdSkelAnimation skelAnim(animQuery.GetPrim());
      VtTokenArray sampledJoints;
      skelAnim.GetJointsAttr().Get(&sampledJoints);

      for(auto i = 0; i < sampledJoints.size(); ++i)
      {
        VtTokenArray::iterator it = std::find(jointNames.begin(), jointNames.end(), sampledJoints[i]);
        if(it != jointNames.end())
        {
          const SdfPath jointPath = makeJointPath(containerPath, sampledJoints[i]);
          MObjectHandle jointObj;
          ctx->getMObject(jointPath, jointObj, MFn::kJoint);
          if (!jointObj.isValid())
          {
            MGlobal::displayError("Could not find node for joint "+ MString(jointPath.GetText()));
            return false;
          }

         // Suppose jointLocals column is aligned with jointOrder
         for(auto i = 0; i < jointLocals.size(); ++i)
         {
           xforms[i] = jointLocals[i][it-jointNames.begin()];
         }

         if(!setAnimationOnTransform(joints[it-jointNames.begin()], xforms, mayaTimes, ctx))
           return false;
      }
    }
   }
  }
  else
  {
    // If skeleton prim does have local transformation, set keys on all joints
    for (auto jointIdx = 0; jointIdx < joints.size(); ++jointIdx)
    {
      if (!jointDep.setObject(joints[jointIdx]))
        continue;

      // Get the transforms of just this joint.
      for(size_t i = 0; i < jointLocals.size(); ++i)
      {
        xforms[i] = jointLocals[i][jointIdx];
      }

      if(!setAnimationOnTransform(joints[jointIdx], xforms, mayaTimes, ctx))
        return false;
    }

  }

  return true;
}

//---------------------------------------------------------------------------------------------------------------------
MTimeArray SkeletonUtils::GetTimeSamples(const UsdSkelSkeletonQuery& skelQuery, std::vector<double>& usdTimes)
{
  // By default getting all time samples if there is any
  if(UsdSkelAnimQuery animQuery = skelQuery.GetAnimQuery())
    animQuery.GetJointTransformTimeSamples(&usdTimes);
  if(usdTimes.empty())
    usdTimes.resize(1, UsdTimeCode::EarliestTime().GetValue());

  MTimeArray mayaTimes;
  mayaTimes.setLength(usdTimes.size());
  for (auto i = 0; i < usdTimes.size(); ++i)
  {
    mayaTimes[i] = usdTimes[i];
  }
  return mayaTimes;
}

bool SkeletonUtils::setAnimationOnTransform(const MObject& transformObj, const std::vector<GfMatrix4d>& xforms, MTimeArray mayaTimes, TranslatorContextPtr ctx)
{
  MStatus status = MS::kSuccess;

  if (xforms.size() != mayaTimes.length())
  {
    TF_DEBUG(ALUSDMAYA_TRANSLATORS).Msg("The size of xform %d is not aligned with size of time sample %d\n", xforms.size(), mayaTimes.length());
    return false;
  }

  if(xforms.empty())
    return true;

  const unsigned int numSamples = mayaTimes.length();
  if (numSamples > 1)
  {
    MDoubleArray translates[3] = { MDoubleArray(numSamples), MDoubleArray(numSamples), MDoubleArray(numSamples)};
    MDoubleArray rotates[3] = { MDoubleArray(numSamples), MDoubleArray(numSamples), MDoubleArray(numSamples)};
    MDoubleArray scales[3] = { MDoubleArray(numSamples, 1), MDoubleArray(numSamples, 1), MDoubleArray(numSamples, 1)};

    // Decompose all transforms.
    for(auto i = 0; i < numSamples; ++i)
    {
      const auto& xform = xforms[i];
      GfVec3d t, r, s;
      if(convertUsdMatrixToComponents(xform, t, r, s))
      {
        for(int c = 0 ; c < 3; ++c)
        {
          translates[c][i] = t[c];
          rotates[c][i] = r[c];
          scales[c][i] = s[c];
        }
      }
    }

    if(!setAnimPlugData(transformObj, "translateX", translates[0], mayaTimes, ctx) ||
       !setAnimPlugData(transformObj, "translateY", translates[1], mayaTimes, ctx) ||
       !setAnimPlugData(transformObj, "translateZ", translates[2], mayaTimes, ctx))
      return false;

    if(!setAnimPlugData(transformObj, "rotateX", rotates[0], mayaTimes, ctx) ||
       !setAnimPlugData(transformObj, "rotateY", rotates[1], mayaTimes, ctx) ||
       !setAnimPlugData(transformObj, "rotateZ", rotates[2], mayaTimes, ctx))
      return false;

    if(!setAnimPlugData(transformObj, "scaleX", scales[0], mayaTimes, ctx) ||
       !setAnimPlugData(transformObj, "scaleY", scales[1], mayaTimes, ctx) ||
       !setAnimPlugData(transformObj, "scaleZ", scales[2], mayaTimes, ctx))
      return false;
  }
  else
  {
    const auto& xform = xforms.front();
    GfVec3d t, r, s;
    if(convertUsdMatrixToComponents(xform, t, r, s))
    {
       MFnDependencyNode fnDep(transformObj, &status);
       MObject transPlug = fnDep.findPlug("translate", &status);
       MObject rotatePlug = fnDep.findPlug("rotate", &status);
       MObject scalePlug = fnDep.findPlug("scale", &status);

       const char* const errorString = "Skeleton Translator: error setting transformation animation on transform object. ";
       AL_MAYA_CHECK_ERROR(DgNodeTranslator::setVec3(transformObj,transPlug, t[0], t[1], t[2]), errorString);
       AL_MAYA_CHECK_ERROR(DgNodeTranslator::setVec3(transformObj,rotatePlug, r[0], r[1], r[2]), errorString);
       AL_MAYA_CHECK_ERROR(DgNodeTranslator::setVec3(transformObj,scalePlug, s[0], s[1], s[2]), errorString);

      }
  }
  return true;
}

bool SkeletonUtils::convertUsdMatrixToComponents(const GfMatrix4d& matrix, GfVec3d& translation, GfVec3d& rotation, GfVec3d& scale)
{
  //const TransformRotationOrder rotationOrder, XYZ
  GfVec3d pivotPosition(0,0,0);
  GfVec3d pivotOrientation(0,0,0);
  GfVec3d rot, scaleVec;

  GfMatrix3d pivotOrientMat(GfRotation(GfVec3d::XAxis(), pivotOrientation[0]) *
                             GfRotation(GfVec3d::YAxis(), pivotOrientation[1]) *
                             GfRotation(GfVec3d::ZAxis(), pivotOrientation[2]));

  GfMatrix4d pp = GfMatrix4d(1.0).SetTranslate(pivotPosition);
  GfMatrix4d ppInv = GfMatrix4d(1.0).SetTranslate(-pivotPosition);
  GfMatrix4d po    = GfMatrix4d(1.0).SetRotate(pivotOrientMat);
  GfMatrix4d poInv = GfMatrix4d(1.0).SetRotate(pivotOrientMat.GetInverse());

  GfMatrix4d factorMe = po * pp * matrix * ppInv;
  GfMatrix4d scaleOrientMat, factoredRotMat, perspMat;
  factorMe.Factor(&scaleOrientMat, &scaleVec, &factoredRotMat, &translation, &perspMat);

  GfMatrix4d rotMat = factoredRotMat * poInv;

  if(!rotMat.Orthonormalize(false))
    TF_DEBUG(ALUSDMAYA_TRANSLATORS).Msg("Failed to orthonormalize rotMat.");

  GfVec3d angles = rotMat.ExtractRotation().Decompose(GfVec3d::ZAxis(),GfVec3d::YAxis(),GfVec3d::XAxis());
  rot[0] = angles[2];
  rot[1] = angles[1];
  rot[2] = angles[0];

  for(int i=0; i<3; ++i)
  {
    rotation[i] = GfDegreesToRadians(rot[i]);
    scale[i] = scaleVec[i];
  }

  return true;
}


bool SkeletonUtils::setAnimPlugData(const MObject& obj, const MString& attr, MDoubleArray& values, MTimeArray& times, TranslatorContextPtr ctx)
{
    MStatus status;
    MFnDependencyNode depNode(obj, &status);
    MPlug plug = depNode.findPlug(attr, &status);

    if(!plug.isKeyable())
      status = plug.setKeyable(true);

    //Test
    for(int i = 0; i < values.length();i++)
    {
        std::cout<<"the decomposed values are "<<values[i]<<","<<std::endl;
    }
    MFnAnimCurve animFn;
    MObject animObj = animFn.create(plug, nullptr, &status);
    status = animFn.addKeys(&times, &values);

    //TODO: There is no way to register newly created anim curve object, as there is no corresponding prim in usd file


    return true;
}

} // translators
} // fileio
} // usdmaya
} // AL
//-----------------------------------------------------------------------------
