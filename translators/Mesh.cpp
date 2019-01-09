
//
// Copyright 2017 Animal Logic
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
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

#include "pxr/usd/usd/modelAPI.h"
#include "pxr/usd/usd/timeCode.h"
#include "pxr/usd/usdGeom/mesh.h"
#include "maya/MFloatPointArray.h"
#include "maya/MVectorArray.h"
#include "maya/MIntArray.h"
#include "maya/MFnTypedAttribute.h"
#include "maya/MFnMesh.h"
#include "maya/MFnSet.h"
#include "maya/MFnStringData.h"
#include "maya/MFileIO.h"
#include "maya/MNodeClass.h"

#include "AL/usdmaya/utils/DiffPrimVar.h"
#include "AL/usdmaya/utils/MeshUtils.h"

#include "AL/usdmaya/DebugCodes.h"
#include "AL/usdmaya/fileio/translators/DagNodeTranslator.h"
#include "AL/usdmaya/fileio/AnimationTranslator.h"
#include "AL/usdmaya/nodes/ProxyShape.h"
#include "AL/usdmaya/Metadata.h"
#include "pxr/usd/usdGeom/mesh.h"
#include "pxr/usd/usdShade/materialBindingAPI.h" //Naiqi's change
#include "pxr/usd/usdShade/tokens.h"

#include "Mesh.h"

namespace AL {
namespace usdmaya {
namespace fileio {
namespace translators {

AL_USDMAYA_DEFINE_TRANSLATOR(Mesh, PXR_NS::UsdGeomMesh)

//----------------------------------------------------------------------------------------------------------------------
MStatus Mesh::initialize()
{
  //Initialise all the class plugs
  return MStatus::kSuccess;
}

//----------------------------------------------------------------------------------------------------------------------
MStatus Mesh::import(const UsdPrim& prim, MObject& parent, MObject& createdObj)
{
  TF_DEBUG(ALUSDMAYA_TRANSLATORS).Msg("Mesh::import prim=%s\n", prim.GetPath().GetText());

  const UsdGeomMesh mesh(prim);

  TranslatorContextPtr ctx = context();
  UsdTimeCode timeCode = (ctx && ctx->getForceDefaultRead()) ? UsdTimeCode::Default() : UsdTimeCode::EarliestTime();

  bool parentUnmerged = false;
  TfToken val;
  if(prim.GetParent().GetMetadata(AL::usdmaya::Metadata::mergedTransform, &val))
  {
    parentUnmerged = (val == AL::usdmaya::Metadata::unmerged);
  }
  MString dagName = prim.GetName().GetString().c_str();
  if(!parentUnmerged)
  {
    dagName += "Shape";
  }

  AL::usdmaya::utils::MeshImportContext importContext(mesh, parent, dagName, timeCode);
  importContext.applyVertexNormals();
  importContext.applyHoleFaces();
  importContext.applyVertexCreases();
  importContext.applyEdgeCreases();

  MObject initialShadingGroup;
  DagNodeTranslator::initialiseDefaultShadingGroup(initialShadingGroup);
  // Apply default material to Shape
  MStatus status;
  MFnSet fn(initialShadingGroup, &status);
  AL_MAYA_CHECK_ERROR(status, "Unable to attach MfnSet to initialShadingGroup");
  
  createdObj = importContext.getPolyShape();
  fn.addMember(createdObj);
  importContext.applyPrimVars();

  if (ctx)
  {
    ctx->addExcludedGeometry(prim.GetPath());
    ctx->insertItem(prim, createdObj);
  }
  return MStatus::kSuccess;
}

//----------------------------------------------------------------------------------------------------------------------
UsdPrim Mesh::exportObject(UsdStageRefPtr stage, MDagPath dagPath, const SdfPath& usdPath, const ExporterParams& params)
{
  if(!params.m_meshes)
    return UsdPrim();

  UsdGeomMesh mesh = UsdGeomMesh::Define(stage, usdPath);

  AL::usdmaya::utils::MeshExportContext context(dagPath, mesh, params.m_timeCode, false, (AL::usdmaya::utils::MeshExportContext::CompactionLevel)params.m_compactionLevel);
  if(context)
  {
    UsdAttribute pointsAttr = mesh.GetPointsAttr();
    if (params.m_animTranslator && AnimationTranslator::isAnimatedMesh(dagPath))
    {
      params.m_animTranslator->addMesh(dagPath, pointsAttr);
    }

    if(params.m_meshPoints)
    {
      context.copyVertexData(context.timeCode());
    }
    if(params.m_meshConnects)
    {
      context.copyFaceConnectsAndPolyCounts();
    }
    if(params.m_meshHoles)
    {
      context.copyInvisibleHoles();
    }
    if(params.m_meshUvs)
    {
      context.copyUvSetData();
    }
    if(params.m_meshNormals)
    {
      context.copyNormalData(context.timeCode());
    }
    if(params.m_meshColours)
    {
      context.copyColourSetData();
    }
    if(params.m_meshVertexCreases)
    {
      context.copyCreaseVertices();
    }
    if(params.m_meshEdgeCreases)
    {
      context.copyCreaseEdges();
    }
    if (params.m_meshPointsAsPref)
    {
      context.copyBindPoseData(context.timeCode());
    }

    // pick up any additional attributes attached to the mesh node (these will be added alongside the transform attributes)
    if(params.m_dynamicAttributes)
    {
      UsdPrim prim = mesh.GetPrim();
      DgNodeTranslator::copyDynamicAttributes(dagPath.node(), prim);
    }
  }
  return mesh.GetPrim();
}

//----------------------------------------------------------------------------------------------------------------------
MStatus Mesh::tearDown(const SdfPath& path)
{
  TF_DEBUG(ALUSDMAYA_TRANSLATORS).Msg("MeshTranslator::tearDown prim=%s\n", path.GetText());

  context()->removeItems(path);
  context()->removeExcludedGeometry(path);
  return MS::kSuccess;
}

//----------------------------------------------------------------------------------------------------------------------
MStatus Mesh::update(const UsdPrim& path)
{
  return MS::kSuccess;
}

//----------------------------------------------------------------------------------------------------------------------
MStatus Mesh::preTearDown(UsdPrim& prim)
{
  TF_DEBUG(ALUSDMAYA_TRANSLATORS).Msg("MeshTranslator::preTearDown prim=%s\n", prim.GetPath().GetText());\
  if(!prim.IsValid())
  {
    TF_DEBUG(ALUSDMAYA_TRANSLATORS).Msg("Mesh::preTearDown prim invalid\n");
    return MS::kFailure;
  }
  TranslatorBase::preTearDown(prim);

  /* TODO
   * This block was put in since writeEdits modifies USD and thus triggers the OnObjectsChanged callback which will then tearDown
   * the this Mesh prim. The writeEdits method will then continue attempting to copy maya mesh data to USD but will end up crashing
   * since the maya mesh has now been removed by the tearDown.
   *
   * I have tried turning off the TfNotice but I get the 'Detected usd threading violation. Concurrent changes to layer(s) composed'
   * error.
   *
   * This crash and error seems to be happening mainly when switching out a variant that contains a Mesh, and that Mesh has been
   * force translated into Maya.
   */
  TfNotice::Block block;
  // Write the overrides back to the path it was imported at
  MObjectHandle obj;
  context()->getMObject(prim, obj, MFn::kInvalid);
  if(obj.isValid())
  {
    MFnDagNode fn(obj.object());
    MDagPath path;
    fn.getPath(path);
    MStatus status;
    MFnMesh fnMesh(path, &status);
    AL_MAYA_CHECK_ERROR(status, MString("unable to attach function set to mesh: ") + path.fullPathName());

    UsdGeomMesh geomPrim(prim);
    writeEdits(path, geomPrim, kPerformDiff | kDynamicAttributes);
  }
  else
  {
    TF_DEBUG(ALUSDMAYA_TRANSLATORS).Msg("Unable to find the corresponding Maya Handle at prim path '%s'\n", prim.GetPath().GetText());
    return MS::kFailure;
  }
  return MS::kSuccess;
}

//----------------------------------------------------------------------------------------------------------------------
void Mesh::writeEdits(MDagPath& dagPath, UsdGeomMesh& geomPrim, uint32_t options)
{
  TF_DEBUG(ALUSDMAYA_TRANSLATORS).Msg("MeshTranslator::writing edits to prim='%s'\n", geomPrim.GetPath().GetText());
  UsdTimeCode t = UsdTimeCode::Default();
  AL::usdmaya::utils::MeshExportContext context(dagPath, geomPrim, t, options & kPerformDiff);
  if(context)
  {
    context.copyVertexData(t);
    context.copyNormalData(t);
    context.copyFaceConnectsAndPolyCounts();
    context.copyInvisibleHoles();
    context.copyCreaseVertices();
    context.copyCreaseEdges();
    context.copyUvSetData();
    context.copyColourSetData();
    context.copyBindPoseData(t);
    if(options & kDynamicAttributes)
    {
      UsdPrim prim = geomPrim.GetPrim();
      DgNodeTranslator::copyDynamicAttributes(dagPath.node(), prim);
    }
  }
}

//Naiqi's change
//----------------------------------------------------------------------------------------------------------------------
MStatus Mesh::postImport(const UsdPrim& prim)
{
  MStatus status = MS::kSuccess;

  //TODO: check if it is a valid geomPrim
  UsdShadeMaterialBindingAPI bindAPI(prim);
  if(!bindAPI)
      return MS::kFailure;
  SdfPath matPath = bindAPI.GetDirectBinding().GetMaterialPath();

  MSelectionList matList;
  MGlobal::getSelectionListByName(MString(matPath.GetName().c_str()), matList);

  if(matList.length() == 0)
  {
      std::cout<<"Could not find material with name: "<<matPath.GetName().c_str()<<std::endl;
      return MS::kFailure;
  }

  if(matList.length() > 1)
  {
    // Assume there are no more than one material with the same name
    std::cout<<"There are more than one material with the same name: "<<matPath.GetName().c_str()<<std::endl;
    return MS::kFailure;
  }

  MObject matNode;
  status = matList.getDependNode(0, matNode);
  MFnDependencyNode matNodeFn(matNode, &status);

  MObjectHandle shapeNode;
  context()->getMObject(prim.GetPath(), shapeNode, MFn::kMesh);
  if(shapeNode.isValid())
  {
    // TODO: need to consider situations that part of the shape is in the set
    MFnDependencyNode shapeFn(shapeNode.object(), &status);

    //Add a custom attribute to record usdPath, so that when material is written out, it can find write out corresponding shape usd path
    MFnStringData strData;
    MObject strObj = strData.create(MString(prim.GetPath().GetText()), &status);
    MFnTypedAttribute attr;
    MObject attrObj = attr.create("sdfPath", "spt", MFnData::kString, strObj);
    shapeFn.addAttribute(attrObj, MFnDependencyNode::kLocalDynamicAttr);

    MPlug instObjGroupsPlug = shapeFn.findPlug("instObjGroups");
    // Currently assuming a shape is only connected to one shader
    MPlug instObjGroupsElemPlug = instObjGroupsPlug.elementByLogicalIndex(0);
    MDGModifier dgMod;

    // Need to get rid of old connections if there is any
    MFnMesh meshFn(shapeNode.object(), &status);
    MObjectArray shaders;
    MIntArray indices;
    meshFn.getConnectedShaders(0, shaders, indices);
    if(shaders.length() != 0)
    {
      for(unsigned int i = 0; i < shaders.length(); i++)
      {
        MFnDependencyNode shaderFn(shaders[i]);
        MPlugArray destArray;
        instObjGroupsElemPlug.connectedTo(destArray, false, true, &status);

        for(unsigned int j = 0; j < destArray.length(); j++)
        {
          std::string testStr(destArray[j].partialName().asChar());
          if(destArray[j].partialName() == MString("dsm"))
            status = dgMod.disconnect(instObjGroupsElemPlug, destArray[j]);
        }
      }
    }

    MPlug dagSetMemberPlug = matNodeFn.findPlug("dagSetMembers");
    MPlug dagSetMemberElemPlug = dagSetMemberPlug.elementByLogicalIndex(dagSetMemberPlug.numElements()+1);
    status = dgMod.connect(instObjGroupsElemPlug, dagSetMemberElemPlug);
    status = dgMod.doIt();
  }
  else
  {
      TF_DEBUG(ALUSDMAYA_TRANSLATORS).Msg("Mesh::postImport prim=%s\n", prim.GetPath().GetText());
      return MS::kFailure;
  }
  return status;
}

//----------------------------------------------------------------------------------------------------------------------
} // namespace translators
} // namespace fileio
} // namespace usdmaya
} // namespace AL
//----------------------------------------------------------------------------------------------------------------------

