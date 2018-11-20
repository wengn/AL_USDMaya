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
#include "AL/usdmaya/fileio/AnimationTranslator.h"
#include "AL/usdmaya/fileio/Export.h"
#include "AL/usdmaya/fileio/NodeFactory.h"
#include "AL/usdmaya/fileio/translators/TransformTranslator.h"
#include "AL/usdmaya/TransformOperation.h"
#include "AL/usdmaya/Metadata.h"

#include "maya/MAnimControl.h"
#include "maya/MAnimUtil.h"
#include "maya/MArgDatabase.h"
#include "maya/MDagPath.h"
#include "maya/MFnCamera.h"
#include "maya/MFnDagNode.h"
#include "maya/MFnMesh.h"
#include "maya/MFnTransform.h"
#include "maya/MGlobal.h"
#include "maya/MItDag.h"
#include "maya/MSyntax.h"
#include "maya/MNodeClass.h"
#include "maya/MObjectArray.h"
#include "maya/MPlug.h"
#include "maya/MPlugArray.h"
#include "maya/MSelectionList.h"
#include "maya/MUuid.h"

#include "pxr/usd/usd/modelAPI.h"
#include "pxr/usd/usd/timeCode.h"
#include "pxr/usd/usd/variantSets.h"
#include "pxr/usd/usd/primRange.h"
#include "pxr/usd/usdGeom/xform.h"
#include "pxr/usd/usdGeom/xformCommonAPI.h"
#include "pxr/usd/usdGeom/mesh.h"
#include "pxr/usd/usdGeom/nurbsCurves.h"
#include "pxr/base/gf/transform.h"
#include "pxr/usd/usdGeom/camera.h"
#include "pxr/usd/usdShade/material.h"
#include "pxr/usd/usdShade/materialBindingAPI.h"

#include <unordered_set>
#include <algorithm>
#include "AL/usdmaya/utils/Utils.h"
#include "AL/usdmaya/utils/MeshUtils.h"
#include "AL/usd/utils/SIMD.h"
#include "AL/maya/utils/MObjectMap.h"
#include <functional>

namespace AL {
namespace usdmaya {
namespace fileio {

AL_MAYA_DEFINE_COMMAND(ExportCommand, AL_usdmaya);

//----------------------------------------------------------------------------------------------------------------------
static unsigned int mayaDagPathToSdfPath(char* dagPathBuffer, unsigned int dagPathSize)
{
  char* writer = dagPathBuffer;
  char* backTrack = writer;

  for (const char *reader = dagPathBuffer, *end = reader + dagPathSize; reader != end; ++reader)
  {
    const char c = *reader;
    switch (c)
    {
      case ':':
        writer = backTrack;
        break;

      case '|':
        *writer = '/';
        ++writer;
        backTrack = writer;
        break;

      default:
        if (reader != writer)
          *writer = c;

        ++writer;
        break;
    }
  }

  return writer - dagPathBuffer;
}

//----------------------------------------------------------------------------------------------------------------------
static inline SdfPath makeUsdPath(const MDagPath& rootPath, const MDagPath& path)
{
  // if the rootPath is empty, we can just use the entire path
  const uint32_t rootPathLength = rootPath.length();
  if(!rootPathLength)
  {
    std::string fpn = AL::maya::utils::convert(path.fullPathName());
    fpn.resize(mayaDagPathToSdfPath(&fpn[0], fpn.size()));
    return SdfPath(fpn);
  }

  // otherwise we need to do a little fiddling.
  MString rootPathString, pathString, newPathString;
  rootPathString = rootPath.fullPathName();
  pathString = path.fullPathName();

  // trim off the root path from the object we are exporting
  newPathString = pathString.substring(rootPathString.length(), pathString.length());

  std::string fpn = AL::maya::utils::convert(newPathString);
  fpn.resize(mayaDagPathToSdfPath(&fpn[0], fpn.size()));
  return SdfPath(fpn);
}

//----------------------------------------------------------------------------------------------------------------------
static inline MDagPath getParentPath(const MDagPath& dagPath)
{
  MFnDagNode dagNode(dagPath);
  MFnDagNode parentNode(dagNode.parent(0));
  MDagPath parentPath;
  parentNode.getPath(parentPath);
  return parentPath;
}

//----------------------------------------------------------------------------------------------------------------------
static inline SdfPath makeMasterPath(UsdPrim instancesPrim, const MDagPath& dagPath)
{
  MString fullPathName = dagPath.fullPathName();
  std::string fullPath(fullPathName.asChar() + 1, fullPathName.length() - 1);
  std::string usdPath = instancesPrim.GetPath().GetString() + '/' + fullPath;
  std::replace(usdPath.begin(), usdPath.end(), '|', '_');
  std::replace(usdPath.begin(), usdPath.end(), ':', '_');
  return SdfPath(usdPath);
}

//----------------------------------------------------------------------------------------------------------------------
/// Internal USD exporter implementation
//----------------------------------------------------------------------------------------------------------------------
struct Export::Impl
{
  inline bool contains(const MFnDependencyNode& fn)
  {
    #if AL_UTILS_ENABLE_SIMD
    union
    {
      __m128i sse;
      AL::maya::utils::guid uuid;
    };
    fn.uuid().get(uuid.uuid);
    bool contains = m_nodeMap.find(sse) != m_nodeMap.end();
    if(!contains)
      m_nodeMap.insert(std::make_pair(sse, fn.object()));
    #else
    AL::maya::utils::guid uuid;
    fn.uuid().get(uuid.uuid);
    bool contains = m_nodeMap.find(uuid) != m_nodeMap.end();
    if(!contains)
      m_nodeMap.insert(std::make_pair(uuid, fn.object()));
    #endif
    return contains;
  }

  inline bool contains(const MObject& obj)
  {
    MFnDependencyNode fn(obj);
    return contains(fn);
  }

  inline SdfPath getMasterPath(const MFnDagNode& fn)
  {
    #if AL_UTILS_ENABLE_SIMD
    union
    {
      __m128i sse;
      AL::maya::utils::guid uuid;
    };
    fn.uuid().get(uuid.uuid);
    auto iterInst = m_instanceMap.find(sse);
    if (iterInst == m_instanceMap.end())
    {
      MDagPath dagPath;
      fn.getPath(dagPath);
      SdfPath instancePath = makeMasterPath(m_instancesPrim, dagPath);
      m_instanceMap.emplace(sse, instancePath);
      return instancePath;
    }
    #else
    AL::maya::utils::guid uuid;
    fn.uuid().get(uuid.uuid);
    auto iterInst = m_instanceMap.find(uuid);
    if (iterInst == m_instanceMap.end())
    {
      MDagPath dagPath;
      fn.getPath(dagPath);
      SdfPath instancePath = makeMasterPath(m_instancesPrim, dagPath);
      m_instanceMap.emplace(uuid, instancePath);
      return instancePath;
    }
    #endif
    return iterInst->second;
  }


  inline bool setStage(UsdStageRefPtr ptr)
  {
    m_stage = ptr;
    return m_stage;
  }

  inline UsdStageRefPtr stage()
  {
    return m_stage;
  }

  void setAnimationFrame(double minFrame, double maxFrame)
  {
    m_stage->SetStartTimeCode(minFrame);
    m_stage->SetEndTimeCode(maxFrame);
  }

  void setDefaultPrimIfOnlyOneRoot(SdfPath defaultPrim)
  {
    UsdPrim psuedo = m_stage->GetPseudoRoot();
    auto children = psuedo.GetChildren();
    auto first = children.begin();
    if(first != children.end())
    {
      auto next = first;
      ++next;
      if(next == children.end())
      {
        // if we get here, there is only one prim at the root level.
        // set that prim as the default prim.
        m_stage->SetDefaultPrim(*first);
      }
    }
    if (!m_stage->HasDefaultPrim() && !defaultPrim.IsEmpty())
    {
      m_stage->SetDefaultPrim(m_stage->GetPrimAtPath(defaultPrim));
    }
  }

  void filterSample()
  {
    std::vector<double> timeSamples;
    std::vector<double> dupSamples;
    for (auto prim : m_stage->Traverse())
    {
      std::vector<UsdAttribute> attributes = prim.GetAuthoredAttributes();
      for (auto attr : attributes)
      {
        timeSamples.clear();
        dupSamples.clear();
        attr.GetTimeSamples(&timeSamples);
        VtValue prevSampleBlob;
        for (auto sample : timeSamples)
        {
          VtValue currSampleBlob;
          attr.Get(&currSampleBlob, sample);
          if (prevSampleBlob == currSampleBlob)
          {
            dupSamples.emplace_back(sample);
          }
          else
          {
            prevSampleBlob = currSampleBlob;
            // only clear samples between constant segment
            if (dupSamples.size() > 1)
            {
              dupSamples.pop_back();
              for (auto dup : dupSamples)
              {
                attr.ClearAtTime(dup);
              }
            }
            dupSamples.clear();
          }
        }
        for (auto dup : dupSamples)
        {
          attr.ClearAtTime(dup);
        }
      }
    }
  }

  void doExport(const char* const filename, bool toFilter = false, SdfPath defaultPrim = SdfPath())
  {
    setDefaultPrimIfOnlyOneRoot(defaultPrim);
    if (toFilter)
    {
      filterSample();
    }
    m_stage->GetRootLayer()->Save();
    m_nodeMap.clear();
  }

  inline UsdPrim instancesPrim()
  {
    return m_instancesPrim;
  }

  void createInstancesPrim()
  {
    m_instancesPrim = m_stage->OverridePrim(SdfPath("/InstanceSources"));
  }

  void processInstances()
  {
    if (!m_instancesPrim)
      return;
    if (!m_instancesPrim.GetAllChildren())
    {
      m_stage->RemovePrim(m_instancesPrim.GetPrimPath());
    }
    else
    {
      m_instancesPrim.SetSpecifier(SdfSpecifierOver);
    }
  }

private:
  #if AL_UTILS_ENABLE_SIMD
  std::map<i128, MObject, AL::maya::utils::guid_compare> m_nodeMap;
  std::map<i128, SdfPath, AL::maya::utils::guid_compare> m_instanceMap;
  #else
  std::map<AL::maya::utils::guid, MObject, AL::maya::utils::guid_compare> m_nodeMap;
  std::map<AL::maya::utils::guid, SdfPath, AL::maya::utils::guid_compare> m_instanceMap;
  #endif
  UsdStageRefPtr m_stage;
  UsdPrim m_instancesPrim;
};

static MObject g_transform_rotateAttr = MObject::kNullObj;
static MObject g_transform_translateAttr = MObject::kNullObj;
static MObject g_handle_startJointAttr = MObject::kNullObj;
static MObject g_effector_handleAttr = MObject::kNullObj;
static MObject g_geomConstraint_targetAttr = MObject::kNullObj;

//----------------------------------------------------------------------------------------------------------------------
Export::Export(const ExporterParams& params)
  : m_params(params),
    m_impl(new Export::Impl),
    m_translatorManufacture(nullptr)
{
  if(g_transform_rotateAttr == MObject::kNullObj)
  {
    MNodeClass nct("transform");
    MNodeClass nch("ikHandle");
    MNodeClass nce("ikEffector");
    MNodeClass ngc("geometryConstraint");
    g_transform_rotateAttr = nct.attribute("r");
    g_transform_translateAttr = nct.attribute("t");
    g_handle_startJointAttr = nch.attribute("hsj");
    g_effector_handleAttr = nce.attribute("hp");
    g_geomConstraint_targetAttr = ngc.attribute("tg");
  }

  if(m_impl->setStage(UsdStage::CreateNew(m_params.m_fileName.asChar())))
  {
    doExport();
  }
}

//----------------------------------------------------------------------------------------------------------------------
Export::~Export()
{
  delete m_impl;
}

//----------------------------------------------------------------------------------------------------------------------
UsdPrim Export::exportMeshUV(MDagPath path, const SdfPath& usdPath)
{
  UsdPrim overPrim = m_impl->stage()->OverridePrim(usdPath);
  MStatus status;
  MFnMesh fnMesh(path, &status);
  AL_MAYA_CHECK_ERROR2(status, MString("unable to attach function set to mesh") + path.fullPathName());
  if (status == MStatus::kSuccess)
  {
    UsdGeomMesh mesh(overPrim);
    AL::usdmaya::utils::MeshExportContext context(path, mesh, UsdTimeCode::Default(), false);
    context.copyUvSetData();
  }
  return overPrim;
}

//----------------------------------------------------------------------------------------------------------------------
UsdPrim Export::exportAssembly(MDagPath path, const SdfPath& usdPath)
{
  UsdGeomXform mesh = UsdGeomXform::Define(m_impl->stage(), usdPath);
  return mesh.GetPrim();
}

//----------------------------------------------------------------------------------------------------------------------
UsdPrim Export::exportPluginLocatorNode(MDagPath path, const SdfPath& usdPath)
{
  UsdGeomXform mesh = UsdGeomXform::Define(m_impl->stage(), usdPath);
  return mesh.GetPrim();
}

//----------------------------------------------------------------------------------------------------------------------
UsdPrim Export::exportPluginShape(MDagPath path, const SdfPath& usdPath)
{
  UsdGeomXform mesh = UsdGeomXform::Define(m_impl->stage(), usdPath);
  return mesh.GetPrim();
}

//----------------------------------------------------------------------------------------------------------------------
void Export::exportGeometryConstraint(MDagPath constraintPath, const SdfPath& usdPath)
{
  auto animTranslator = m_params.m_animTranslator;
  if(!animTranslator)
  {
    return;
  }

  MPlug plug(constraintPath.node(), g_geomConstraint_targetAttr);
  for(uint32_t i = 0, n = plug.numElements(); i < n; ++i)
  {
    MPlug geom = plug.elementByLogicalIndex(i).child(0);
    MPlugArray connected;
    geom.connectedTo(connected, true, true);
    if(connected.length())
    {
      MPlug inputGeom = connected[0];
      MFnDagNode fn(inputGeom.node());
      MDagPath geomPath;
      fn.getPath(geomPath);
      if(AnimationTranslator::isAnimatedMesh(geomPath))
      {
        auto stage = m_impl->stage();

        // move to the constrained node
        constraintPath.pop();

        SdfPath newPath = usdPath.GetParentPath();

        UsdPrim prim = stage->GetPrimAtPath(newPath);
        if(prim)
        {
          UsdGeomXform xform(prim);
          bool reset;
          std::vector<UsdGeomXformOp> ops = xform.GetOrderedXformOps(&reset);
          for(auto op : ops)
          {
            const TransformOperation thisOp = xformOpToEnum(op.GetBaseName());
            if(thisOp == kTranslate)
            {
              animTranslator->forceAddPlug(MPlug(constraintPath.node(), g_transform_translateAttr), op.GetAttr());
              break;
            }
          }
          return;
        }
        else
        {
          std::cout << "prim not valid " << newPath.GetText() << std::endl;
        }
      }
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
void Export::exportIkChain(MDagPath effectorPath, const SdfPath& usdPath)
{
  auto animTranslator = m_params.m_animTranslator;
  if(!animTranslator)
  {
    return;
  }

  MPlug hp(effectorPath.node(), g_effector_handleAttr);
  hp = hp.elementByLogicalIndex(0);
  MPlugArray connected;
  hp.connectedTo(connected, true, true);
  if(connected.length())
  {
    // grab the handle node
    MObject handleObj = connected[0].node();

    MPlug translatePlug(handleObj, g_transform_translateAttr);

    // if the translation values on the ikHandle is animated, then we can assume the rotation values on the
    // joint chain between the effector and the start joint will also be animated
    bool handleIsAnimated = AnimationTranslator::isAnimated(translatePlug, true);
    if(handleIsAnimated)
    {
      // locate the start joint in the chain
      MPlug startJoint(handleObj, g_handle_startJointAttr);
      MPlugArray connected;
      startJoint.connectedTo(connected, true, true);
      if(connected.length())
      {
        // this will be the top chain in the system
        MObject startNode = connected[0].node();

        auto stage = m_impl->stage();
        SdfPath newPath = usdPath;

        UsdPrim prim;
        // now step up from the effector to the start joint and output the rotations
        do
        {
          // no point handling the effector
          effectorPath.pop();
          newPath = newPath.GetParentPath();

          prim = stage->GetPrimAtPath(newPath);
          if(prim)
          {
            UsdGeomXform xform(prim);
            MPlug rotatePlug(effectorPath.node(), g_transform_rotateAttr);
            bool reset;
            std::vector<UsdGeomXformOp> ops = xform.GetOrderedXformOps(&reset);
            for(auto op : ops)
            {
              const float radToDeg = 180.0f / 3.141592654f;
              bool added = false;
              switch(op.GetOpType())
              {
              case UsdGeomXformOp::TypeRotateXYZ:
              case UsdGeomXformOp::TypeRotateXZY:
              case UsdGeomXformOp::TypeRotateYXZ:
              case UsdGeomXformOp::TypeRotateYZX:
              case UsdGeomXformOp::TypeRotateZXY:
              case UsdGeomXformOp::TypeRotateZYX:
                added = true;
                animTranslator->forceAddPlug(rotatePlug, op.GetAttr(), radToDeg);
                break;
              default:
                break;
              }
              if(added) break;
            }
          }
          else
          {
            std::cout << "prim not valid" << std::endl;
          }
        }
        while(effectorPath.node() != startNode);
      }
    }
  }
  return;
}

//----------------------------------------------------------------------------------------------------------------------
void Export::copyTransformParams(UsdPrim prim, MFnTransform& fnTransform)
{
  translators::TransformTranslator::copyAttributes(fnTransform.object(), prim, m_params);
  if(m_params.m_dynamicAttributes)
  {
    translators::DgNodeTranslator::copyDynamicAttributes(fnTransform.object(), prim);
  }
}

//----------------------------------------------------------------------------------------------------------------------
SdfPath Export::determineUsdPath(MDagPath path, const SdfPath& usdPath, ReferenceType refType)
{
  switch (refType)
  {
    case kNoReference:
    {
      return usdPath;
    }
    break;

    case kShapeReference:
    {
      return makeMasterPath(m_impl->instancesPrim(), path);
    }
    break;

    case kTransformReference:
    {
      UsdStageRefPtr stage = m_impl->stage();
      SdfPath masterTransformPath = makeMasterPath(m_impl->instancesPrim(), getParentPath(path));
      if (!stage->GetPrimAtPath(masterTransformPath))
      {
        UsdGeomXform::Define(stage, masterTransformPath);
      }
      TfToken shapeName(MFnDagNode(path).name().asChar());
      return masterTransformPath.AppendChild(shapeName);
    }
    break;
  }
  return SdfPath();
}

//----------------------------------------------------------------------------------------------------------------------
void Export::addReferences(MDagPath shapePath, MFnTransform& fnTransform, SdfPath& usdPath,
                           const SdfPath& instancePath, ReferenceType refType)
{
  UsdStageRefPtr stage = m_impl->stage();
  if (refType == kShapeReference)
  {
    if (shapePath.node().hasFn(MFn::kMesh))
    {
      UsdGeomMesh::Define(stage, usdPath);
    }
    else if (shapePath.node().hasFn(MFn::kNurbsCurve))
    {
      UsdGeomNurbsCurves::Define(stage, usdPath);
    }
  }
  UsdPrim usdPrim = stage->GetPrimAtPath(usdPath);
  if (usdPrim)
  {
    // usd only allows instanceable on transform prim
    switch (refType)
    {
    case kTransformReference:
      {
        usdPrim.SetInstanceable(true);
      }
      break;

    case kShapeReference:
      {
        copyTransformParams(usdPrim, fnTransform);
      }
      break;

    default:
      break;
    }
    usdPrim.GetReferences().AddReference(SdfReference("", instancePath));
  }
}

//----------------------------------------------------------------------------------------------------------------------
void Export::exportShapesCommonProc(MDagPath shapePath, MFnTransform& fnTransform, SdfPath& usdPath,
                                    const ReferenceType refType)
{
  UsdPrim transformPrim;
  bool copyTransform = true;
  MFnDagNode dgNode(shapePath);

  translators::TranslatorManufacture::RefPtr translatorPtr = m_translatorManufacture.get(shapePath.node());
  if (translatorPtr)
  {
    SdfPath meshPath = determineUsdPath(shapePath, usdPath, refType);
    transformPrim = translatorPtr->exportObject(m_impl->stage(), shapePath, meshPath, m_params);
    copyTransform = (refType == kNoReference);
  }
  else // no translator register for this Maya type
  {
    if (shapePath.node().hasFn(MFn::kAssembly))
    {
      transformPrim = exportAssembly(shapePath, usdPath);
    }
    else
    if (shapePath.node().hasFn(MFn::kPluginLocatorNode))
    {
      transformPrim = exportPluginLocatorNode(shapePath, usdPath);
    }
    else
    if (shapePath.node().hasFn(MFn::kPluginShape))
    {
      transformPrim = exportPluginShape(shapePath, usdPath);
    }
  }

  // if we haven't created a transform for this shape (possible if we chose not to export it)
  // create a transform shape for the prim.
  if (!transformPrim)
  {
    UsdGeomXform xform = UsdGeomXform::Define(m_impl->stage(), usdPath);
    transformPrim = xform.GetPrim();
  }

  if(m_params.m_mergeTransforms && copyTransform)
  {
    copyTransformParams(transformPrim, fnTransform);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void Export::exportShapesOnlyUVProc(MDagPath shapePath, MFnTransform& fnTransform, SdfPath& usdPath)
{
  if(shapePath.node().hasFn(MFn::kMesh))
  {
    exportMeshUV(shapePath, usdPath);
  }
  else
  {
    m_impl->stage()->OverridePrim(usdPath);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void Export::exportSceneHierarchy(MDagPath rootPath, SdfPath& defaultPrim)
{
  MDagPath parentPath = rootPath;
  parentPath.pop();
  static const TfToken xformToken("Xform");

  MItDag it(MItDag::kDepthFirst);
  it.reset(rootPath, MItDag::kDepthFirst, MFn::kTransform);

  std::function<void(MDagPath, MFnTransform&, SdfPath&, ReferenceType)> exportShapeProc =
      [this] (MDagPath shapePath, MFnTransform& fnTransform, SdfPath& usdPath, ReferenceType refType)
  {
    this->exportShapesCommonProc(shapePath, fnTransform, usdPath, refType);
  };
  std::function<void(MDagPath, MFnTransform&, SdfPath&)> exportTransformFunc =
      [this] (MDagPath transformPath, MFnTransform& fnTransform, SdfPath& usdPath)
  {
    UsdGeomXform xform = UsdGeomXform::Define(m_impl->stage(), usdPath);
    UsdPrim transformPrim = xform.GetPrim();
    this->copyTransformParams(transformPrim, fnTransform);
  };

  // choose right proc required by meshUV option
  if (m_params.m_meshUV)
  {
    exportShapeProc =
        [this] (MDagPath shapePath, MFnTransform& fnTransform, SdfPath& usdPath, ReferenceType refType)
    {
      this->exportShapesOnlyUVProc(shapePath, fnTransform, usdPath);
    };
    exportTransformFunc =
          [this] (MDagPath transformPath, MFnTransform& fnTransform, SdfPath& usdPath)
    {
      m_impl->stage()->OverridePrim(usdPath);
    };
  }

  MFnTransform fnTransform;
  // loop through transforms only
  while(!it.isDone())
  {
    // assign transform function set
    MDagPath transformPath;
    it.getPath(transformPath);

    fnTransform.setObject(transformPath);

    // Make sure we haven't seen this transform before.
    bool transformHasBeenExported = m_impl->contains(fnTransform);
    if(transformHasBeenExported)
    {
      // We have an instanced shape!
      std::cout << "encountered transform instance " << fnTransform.fullPathName().asChar() << std::endl;
    }

    if(!transformHasBeenExported || m_params.m_duplicateInstances)
    {
      // generate a USD path from the current path
      SdfPath usdPath;

      // we should take a look to see whether the name was changed on import.
      // If it did change, make sure we save it using the original name, and not the new one.
      MStatus status;
      MPlug originalNamePlug = fnTransform.findPlug("alusd_originalPath", &status);
      if(!status)
      {
        usdPath = makeUsdPath(parentPath, transformPath);
      }

      // for UV only exporting, record first prim as default
      if (m_params.m_meshUV && defaultPrim.IsEmpty())
      {
        defaultPrim = usdPath;
      }

      if(transformPath.node().hasFn(MFn::kIkEffector))
      {
        exportIkChain(transformPath, usdPath);
      }
      else
      if(transformPath.node().hasFn(MFn::kGeometryConstraint))
      {
        exportGeometryConstraint(transformPath, usdPath);
      }

      // how many shapes are directly under this transform path?
      uint32_t numShapes;
      transformPath.numberOfShapesDirectlyBelow(numShapes);

      if(!m_params.m_mergeTransforms)
      {
        exportTransformFunc(transformPath, fnTransform, usdPath);
        UsdPrim prim = m_impl->stage()->GetPrimAtPath(usdPath);
        prim.SetMetadata<TfToken>(AL::usdmaya::Metadata::mergedTransform, AL::usdmaya::Metadata::unmerged);
      }

      if(numShapes)
      {
        // This is a slight annoyance about the way that USD has no concept of
        // shapes (it merges shapes into transforms usually). This means if we have
        // 1 transform, with 4 shapes parented underneath, it means we'll end up with
        // the transform data duplicated four times.

        ReferenceType refType = kNoReference;

        for(uint32_t j = 0; j < numShapes; ++j)
        {
          MDagPath shapePath = transformPath;
          shapePath.extendToShapeDirectlyBelow(j);
          MFnDagNode shapeDag(shapePath);
          SdfPath shapeUsdPath = usdPath;

          if(!m_params.m_mergeTransforms)
          {
            fnTransform.setObject(shapePath);
            shapeUsdPath = makeUsdPath(parentPath, shapePath);
          }

          bool shapeNotYetExported = !m_impl->contains(shapePath.node());
          bool shapeInstanced = shapePath.isInstanced();
          if(shapeNotYetExported || m_params.m_duplicateInstances)
          {
            // if the path has a child shape, process the shape now
            if (!m_params.m_duplicateInstances && shapeInstanced)
            {
              refType = m_params.m_mergeTransforms ? kShapeReference : kTransformReference;
            }
            exportShapeProc(shapePath, fnTransform, shapeUsdPath, refType);

            checkShapeShading(shapePath, shapeUsdPath);
          }
          else
          {
            refType = m_params.m_mergeTransforms ? kShapeReference : kTransformReference;
          }

          if (refType == kShapeReference)
          {
            SdfPath instancePath = m_impl->getMasterPath(shapeDag);
            addReferences(shapePath, fnTransform, shapeUsdPath, instancePath, refType);
          }
          else if (refType == kTransformReference)
          {
            SdfPath instancePath = m_impl->getMasterPath(MFnDagNode(shapeDag.parent(0)));
            addReferences(shapePath, fnTransform, usdPath, instancePath, refType);
          }
        }
      }
      else
      {
        if(m_params.m_mergeTransforms)
        {
          exportTransformFunc(transformPath, fnTransform, usdPath);
        }
      }
    }
    else
    {
      // We have an instanced transform
      // How do we reference that here?
    }

    it.next();
  }
}

//----------------------------------------------------------------------------------------------------------------------
void Export::checkShapeShading(MDagPath shapePath, SdfPath& usdPath)
{
  //Naiqi's test
  // Find its connecting node, if there is shading node and it is Arnold shading
  MStatus status;
  MString dagPathStr = shapePath.fullPathName();

  MFnMesh meshNode(shapePath, &status);
  AL_MAYA_CHECK_ERROR2(status, MString("Wrong creating meshNode"));

  if(shapePath.isInstanced())
  {
    MObjectArray shaderEngines;
    MIntArray indices;
    for(unsigned int i = 0; i < shapePath.instanceNumber(); i++)
    {
      meshNode.getConnectedShaders(i,shaderEngines,indices);
      for(unsigned int j = 0; j < shaderEngines.length(); j++)
      {
        //Find the arnold shader
        MFnDependencyNode shaderEnFn(shaderEngines[j]);

        std::string shaderEnName(shaderEnFn.name().asChar());
        if(shaderEnName.find("ai") == std::string::npos)
          continue;
        MPlug surfaceShaderPlug = shaderEnFn.findPlug(MString("surfaceShader"),&status);
        MPlugArray srcPlugs;
        surfaceShaderPlug.connectedTo(srcPlugs, true,false, &status);

        MObjectHandle aiSSNode(srcPlugs[0].node());
        if(std::find(m_aiSurfaceShaders.begin(), m_aiSurfaceShaders.end(), aiSSNode) == m_aiSurfaceShaders.end())
        {
           m_aiSurfaceShaders.push_back(aiSSNode);
        }
        m_shapeDagPaths.push_back(shapePath);
        m_shapeUsdPaths.push_back(usdPath);
      }
    }
  }else
  {
    MObjectArray shaderEngines;
    MIntArray indices;
    meshNode.getConnectedShaders(0,shaderEngines,indices);
    for(unsigned int j = 0; j < shaderEngines.length(); j++)
    {
      //Find the arnold shader
      MFnDependencyNode shaderEnFn(shaderEngines[j]);

      std::string shaderEnName(shaderEnFn.name().asChar());
      if(shaderEnName.find("ai") == std::string::npos)
        continue;

      MPlug surfaceShaderPlug = shaderEnFn.findPlug(MString("surfaceShader"),&status);
      MPlugArray srcPlugs;
      surfaceShaderPlug.connectedTo(srcPlugs, true,false, &status);

      MObjectHandle aiSSNode(srcPlugs[0].node());
      if(std::find(m_aiSurfaceShaders.begin(), m_aiSurfaceShaders.end(), aiSSNode) == m_aiSurfaceShaders.end())
      {
        m_aiSurfaceShaders.push_back(aiSSNode);
      }
      m_shapeDagPaths.push_back(shapePath);
      m_shapeUsdPaths.push_back(usdPath);
     }
 }

}

//----------------------------------------------------------------------------------------------------------------------
void Export::exportAIShader()
{
  MStatus status = MS::kSuccess;
  if(m_aiSurfaceShaders.size()== 0)
    return;

  //This material and shader will be written to the root path
  for(std::vector<MObjectHandle>::iterator it = m_aiSurfaceShaders.begin(); it != m_aiSurfaceShaders.end(); ++it)
  {
    UsdPrim matPrim;
    translators::TranslatorManufacture::RefPtr translatorPtr = m_translatorManufacture.get((*it).object());
    if (translatorPtr)
    {
      SdfPath emptyPath;
      matPrim = translatorPtr->exportObject(m_impl->stage(), (*it).object(), emptyPath, m_params);
      UsdShadeMaterial usdMat(matPrim);
      if(!usdMat)
          std::cout<<"UsdShadeMaterial is not created correctly"<<std::endl;

      std::string newMatNameStr = matPrim.GetName();
      std::string shadingNodeName = newMatNameStr.substr(0, newMatNameStr.find('_'));

      // Travese all the m_shapeDagPaths list and find connected shape
      // Bind the material to the shape
      for(std::vector<MDagPath>::iterator it = m_shapeDagPaths.begin(); it != m_shapeDagPaths.end(); ++it)
      {
        MFnMesh meshFn(*it, &status);
        MObjectArray shaders;
        MIntArray indices;

        // It does not matter whether this is instanced, as all the related dag path shapes have been added into this list
        meshFn.getConnectedShaders(0,shaders, indices);
        for(unsigned int j = 0; j < shaders.length(); j++)
        {
          //Find the arnold shader
          MFnDependencyNode shadersFn(shaders[j]);
          std::string curShaderStr = std::string(shadersFn.name().asChar());
          if(std::string(shadersFn.name().asChar()).find(shadingNodeName) == std::string::npos)
            continue;

          UsdPrim shapePrim = m_impl->stage()->GetPrimAtPath(m_shapeUsdPaths[it - m_shapeDagPaths.begin()]);
          if(!shapePrim)
              std::cout<<"Shape prim is not valid"<<std::endl;
          UsdShadeMaterialBindingAPI bindAPI(shapePrim);
          if(usdMat && shapePrim)
            bindAPI.Bind(usdMat, UsdShadeTokens->fallbackStrength, UsdShadeTokens->preview);
        }
      }
    }
  }

/*
  // Material is a special case there is no one-on-one relationship between a sdfPath and a MObject in the above exportObject() call
  // Traverse the m_shapePaths list and bind the shape to corresponding material
  for(std::vector<MDagPath>::iterator it = m_shapeDagPaths.begin(); it != m_shapeDagPaths.end(); ++it)
  {
    MFnMesh meshFn(*it, &status);
    if((*it).isInstanced())
    {
      for(unsigned int i = 0; i < (*it).instanceNumber(); i++)
      {
        MObjectArray shaders;
        MIntArray indices;
        meshFn.getConnectedShaders(i,shaders, indices);
        for(unsigned int j = 0; j < shaders.length(); j++)
        {
          //Find the arnold shader
          MFnDependencyNode shadersFn(shaders[j]);
          if(std::string(shadersFn.name().asChar()).find("ai") == std::string::npos)
            continue;

          SdfPath matPath(SdfPath::AbsoluteRootPath().GetString() + std::string(shadersFn.name(&status).asChar()) + "_" + "Mat");
          UsdShadeMaterial usdMat(m_impl->stage()->GetPrimAtPath(matPath));
          UsdPrim shapePrim = m_impl->stage()->GetPrimAtPath(m_shapeUsdPaths[it - m_shapeDagPaths.begin()]);
          if(!shapePrim)
          {
            UsdShadeMaterialBindingAPI bindAPI(shapePrim);

            if(!usdMat)
              bindAPI.Bind(usdMat, UsdShadeTokens->fallbackStrength, UsdShadeTokens->preview);
            else
            {
              std::cout<<"material is not valid"<<std::endl;
            }
          }else
          {
              std::cout<<"the prim is not valid"<<std::endl;
          }
        }
      }
    }
    else
    {
      MObjectArray shaders;
      MIntArray indices;
      meshFn.getConnectedShaders(0,shaders, indices);
      for(unsigned int j = 0; j < shaders.length(); j++)
      {
        //Find the arnold shader
        MFnDependencyNode shadersFn(shaders[j]);
        if(std::string(shadersFn.name().asChar()).find("ai") == std::string::npos)
          continue;

        SdfPath matPath(SdfPath::AbsoluteRootPath().GetString() + std::string(shadersFn.name(&status).asChar()) + "_" + "Mat");
        std::string matStr = matPath.GetString();
        UsdPrim matPrim = m_impl->stage()->GetPrimAtPath(matPath);
        UsdShadeMaterial usdMat(m_impl->stage()->GetPrimAtPath(matPath));

        SdfPath testSdf = m_shapeUsdPaths[it - m_shapeDagPaths.begin()];
        std::string sdfStr = testSdf.GetString();
        UsdPrim shapePrim = m_impl->stage()->GetPrimAtPath(m_shapeUsdPaths[it - m_shapeDagPaths.begin()]);
        MString testDag = (*it).fullPathName();
        std::string testDagStr(testDag.asChar());


        if(!shapePrim && (!matPrim))
        {
          UsdShadeMaterialBindingAPI bindAPI(shapePrim);

          if(!usdMat)
            bindAPI.Bind(usdMat, UsdShadeTokens->fallbackStrength, UsdShadeTokens->preview);
          else
          {
            std::cout<<"material is not valid"<<std::endl;
          }
        }else
        {
            std::cout<<"the prim is not valid"<<std::endl;
        }



        UsdShadeMaterialBindingAPI bindAPI(shapePrim);
        //bindAPI.Bind(usdMat, UsdShadeTokens->fallbackStrength, UsdShadeTokens->preview);
      }
    }
  }
  */
}

//----------------------------------------------------------------------------------------------------------------------
void Export::doExport()
{
  // make sure the node factory has been initialised as least once prior to use
  getNodeFactory();

  const MTime oldCurTime = MAnimControl::currentTime();
  if(m_params.m_animTranslator)
  {
    // try to ensure that we have some sort of consistent output for each run by forcing the export to the first frame
    MAnimControl::setCurrentTime(m_params.m_minFrame);
  }

  if(!m_params.m_duplicateInstances)
  {
    m_impl->createInstancesPrim();
  }

  MObjectArray objects;
  const MSelectionList& sl = m_params.m_nodes;
  SdfPath defaultPrim;

  for(uint32_t i = 0, n = sl.length(); i < n; ++i)
  {
    MDagPath path;
    if(sl.getDagPath(i, path))
    {
      if(path.node().hasFn(MFn::kTransform))
      {
        exportSceneHierarchy(path, defaultPrim);
      }
      else
      if(path.node().hasFn(MFn::kShape))
      {
        path.pop();
        exportSceneHierarchy(path, defaultPrim);
      }
    }
    else
    {
      MObject obj;
      sl.getDependNode(i, obj);
      objects.append(obj);
    }
  }

  exportAIShader(); //Naiqi's test

  if(m_params.m_animTranslator)
  {
    m_params.m_animTranslator->exportAnimation(m_params);
    m_impl->setAnimationFrame(m_params.m_minFrame, m_params.m_maxFrame);

    // return user to their original frame
    MAnimControl::setCurrentTime(oldCurTime);
  }

  m_impl->processInstances();
  m_impl->doExport(m_params.m_fileName.asChar(), m_params.m_filterSample, defaultPrim);
}

//----------------------------------------------------------------------------------------------------------------------
ExportCommand::ExportCommand()
 : m_params()
{
}

//----------------------------------------------------------------------------------------------------------------------
ExportCommand::~ExportCommand()
{
}

//----------------------------------------------------------------------------------------------------------------------
MStatus ExportCommand::doIt(const MArgList& args)
{
  MStatus status;
  MArgDatabase argData(syntax(), args, &status);
  AL_MAYA_CHECK_ERROR(status, "ALUSDExport: failed to match arguments");
  AL_MAYA_COMMAND_HELP(argData, g_helpText);

  // fetch filename and ensure it's valid
  if(!argData.isFlagSet("f", &status))
  {
    MGlobal::displayError("ALUSDExport: \"file\" argument must be set");
    return MS::kFailure;
  }
  AL_MAYA_CHECK_ERROR(argData.getFlagArgument("f", 0, m_params.m_fileName), "ALUSDExport: Unable to fetch \"file\" argument");
  if(argData.isFlagSet("sl", &status))
  {
    AL_MAYA_CHECK_ERROR(argData.getFlagArgument("sl", 0, m_params.m_selected), "ALUSDExport: Unable to fetch \"selected\" argument");
  }
  if(argData.isFlagSet("da", &status))
  {
    AL_MAYA_CHECK_ERROR(argData.getFlagArgument("da", 0, m_params.m_dynamicAttributes), "ALUSDExport: Unable to fetch \"dynamic\" argument");
  }
  if(argData.isFlagSet("di", &status))
  {
    AL_MAYA_CHECK_ERROR(argData.getFlagArgument("di", 0, m_params.m_duplicateInstances), "ALUSDExport: Unable to fetch \"duplicateInstances\" argument");
  }
  if(argData.isFlagSet("m", &status))
  {
    AL_MAYA_CHECK_ERROR(argData.getFlagArgument("m", 0, m_params.m_meshes), "ALUSDExport: Unable to fetch \"meshes\" argument");
  }
  if(argData.isFlagSet("muv", &status))
  {
    AL_MAYA_CHECK_ERROR(argData.getFlagArgument("muv", 0, m_params.m_meshUV), "ALUSDExport: Unable to fetch \"meshUV\" argument");
  }
  if(argData.isFlagSet("luv", &status))
  {
    MGlobal::displayWarning("-luv flag is deprecated in AL_usdmaya_ExportCommand\n");
  }
  if(argData.isFlagSet("mt", &status))
  {
    AL_MAYA_CHECK_ERROR(argData.getFlagArgument("mt", 0, m_params.m_mergeTransforms), "ALUSDExport: Unable to fetch \"merge transforms\" argument");
  }
  if(argData.isFlagSet("nc", &status))
  {
    AL_MAYA_CHECK_ERROR(argData.getFlagArgument("nc", 0, m_params.m_nurbsCurves), "ALUSDExport: Unable to fetch \"nurbs curves\" argument");
  }

  if(argData.isFlagSet("fr", &status))
  {
    AL_MAYA_CHECK_ERROR(argData.getFlagArgument("fr", 0, m_params.m_minFrame), "ALUSDExport: Unable to fetch \"frame range\" argument");
    AL_MAYA_CHECK_ERROR(argData.getFlagArgument("fr", 1, m_params.m_maxFrame), "ALUSDExport: Unable to fetch \"frame range\" argument");
    m_params.m_animation = true;
  }
  else if(argData.isFlagSet("ani", &status))
  {
    m_params.m_animation = true;
    m_params.m_minFrame = MAnimControl::minTime().value();
    m_params.m_maxFrame = MAnimControl::maxTime().value();
  }

  if(argData.isFlagSet("ss", &status))
  {
    AL_MAYA_CHECK_ERROR(argData.getFlagArgument("ss", 0, m_params.m_subSamples), "ALUSDExport: Unable to fetch \"sub samples\" argument");
  }

  if (argData.isFlagSet("fs", &status))
  {
    AL_MAYA_CHECK_ERROR(argData.getFlagArgument("fs", 0, m_params.m_filterSample), "ALUSDExport: Unable to fetch \"filter sample\" argument");
  }
  if(argData.isFlagSet("eac", &status))
  {
    AL_MAYA_CHECK_ERROR(argData.getFlagArgument("eac", 0, m_params.m_extensiveAnimationCheck), "ALUSDExport: Unable to fetch \"extensive animation check\" argument");
  }

  if(m_params.m_animation)
  {
    m_params.m_animTranslator = new AnimationTranslator;
  }
  return redoIt();
}

//----------------------------------------------------------------------------------------------------------------------
MStatus ExportCommand::redoIt()
{
  static const std::unordered_set<std::string> ignoredNodes
  {
    "persp",
    "front",
    "top",
    "side"
  };

  if(m_params.m_selected)
  {
    MGlobal::getActiveSelectionList(m_params.m_nodes);
  }
  else
  {
    MDagPath path;
    MItDag it(MItDag::kDepthFirst, MFn::kTransform);
    while(!it.isDone())
    {
      it.getPath(path);
      const MString name = path.partialPathName();
      const std::string s(name.asChar(), name.length());
      if(ignoredNodes.find(s) == ignoredNodes.end())
      {
        m_params.m_nodes.add(path);
      }
      it.prune();
      it.next();
    }
  }

  Export exporter(m_params);
  delete m_params.m_animTranslator;

  return MS::kSuccess;
}

//----------------------------------------------------------------------------------------------------------------------
MStatus ExportCommand::undoIt()
{
  return MS::kSuccess;
}

//----------------------------------------------------------------------------------------------------------------------
MSyntax ExportCommand::createSyntax()
{
  const char* const errorString = "ALUSDExport: failed to create syntax";

  MStatus status;
  MSyntax syntax;
  status = syntax.addFlag("-f" , "-file", MSyntax::kString);
  AL_MAYA_CHECK_ERROR2(status, errorString);
  status = syntax.addFlag("-sl" , "-selected", MSyntax::kBoolean);
  AL_MAYA_CHECK_ERROR2(status, errorString);
  status = syntax.addFlag("-da" , "-dynamic", MSyntax::kBoolean);
  AL_MAYA_CHECK_ERROR2(status, errorString);
  status = syntax.addFlag("-m" , "-meshes", MSyntax::kBoolean);
  AL_MAYA_CHECK_ERROR2(status, errorString);
  status = syntax.addFlag("-muv" , "-meshUV", MSyntax::kBoolean);
  AL_MAYA_CHECK_ERROR2(status, errorString);
  status = syntax.addFlag("-luv" , "-leftHandedUV", MSyntax::kBoolean);
  AL_MAYA_CHECK_ERROR2(status, errorString);
  status = syntax.addFlag("-nc" , "-nurbsCurves", MSyntax::kBoolean);
  AL_MAYA_CHECK_ERROR2(status, errorString);
  status = syntax.addFlag("-di" , "-duplicateInstances", MSyntax::kBoolean);
  AL_MAYA_CHECK_ERROR2(status, errorString);
  status = syntax.addFlag("-mt", "-mergeTransforms", MSyntax::kBoolean);
  AL_MAYA_CHECK_ERROR2(status, errorString);
  status = syntax.addFlag("-ani", "-animation", MSyntax::kNoArg);
  AL_MAYA_CHECK_ERROR2(status, errorString);
  status = syntax.addFlag("-fr", "-frameRange", MSyntax::kDouble, MSyntax::kDouble);
  AL_MAYA_CHECK_ERROR2(status, errorString);
  status = syntax.addFlag("-fs", "-filterSample", MSyntax::kBoolean);
  AL_MAYA_CHECK_ERROR2(status, errorString);
  status = syntax.addFlag("-eac", "-extensiveAnimationCheck", MSyntax::kBoolean);
  AL_MAYA_CHECK_ERROR2(status, errorString);
  status = syntax.addFlag("-ss", "-subSamples", MSyntax::kUnsigned);
  AL_MAYA_CHECK_ERROR2(status, errorString);
  syntax.enableQuery(false);
  syntax.enableEdit(false);

  return syntax;
}

//----------------------------------------------------------------------------------------------------------------------
const char* const ExportCommand::g_helpText = R"(
ExportCommand Overview:

  This command will export your maya scene into the USD format. If you want the export to happen from 
  a certain point in the hierarchy then select the node in maya and pass the parameter selected=True, otherwise
  it will export from the root of the scene.

  If you want to export keeping the time sampled data, you can do so by passing these flags
    1. AL_usdmaya_ExportCommand -f "<path/to/out/file.usd>" -animation

  Exporting attributes that are dynamic attributes can be done by:
    1. AL_usdmaya_ExportCommand -f "<path/to/out/file.usd>" -dynamic

  Exporting samples over a framerange can be done a few ways:
    1. AL_usdmaya_ExportCommand -f "<path/to/out/file.usd>" -frameRange 0 24
    2. AL_usdmaya_ExportCommand -f "<path/to/out/file.usd>" -ani

  Nurbs curves can be exported by passing the corresponding parameters:
    1. AL_usdmaya_ExportCommand -f "<path/to/out/file.usd>" -nc
  
  The exporter can remove samples that contain the same data for adjacent samples
    1. AL_usdmaya_ExportCommand -f "<path/to/out/file.usd>" -fs
)";

//----------------------------------------------------------------------------------------------------------------------
} // fileio
} // usdmaya
} // AL
//----------------------------------------------------------------------------------------------------------------------
