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

#include "AL/usdmaya/utils/DgNodeHelper.h"
#include "AL/usdmaya/fileio/AnimationTranslator.h"
#include "AL/usdmaya/fileio/translators/DgNodeTranslator.h"

#include "maya/MDagModifier.h"
#include "maya/MDataHandle.h"
#include "maya/MFnArrayAttrsData.h"
#include "maya/MFnInstancer.h"
#include "maya/MFnNumericAttribute.h"
#include "maya/MFnMesh.h"
#include "maya/MFnParticleSystem.h"
#include "maya/MFnTransform.h"
#include "maya/MPlugArray.h"
#include "maya/MPointArray.h"

namespace AL {
namespace usdmaya {
namespace fileio {
namespace translators {

AL_USDMAYA_DEFINE_TRANSLATOR(Instancer, PXR_NS::UsdGeomPointInstancer)

//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
template <typename MayaArrayType, typename V,  typename M>
void _MapVtToMayaArrayValues(
    MayaArrayType& mArray,
    const VtArray<V>& vtArray,
    const std::function<M (const V)> mapper)
{
  mArray.clear();
  for (unsigned int i = 0; i < vtArray.size(); ++i)
  {
    mArray.append(mapper(vtArray[i]));
  }
}

template<typename MayaArrayType, typename V, typename M>
void _MapMayaToVtArrayValues(
    const MayaArrayType& mArray,
    VtArray<V>& vtArray,
    const std::function<V (const M)> mapper)
{
  vtArray.clear();
  vtArray.resize(mArray.length());
  for (unsigned int i = 0; i < mArray.length(); ++i)
  {
    vtArray[i] = mapper(mArray[i]);
  }
}


//----------------------------------------------------------------------------------------------------------------------
MStatus Instancer::initialize()
{
  MStatus status = MS::kSuccess;
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
  AL_MAYA_CHECK_ERROR2(status, MString("unable to create maya instancer node "));

  MFnInstancer instFn(createObj, &status);
  instFn.setName(MString(prim.GetName().GetText()), &status);

  TranslatorContextPtr ctx = context();
  UsdTimeCode timeCode = (ctx && ctx->getForceDefaultRead()) ? UsdTimeCode::Default() : UsdTimeCode::EarliestTime();
  if(ctx)
  {
    ctx->insertItem(prim, createObj);
  }
  status = updateMayaAttributes(createObj, prim);

  return status;
}



//----------------------------------------------------------------------------------------------------------------------
bool Instancer::setMayaInstancerArrayAttr(MFnArrayAttrsData& inputPointsData, const UsdAttribute& usdAttr, MString attrName, const std::string& type)
{
  MStatus status;
  if(type == "point" || type == "vector")
  {
    MVectorArray mayaVecArray = inputPointsData.vectorArray(attrName, &status);
    VtArray<GfVec3f> usdVecArray;
    usdAttr.Get(&usdVecArray);

    _MapVtToMayaArrayValues<MVectorArray, GfVec3f, MVector>(
          mayaVecArray, usdVecArray, [](GfVec3f v){ return MVector(v[0], v[1], v[2]); }
    );
  }
  else if (type == "int32")
  {
    MIntArray mayaIntArray = inputPointsData.intArray(attrName, &status);
    VtArray<int> usdIntArray;
    usdAttr.Get(&usdIntArray);
    if(usdIntArray.size() == 0)
      std::cout<<"Error getting usd array data"<<std::endl;
    _MapVtToMayaArrayValues<MIntArray, int, int>(
          mayaIntArray, usdIntArray, [](int x ){ return x; }
     );
  }
  else if (type == "int64" ||type == "double")
  {
    MDoubleArray mayaDoubleArray = inputPointsData.doubleArray(attrName, &status);
    VtArray<int64_t> usdIntArray;
    usdAttr.Get(&usdIntArray);

    _MapVtToMayaArrayValues<MDoubleArray, int64_t, double>(
          mayaDoubleArray, usdIntArray, [](int64_t x ){ return double(x); }
     );
  }else if (type == "quat")
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
  }

}


//----------------------------------------------------------------------------------------------------------------------
MStatus Instancer::updateMayaAttributes(MObject& mayaObj, const UsdPrim& prim)
{
  MStatus status = MS::kFailure;

  UsdGeomPointInstancer instancer(prim);
  if(!instancer)
    return status;
  SdfPath instancerPath = prim.GetPrimPath();

  MFnInstancer instFn(mayaObj, &status);
  if(status == MS::kFailure)
  {
    MGlobal::displayError("Unable to create function set on instancer node");
    return status;
  }

  //Create a maya particle node into the Maya scene to record all data and connect it to this instance node
  MPlug instancePointDataPlug;
  bool particleResult = setupParticleNode(mayaObj, prim,instancePointDataPlug );

  //Usually the instancer data comes from connection from a particle node, so setting the data directly on instancer node doesn't really making sense now


  MFnArrayAttrsData inputPointsData;
  MObject inputPointsObj = inputPointsData.create(&status);

  UsdAttribute protoIdsAttr = instancer.GetIdsAttr();
  if (!protoIdsAttr.HasValue()) {
    TF_WARN("Point instancer %s does not have a 'Ids'"
            "attribute. Not adding it to the render index.",
            instancerPath.GetText());
    return status;
  }
  setMayaInstancerArrayAttr(inputPointsData, protoIdsAttr, MString("id"), std::string("int64"));


  UsdAttribute protoIndicesAttr = instancer.GetProtoIndicesAttr();
  if (!protoIndicesAttr.HasValue()) {
    TF_WARN("Point instancer %s does not have a 'protoIndices'"
            "attribute. Not adding it to the render index.",
            instancerPath.GetText());
    return status;
  }
 setMayaInstancerArrayAttr(inputPointsData, protoIndicesAttr, MString("objectIndex"), std::string("int32"));

  UsdAttribute positionsAttr = instancer.GetPositionsAttr();
  if (!positionsAttr.HasValue()) {
    TF_WARN("Point instancer %s does not have a 'positions' attribute. "
            "Not adding it to the render index.", instancerPath.GetText());
    return status;
  }
  setMayaInstancerArrayAttr(inputPointsData, positionsAttr, MString("position"), std::string("point"));

  UsdAttribute orientationAttr = instancer.GetOrientationsAttr();
  if (!orientationAttr.HasValue()) {
    TF_WARN("Point instancer %s does not have a 'orientations' attribute. "
            "Not adding it to the render index.", instancerPath.GetText());
    return status;
  }
  setMayaInstancerArrayAttr(inputPointsData, orientationAttr, MString("rotation"), std::string("quat"));

  UsdAttribute scaleAttr = instancer.GetScalesAttr();
  if (!scaleAttr.HasValue()) {
    TF_WARN("Point instancer %s does not have a 'scales' attribute. "
            "Not adding it to the render index.", instancerPath.GetText());
    return status;
  }
 setMayaInstancerArrayAttr(inputPointsData, scaleAttr, MString("scale"), std::string("vector"));

 status = instancePointDataPlug.setValue(inputPointsObj);


  return status;
}

//----------------------------------------------------------------------------------------------------------------------
bool Instancer::setupParticleNode(MObject& mayaObj, const UsdPrim& prim, MPlug& instPointDataPlug )
{
  MStatus status = MS::kFailure;

  MDagModifier modifier;
  MObject parentObj = modifier.createNode("particle",MObject::kNullObj, &status);
  status = modifier.doIt();

  if(parentObj.isNull())
    return MS::kFailure;

  MFnDagNode transformFn(parentObj, &status);
  MObject particleObj = transformFn.child(0, &status);
  if(particleObj.isNull())
  {
    return MS::kFailure;
  }

  MFnParticleSystem particleFn(particleObj, &status);
  MPlug instanceDataArrayPlug = particleFn.findPlug("instanceData");
  MPlug instanceDataPlug = instanceDataArrayPlug.elementByLogicalIndex(0, &status);
  CHECK_MSTATUS_AND_RETURN(status, false);

  if(instanceDataPlug.numChildren() != 2)
    MGlobal::displayError("InstanceData element plug does not have 2 child plugs\n");

  instPointDataPlug = instanceDataPlug.child(1,&status);

  MFnInstancer instancerFn(mayaObj, &status);
  MPlug destPlug = instancerFn.findPlug("inputPoints", &status);

  //Connect it to instancer node
  MDGModifier dgMod;
  status = dgMod.connect(instPointDataPlug, destPlug);
  dgMod.doIt();
  CHECK_MSTATUS_AND_RETURN(status, false);

}

//----------------------------------------------------------------------------------------------------------------------
MStatus Instancer::postImport(const UsdPrim& prim)
{
  //TODO: maybe should add a flag, only when the flag is set, the instancer is converted to maya instancer and correponding
  //mesh needs to be converted too

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

            MDagModifier mod;
            mod.connect(matrixPlug, inputHierPlug.elementByLogicalIndex(it - protoPaths.begin()));
            status = mod.doIt();
          }
          else
            return MS::kFailure;
        }
      }
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
  MFnDagNode dagNode(obj,&status);

  //Setting up prototype prim
  const MPlug inputHierarchy = dagNode.findPlug("inputHierarchy", true, &status);
  CHECK_MSTATUS_AND_RETURN(status, false);

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

  MPlug inputPointsDest = dagNode.findPlug("inputPoints", true, &status);
  CHECK_MSTATUS_AND_RETURN(status, false);

  MPlugArray inputPointsSrc;
  inputPointsDest.connectedTo(inputPointsSrc, true, false, &status);
  if (inputPointsSrc.length() < 1)
  {
    TF_WARN("inputPoints not connected on instancer '%s'", dagNode.fullPathName().asChar());
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

   // Load the completed point instancer to compute and set its extent.
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
  MStatus status;

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

    bool test = _SetUSDInstancerAttr(instancer.CreateIdsAttr(), vtArray, usdTime, &valueWriter);

    if (!test)
      return test;
   }

  if(inputPointsData.checkArrayExist("objectIndex", type) && type == MFnArrayAttrsData::kIntArray)
  {
    const MIntArray objectIndex = inputPointsData.intArray("objectIndex", &status);
    CHECK_MSTATUS_AND_RETURN(status, false);

    VtArray<int> vtArray;
    _MapMayaToVtArrayValues<MIntArray, int, int>(
        objectIndex, vtArray, [](int x) {  return x; }
    );

    bool test2 = _SetUSDInstancerAttr(instancer.CreateProtoIndicesAttr(), vtArray, usdTime, &valueWriter);

    if (!test2)
      return test2;
  }

  if(inputPointsData.checkArrayExist("position", type) && type == MFnArrayAttrsData::kVectorArray)
  {
    const MVectorArray position = inputPointsData.vectorArray("position", &status);
    CHECK_MSTATUS_AND_RETURN(status, false);

    VtArray<GfVec3f> vtArray;
    _MapMayaToVtArrayValues<MVectorArray, GfVec3f, MVector>(
        position, vtArray, [](MVector v) {  return GfVec3f(v.x, v.y, v.z); });

    bool test3 = _SetUSDInstancerAttr(instancer.CreatePositionsAttr(), vtArray, usdTime, &valueWriter);

    if (!test3)
      return test3;
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

    bool test4 = _SetUSDInstancerAttr(instancer.CreateOrientationsAttr(), vtArray, usdTime, &valueWriter);

    if (!test4)
      return test4;
  }

  if(inputPointsData.checkArrayExist("scale", type) && type == MFnArrayAttrsData::kVectorArray)
  {
    const MVectorArray scale = inputPointsData.vectorArray("scale", &status);
    CHECK_MSTATUS_AND_RETURN(status, false);

    VtArray<GfVec3f> vtArray;
    _MapMayaToVtArrayValues<MVectorArray, GfVec3f, MVector>(
          scale, vtArray, [](MVector v){ return GfVec3f(v.x, v.y, v.z); });

    bool test5 = _SetUSDInstancerAttr(instancer.CreateScalesAttr(), vtArray, usdTime, &valueWriter);
    if (!test5)
      return test5;
  }
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
MStatus Instancer::preTearDown(UsdPrim& prim)
{
  MStatus status = MS::kSuccess;
  return status;
}


//----------------------------------------------------------------------------------------------------------------------
MStatus Instancer::tearDown(const SdfPath &path)
{
  MStatus status = MS::kSuccess;
  return status;

}

//----------------------------------------------------------------------------------------------------------------------
MStatus Instancer::update(const UsdPrim& prim)
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

