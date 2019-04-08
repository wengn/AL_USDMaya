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

#include "Instancer.h"
#include "pxr/usd/usd/modelAPI.h"
#include "pxr/usd/usdGeom/pointInstancer.h"
#include "pxr/usd/usdUtils/sparseValueWriter.h"
#include "pxr/base/vt/types.h"
#include "pxr/base/gf/rotation.h"

#include "AL/maya/utils/Utils.h"
#include "AL/usdmaya/utils/DgNodeHelper.h"
#include "AL/usdmaya/fileio/AnimationTranslator.h"
#include "AL/usdmaya/fileio/translators/DgNodeTranslator.h"
#include "AL/usdmaya/utils/AttributeType.h"

#include "maya/MDagModifier.h"
#include "maya/MDataHandle.h"
#include "maya/MFnArrayAttrsData.h"
#include "maya/MFnInstancer.h"
#include "maya/MFnNumericAttribute.h"
#include "maya/MFnMesh.h"
#include "maya/MFnParticleSystem.h"
#include "maya/MFnTransform.h"
#include "maya/MNodeClass.h"
#include "maya/MPlugArray.h"
#include "maya/MPointArray.h"

namespace AL {
namespace usdmaya {
namespace fileio {
namespace translators {

AL_USDMAYA_DEFINE_TRANSLATOR(Instancer, PXR_NS::UsdGeomPointInstancer)

//----------------------------------------------------------------------------------------------------------------------
MObject Instancer::m_inputPoints;
MObject Instancer::m_inputHierarchy;
MObject Instancer::m_instanceData;
MObject Instancer::m_matrix;
MObject Instancer::m_visibility;

//----------------------------------------------------------------------------------------------------------------------
template <typename MayaArrayType, typename V,  typename M>
void _MapVtToMayaArrayValues(
    MayaArrayType& mArray,
    const VtArray<V>& vtArray,
    const std::function<M (const V)> mapper)
{
  const size_t size = vtArray.size();
  mArray.clear();
  mArray.setLength(size);
  if(size)
  {
    auto input = vtArray.cdata();
    for (size_t i = 0; i < size; ++i)
    {
       mArray[i] = mapper(input[i]);
    }
  }
}

template<typename MayaArrayType, typename V, typename M>
void _MapMayaToVtArrayValues(
    const MayaArrayType& mArray,
    VtArray<V>& vtArray,
    const std::function<V (const M)> mapper)
{
  unsigned int size = mArray.length();
  vtArray.clear();
  vtArray.resize(size);

  if(size)
  {
    for (unsigned int i = 0; i < size; ++i)
    {
      vtArray[i] = mapper(mArray[i]);
    }
  }
}


//----------------------------------------------------------------------------------------------------------------------
MStatus Instancer::initialize()
{
  MStatus status = MS::kSuccess;
  const char * const errorString = "InstancerTranslator: error retrieving maya instancer or particle attributes";

  MNodeClass instancerNode("instancer");
  m_inputPoints = instancerNode.attribute("inputPoints", &status);
  m_inputHierarchy = instancerNode.attribute("inputHierarchy", &status);
  MNodeClass particleNode("particle");
  m_instanceData = particleNode.attribute("instanceData", &status);
  AL_MAYA_CHECK_ERROR(status, errorString);

  const char * const errorString2 = "InstancerTranslator: error retrieving maya transform attributes";
  MNodeClass transformNode("transform");
  m_matrix = transformNode.attribute("matrix", &status);
  m_visibility = transformNode.attribute("visibility", &status);
  AL_MAYA_CHECK_ERROR(status, errorString2);

  return status;
}

//----------------------------------------------------------------------------------------------------------------------
MStatus Instancer::import(const UsdPrim& prim, MObject& parent, MObject& createObj)
{
  MStatus status = MS::kSuccess;

  if(!UsdGeomPointInstancer(prim))
    return MS::kFailure;

  MFnDagNode fnDag;
  createObj = fnDag.create("instancer", parent, &status);
  AL_MAYA_CHECK_ERROR2(status, "unable to create maya instancer node ");

  MFnInstancer instFn(createObj, &status);
  instFn.setName(AL::maya::utils::convert(prim.GetName().GetString()), &status);

  TranslatorContextPtr ctx = context();
  if(ctx)
  {
    ctx->insertItem(prim, createObj);
  }
  status = updateMayaAttributes(createObj, prim);

  return status;
}



//----------------------------------------------------------------------------------------------------------------------
void Instancer::setMayaInstancerArrayAttr(MFnArrayAttrsData& inputPointsData, const UsdAttribute& usdAttr, MString attrName)
{
  MStatus status;

  auto attrType = AL::usdmaya::utils::getAttributeType(usdAttr);
  switch (attrType)
  {
  case AL::usdmaya::utils::UsdDataType::kVec3f:
  {
    MVectorArray mayaVecArray = inputPointsData.vectorArray(attrName, &status);
    VtArray<GfVec3f> usdVecArray;
    usdAttr.Get(&usdVecArray);
    _MapVtToMayaArrayValues<MVectorArray, GfVec3f, MVector>(
          mayaVecArray, usdVecArray, [](GfVec3f v){ return MVector(v[0], v[1], v[2]); }
    );
    break;
  }
  case AL::usdmaya::utils::UsdDataType::kInt:
  {
    MIntArray mayaIntArray = inputPointsData.intArray(attrName, &status);
    VtArray<int> usdIntArray;
    usdAttr.Get(&usdIntArray);
    _MapVtToMayaArrayValues<MIntArray, int, int>(
          mayaIntArray, usdIntArray, [](int x ){ return x; }
     );
    break;
  }
  case AL::usdmaya::utils::UsdDataType::kInt64:
  case AL::usdmaya::utils::UsdDataType::kDouble:
  {
    MDoubleArray mayaDoubleArray = inputPointsData.doubleArray(attrName, &status);
    VtArray<int64_t> usdIntArray;
    usdAttr.Get(&usdIntArray);
    _MapVtToMayaArrayValues<MDoubleArray, int64_t, double>(
          mayaDoubleArray, usdIntArray, [](int64_t x ){ return double(x); }
     );
    break;
  }
  case AL::usdmaya::utils::UsdDataType::kQuath:
  {
    MVectorArray mayaVecArray = inputPointsData.vectorArray(attrName, &status);
    VtArray<GfQuath> usdQuatArray;
    usdAttr.Get(&usdQuatArray);
    _MapVtToMayaArrayValues<MVectorArray, GfQuath, MVector>(
          mayaVecArray, usdQuatArray, [](GfQuath quat)
    {
      GfRotation rotation(quat);
      GfVec3d result = rotation.Decompose(
            GfVec3d::XAxis(), GfVec3d::YAxis(), GfVec3d::ZAxis());
      return MVector(result[0], result[1], result[2]);
    });
    break;
  }
  }

}


//----------------------------------------------------------------------------------------------------------------------
MStatus Instancer::updateMayaAttributes(MObject mayaObj, const UsdPrim& prim)
{
  //Create a maya particle node into the Maya scene to record all data and connect it to this instance node
  if(!setupParticleNode(mayaObj, prim))
    return MS::kFailure;

 return MS::kSuccess;
}

//----------------------------------------------------------------------------------------------------------------------
bool Instancer::setupParticleNode(MObject mayaObj, const UsdPrim& prim )
{
  MStatus status = MS::kFailure;

  MDagModifier modifier;
  MObject transformObj = modifier.createNode("transform", MObject::kNullObj, &status);
  MObject particleObj = modifier.createNode("particle", transformObj, &status);
  status = modifier.doIt();
  CHECK_MSTATUS_AND_RETURN(status, false);

  MPlug instanceDataArrayPlug(particleObj, m_instanceData);
  MPlug instanceDataPlug = instanceDataArrayPlug.elementByLogicalIndex(0, &status);
  CHECK_MSTATUS_AND_RETURN(status, false);

  if(instanceDataPlug.numChildren() != 2)
  {
    MGlobal::displayError("InstanceData element plug does not have 2 child plugs\n");
    return false;
  }
  MPlug instPointDataPlug = instanceDataPlug.child(1,&status);
  CHECK_MSTATUS_AND_RETURN(status, false);
  if(!createParticleData(prim, instPointDataPlug))
    return false;

  MPlug destPlug(mayaObj, m_inputPoints);
  status = modifier.connect(instPointDataPlug, destPlug);
  status = modifier.doIt();
  CHECK_MSTATUS_AND_RETURN(status, false);

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool Instancer::createParticleData(const UsdPrim& prim, MPlug& pointsDataPlug)
{
  MStatus status = MS::kFailure;

  UsdGeomPointInstancer instancer(prim);
  if(!instancer)
    return false;
  SdfPath instancerPath = prim.GetPrimPath();

  MFnArrayAttrsData inputPointsData;
  MObject inputPointsObj = inputPointsData.create(&status);
  CHECK_MSTATUS_AND_RETURN(status, false);

  // Ids are optional on UsdGeomPointInstancer
  UsdAttribute protoIdsAttr = instancer.GetIdsAttr();
  if (!protoIdsAttr.HasValue()) {
    TF_WARN("Point instancer %s does not have a 'Ids'"
            "attribute. Not adding it to the render index.",
            instancerPath.GetText());
  }
  else
    setMayaInstancerArrayAttr(inputPointsData, protoIdsAttr, MString("id"));

  UsdAttribute protoIndicesAttr = instancer.GetProtoIndicesAttr();
  if (!protoIndicesAttr.HasValue()) {
    TF_WARN("Point instancer %s does not have a 'protoIndices'"
            "attribute. Not adding it to the render index.",
            instancerPath.GetText());
    return false;
  }
 setMayaInstancerArrayAttr(inputPointsData, protoIndicesAttr, MString("objectIndex"));

  UsdAttribute positionsAttr = instancer.GetPositionsAttr();
  if (!positionsAttr.HasValue()) {
    TF_WARN("Point instancer %s does not have a 'positions' attribute. "
            "Not adding it to the render index.", instancerPath.GetText());
    return false;
  }
  setMayaInstancerArrayAttr(inputPointsData, positionsAttr, MString("position"));

  UsdAttribute orientationAttr = instancer.GetOrientationsAttr();
  if (!orientationAttr.HasValue()) {
    TF_WARN("Point instancer %s does not have a 'orientations' attribute. "
            "Not adding it to the render index.", instancerPath.GetText());
    return false;
  }
  setMayaInstancerArrayAttr(inputPointsData, orientationAttr, MString("rotation"));

  UsdAttribute scaleAttr = instancer.GetScalesAttr();
  if (!scaleAttr.HasValue()) {
    TF_WARN("Point instancer %s does not have a 'scales' attribute. "
            "Not adding it to the render index.", instancerPath.GetText());
    CHECK_MSTATUS_AND_RETURN(status, false);
  }
 setMayaInstancerArrayAttr(inputPointsData, scaleAttr, MString("scale"));

 status = pointsDataPlug.setValue(inputPointsObj);
 CHECK_MSTATUS_AND_RETURN(status, false);

 return true;
}

//----------------------------------------------------------------------------------------------------------------------
MStatus Instancer::postImport(const UsdPrim& prim)
{
  MStatus status = MS::kFailure;
  //Set up connection between "prototype" meshes and Maya instancer node
  UsdGeomPointInstancer pointInstancer(prim);
  if(!pointInstancer)
  {
    return status;
  }

  TranslatorContextPtr ctx = context();
  if(ctx)
  {
    MObjectHandle instObj;
    ctx->getMObject(prim.GetPath(),instObj, MFn::kInstancer);

    if(!instObj.isValid())
      return status;

    SdfPathVector protoPaths;
    if(pointInstancer.GetPrototypesRel().GetTargets(&protoPaths))
    {
      MFnInstancer instFn(instObj.object());
      MPlug inputHierPlug(instObj.object(), instFn.attribute("inputHierarchy", &status));

      if(inputHierPlug.numElements())
      {
        MGlobal::displayError("The instancer is already connected with proto meshes");
        return MS::kFailure;
      }else
        inputHierPlug.setNumElements(protoPaths.size());

      MDagModifier mod;
      for(auto it = protoPaths.begin(); it != protoPaths.end(); ++it)
      {
        //Find corresponding dag path MObject and set up the connection
        if(ctx)
        {
          MObjectHandle meshObj;
          ctx->getMObject(*it, meshObj, MFn::kMesh);

          if(meshObj.isValid())
          {
            MFnMesh meshFn(meshObj.object());
            if(meshFn.parentCount() != 1)
            {
              MGlobal::displayError("Mesh does not have one parent transform, cannot connect to instancer");
              continue;
            }

            MObject parentTransform = meshFn.parent(0);
            MPlug matrixPlug(parentTransform, MFnTransform(parentTransform).attribute("matrix", &status));
            mod.connect(matrixPlug, inputHierPlug.elementByLogicalIndex(it - protoPaths.begin()));           

            // Hide the mesh transform
            MPlug visibilityPlug(parentTransform, MFnTransform(parentTransform).attribute("visibility", &status));
            if(visibilityPlug.asBool())
              visibilityPlug.setBool(false);

          }
          else
            return MS::kFailure;
        } 
      }
      return mod.doIt();
    }
  }
  return status;
}

//----------------------------------------------------------------------------------------------------------------------
UsdPrim Instancer::exportObject(UsdStageRefPtr stage, MDagPath dagPath, const SdfPath& usdPath,
                     const ExporterParams& params)
{
  MStatus status = MS::kSuccess;
  UsdGeomPointInstancer instancer = UsdGeomPointInstancer::Define(stage, usdPath);
  UsdPrim prim = instancer.GetPrim();

  MFnInstancer instFn(dagPath, &status);
  AL_MAYA_CHECK_ERROR2(status, "Export: Failed to attach function set to instancer.");

  const MObject instancerObj = instFn.object(&status);
  AL_MAYA_CHECK_ERROR2(status, "Export: Failed to retrieve object.");

  if(!updateUsdPrim(stage, usdPath, instancerObj))
    return UsdPrim();

  return prim;
}

//----------------------------------------------------------------------------------------------------------------------
bool Instancer::updateUsdPrim(UsdStageRefPtr stage, const SdfPath& usdPath, const MObject& obj)
{
  MStatus status;

  UsdGeomPointInstancer instancer = UsdGeomPointInstancer::Define(stage, usdPath);

  //Setting up prototype prim
  MPlug inputHierarchy(obj, m_inputHierarchy);

  // Note that the "Prototypes" prim needs to be a model group to ensure
  // contiguous model hierarchy.
  const SdfPath prototypesGroupPath = instancer.GetPrim().GetPath().AppendChild(TfToken("Prototypes"));
  const UsdPrim prototypesGroupPrim = stage->DefinePrim(prototypesGroupPath);
  UsdModelAPI(prototypesGroupPrim).SetKind(TfToken("group"));
  UsdRelationship prototypesRel = instancer.CreatePrototypesRel();

  const unsigned int numElements = inputHierarchy.numElements();
  if (numElements == 0)
    return false;

  for (unsigned int i = 0; i < numElements; ++i)
  {
    const MPlug plug = inputHierarchy[i];
    MPlugArray source;
    plug.connectedTo(source, true, false, &status);
    if (source.length() < 1)
    {
      TF_WARN("Cannot read prototype: the source plug %s was null", plug.name().asChar());
      return false;
    }

    MFnDagNode sourceNode(source[0].node(), &status);
    CHECK_MSTATUS_AND_RETURN(status, false);
    MDagPath prototypeDagPath;
    status = sourceNode.getPath(prototypeDagPath);
    const SdfPath prototypeUsdPath = prototypesGroupPrim.GetPath().AppendChild(TfToken(sourceNode.name().asChar()));
    prototypesRel.AddTarget(prototypeUsdPath);
  }

  MPlug inputPointsDest(obj, m_inputPoints);
  MPlugArray inputPointsSrc;
  if (!inputPointsDest.connectedTo(inputPointsSrc, true, false, &status) || inputPointsSrc.length() < 1)
  {
    TF_WARN("inputPoints not connected on instancer '%s'", MFnDagNode(obj).fullPathName().asChar());
    return false;
  }

#if MAYA_API_VERSION >= 20180000
    MDataHandle dataHandle = inputPointsSrc[0].asMDataHandle(&status);
#else
    MDataHandle dataHandle = inputPointsSrc[0].asMDataHandle(MDGContext::fsNormal, &status);
#endif

   MFnArrayAttrsData inputPointsData(dataHandle.data(), &status);
   CHECK_MSTATUS_AND_RETURN(status, false);

   //TODO: Need to consider timed animation
   UsdTimeCode usdTime = UsdTimeCode::Default();

   if (!setUSDInstancerArrayAttribute(stage, inputPointsData, usdPath, numElements, usdTime))
   {
       return false;
   }

   // Load the completed point instancer to compute and set its extent.This is not computing correct value for the test case
   stage->Load(instancer.GetPath());
   VtArray<GfVec3f> extent(2);
   if (instancer.ComputeExtentAtTime(&extent, usdTime, usdTime)) {
      UsdUtilsSparseValueWriter valueWriter;
      return valueWriter.SetAttribute(instancer.CreateExtentAttr(), VtValue::Take(extent), usdTime);
   }

}

//----------------------------------------------------------------------------------------------------------------------
template <typename T>
bool _SetUSDInstancerAttr(const UsdAttribute& usdAttr, const T &value, const UsdTimeCode &usdTime,
              UsdUtilsSparseValueWriter * valueWriter)
{
    return valueWriter ?
           valueWriter->SetAttribute(usdAttr, VtValue(value), usdTime) :
           usdAttr.Set(value, usdTime);
}

//----------------------------------------------------------------------------------------------------------------------
bool Instancer::setUSDInstancerArrayAttribute(UsdStageRefPtr stage, MFnArrayAttrsData& inputPointsData, const SdfPath& usdPath,
    const size_t numPrototypes, UsdTimeCode usdTime)
{
  MStatus status = MS::kFailure;

  bool result = false;
  UsdUtilsSparseValueWriter valueWriter;
  UsdGeomPointInstancer instancer = UsdGeomPointInstancer::Define(stage, usdPath);

  MFnArrayAttrsData::Type type;
  if (inputPointsData.checkArrayExist("id", type) && type == MFnArrayAttrsData::kDoubleArray)
  {
    const MDoubleArray ids = inputPointsData.doubleArray("id", &status);
    CHECK_MSTATUS_AND_RETURN(status, false);

    VtArray<int64_t> vtArray;
    _MapMayaToVtArrayValues<MDoubleArray, int64_t, double>(
        ids, vtArray, [](double x) {  return (int64_t)x; }
    );

   result = _SetUSDInstancerAttr(instancer.CreateIdsAttr(), vtArray, usdTime, &valueWriter);
   if (!result)
      return result;
   }

  if(inputPointsData.checkArrayExist("objectIndex", type) && type == MFnArrayAttrsData::kIntArray)
  {
    const MIntArray objectIndex = inputPointsData.intArray("objectIndex", &status);
    CHECK_MSTATUS_AND_RETURN(status, false);

    VtArray<int> vtArray;
    _MapMayaToVtArrayValues<MIntArray, int, int>(
        objectIndex, vtArray, [](int x) {  return x; }
    );

    result = _SetUSDInstancerAttr(instancer.CreateProtoIndicesAttr(), vtArray, usdTime, &valueWriter);
    if (!result)
      return result;
  }

  if(inputPointsData.checkArrayExist("position", type) && type == MFnArrayAttrsData::kVectorArray)
  {
    const MVectorArray position = inputPointsData.vectorArray("position", &status);
    CHECK_MSTATUS_AND_RETURN(status, false);

    VtArray<GfVec3f> vtArray;
    _MapMayaToVtArrayValues<MVectorArray, GfVec3f, MVector>(
        position, vtArray, [](MVector v) {  return GfVec3f(v.x, v.y, v.z); });

    result = _SetUSDInstancerAttr(instancer.CreatePositionsAttr(), vtArray, usdTime, &valueWriter);
    if (!result)
      return result;
  }

  if (inputPointsData.checkArrayExist("rotation", type) && type == MFnArrayAttrsData::kVectorArray)
  {
    const MVectorArray rotation = inputPointsData.vectorArray("rotation",&status);
    CHECK_MSTATUS_AND_RETURN(status, false);

    VtArray<GfQuath> vtArray;
    _MapMayaToVtArrayValues<MVectorArray, GfQuath, MVector>(
          rotation, vtArray, [](MVector v)
    {
      GfRotation rot = GfRotation(GfVec3d::XAxis(), v.x)
          * GfRotation(GfVec3d::YAxis(), v.y)
          * GfRotation(GfVec3d::ZAxis(), v.z);

      if (rot.GetAngle() == 0.0)
        return GfQuath(GfHalf(0));
      else return GfQuath(rot.GetQuat());
    });

    result = _SetUSDInstancerAttr(instancer.CreateOrientationsAttr(), vtArray, usdTime, &valueWriter);
    if (!result)
      return result;
  }

  if(inputPointsData.checkArrayExist("scale", type) && type == MFnArrayAttrsData::kVectorArray)
  {
    const MVectorArray scale = inputPointsData.vectorArray("scale", &status);
    CHECK_MSTATUS_AND_RETURN(status, false);

    VtArray<GfVec3f> vtArray;
    _MapMayaToVtArrayValues<MVectorArray, GfVec3f, MVector>(
          scale, vtArray, [](MVector v){ return GfVec3f(v.x, v.y, v.z); });

    result = _SetUSDInstancerAttr(instancer.CreateScalesAttr(), vtArray, usdTime, &valueWriter);
    if (!result)
      return result;
  }

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
MStatus Instancer::preTearDown(UsdPrim& prim)
{
  TF_DEBUG(ALUSDMAYA_TRANSLATORS).Msg("InstancerTranslator::preTearDown prim=%s\n", prim.GetPath().GetText());
  if(!prim.IsValid())
  {
    TF_DEBUG(ALUSDMAYA_TRANSLATORS).Msg("InstancerTranslator::preTearDown prim invalid\n");
    return MS::kFailure;
  }
  TranslatorBase::preTearDown(prim);

  // Write the overrides back to the path it was imported at
  MStatus status = MS::kSuccess;
  MObjectHandle handleToInstancer;
  if(!context()->getMObject(prim, handleToInstancer, MFn::kInstancer))
  {
    MGlobal::displayError("unable to locate instancer node");
    return MS::kFailure;
  }

  updateUsdPrim(prim.GetStage(), prim.GetPath(), handleToInstancer.object());

  return status;
}


//----------------------------------------------------------------------------------------------------------------------
MStatus Instancer::tearDown(const SdfPath &path)
{
  MObjectHandle obj;
  bool result = context()->getMObject(path, obj, MFn::kInstancer);

  if (result)
  {
    context()->removeItems(path);
    return MS::kSuccess;
  }
  else return MS::kFailure;
}

//----------------------------------------------------------------------------------------------------------------------
MStatus Instancer::update(const UsdPrim& prim)
{
  MStatus status = MS::kSuccess;

  MObjectHandle handleToInstancer;
  if(context() && !context()->getMObject(prim, handleToInstancer, MFn::kInstancer))
  {
    MGlobal::displayError("unable to locate instancer node");
    return MS::kFailure;
  }

  status = updateMayaAttributes(handleToInstancer.object(), prim);

  return status;
}

//----------------------------------------------------------------------------------------------------------------------
} // translators
} // fileio
} // usdmaya
} // AL
//----------------------------------------------------------------------------------------------------------------------

