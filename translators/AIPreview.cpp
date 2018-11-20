//
// Copyright 2018 Original Force
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

#include "AIPreview.h"
#include "pxr/usd/usdGeom/mesh.h"
#include "pxr/usdImaging/usdImaging/tokens.h"
#include "pxr/usd/usdShade/shader.h"
#include "pxr/usd/usdShade/material.h"
#include "pxr/usd/usdShade/materialBindingAPI.h"
#include "pxr/usd/sdf/valueTypeName.h"
#include "pxr/usd/usd/timeCode.h"

#include "AL/usdmaya/utils/DgNodeHelper.h"
#include "AL/usdmaya/fileio/AnimationTranslator.h"
#include "AL/usdmaya/fileio/translators/DgNodeTranslator.h"

#include "maya/MDagPath.h"
#include "maya/MDGModifier.h"
#include "maya/MFnPlugin.h"
#include "maya/MGlobal.h"
#include "maya/MTime.h"
#include "maya/MDistance.h"
#include "maya/MFileIO.h"
#include "maya/MFnDagNode.h"
#include "maya/MFnSet.h"
#include "maya/MNodeClass.h"
#include "maya/M3dView.h"

namespace AL {
namespace usdmaya {
namespace fileio {
namespace translators {

AL_USDMAYA_DEFINE_TRANSLATOR(AIPreview, PXR_NS::UsdShadeShader)

//----------------------------------------------------------------------------------------------------------------------


//----------------------------------------------------------------------------------------------------------------------
MStatus AIPreview::initialize()
{
  // First need to check if the plugin is loaded correctly
  MObject pluginObj = MFnPlugin::findPlugin("mtoa");
  if( pluginObj == MObject::kNullObj)
  {
    MGlobal::displayError("The mtoa plugin is not loaded.");
    return MS::kFailure;
  }
  return MS::kSuccess;
}

//----------------------------------------------------------------------------------------------------------------------
MStatus AIPreview::import(const UsdPrim& prim, MObject& parent, MObject& createdObj)
{
  MStatus status = MS::kSuccess;
  UsdShadeShader pbrShader(prim);
  std::string curSdfPath = pbrShader.GetPath().GetString();

/*  MFnDependencyNode shaderFn;
  MObject to =  shaderFn.create("aiStandardSurface", prim.GetName().GetText(), &status);
  createdObj = to;
*/
  MString createShaderResult;
  MString createShader("createNode ");
  createShader += "aiStandardSurface;\n";
  status = MGlobal::executeCommand(createShader,createShaderResult);
  MSelectionList shaderList;
  status = MGlobal::getSelectionListByName(createShaderResult, shaderList);
  status = shaderList.getDependNode(0, createdObj);

  status = updateMayaAttributes(createdObj, pbrShader);

  TranslatorContextPtr ctx = context();
  if(ctx)
  {
    ctx->insertItem(prim, createdObj);
  }

  // Create shading group
  MString createSGResult;
  MString createSG("createNode ");
  createSG += "shadingEngine;\n";
  status = MGlobal::executeCommand(createSG, createSGResult);
  MSelectionList shdingEngineList;
  status = MGlobal::getSelectionListByName(createSGResult, shdingEngineList);
  MObject shadingEngineObj;
  status = shdingEngineList.getDependNode(0, shadingEngineObj);

  // Connect shading group with shader
  MFnDependencyNode shaderFn(createdObj, &status);
  MPlug outColorPlug = shaderFn.findPlug("outColor", &status);
  MFnDependencyNode shadingEngineFn(shadingEngineObj, &status);
  shadingEngineFn.setName(shaderFn.name()+"SG");

  MPlug surfaceShaderPlug = shadingEngineFn.findPlug("surfaceShader", &status);
  MDGModifier mod;
  status = mod.connect(outColorPlug, surfaceShaderPlug);
  status = mod.doIt();

  return status;
}

//----------------------------------------------------------------------------------------------------------------------
MStatus AIPreview::updateMayaAttributes(MObject to, UsdShadeShader& usdShader)
{
  MStatus status = MS::kSuccess;
  MFnDependencyNode shaderFn(to, &status);

  const char* const errorString = "AIPriewTranslator: error setting arnold parameters";
  int useSpecularWorkflow = 0;
  float metalness = 0.0;
  float specularRoughness = 0.0;
  float coat = 0.0;
  float coatColor[3] = {1.0, 1.0, 1.0};
  float coatRoughness = 0.0;
  float specularIOR = 0.0;
  float transmission = 0.0;
  float opacity = 0.0;

  auto diffuseAttr = usdShader.GetInput(TfToken("diffuseColor"));
  VtValue diffuse;
  diffuseAttr.Get(&diffuse);
  const GfVec3f rawVal = diffuse.Get<GfVec3f>();
  MObject baseColorObj = shaderFn.attribute("baseColor", &status);
  AL_MAYA_CHECK_ERROR(DgNodeTranslator::setVec3(to, baseColorObj, rawVal[0], rawVal[1], rawVal[2]), errorString);
  MObject baseObj = shaderFn.attribute("base", &status);
  AL_MAYA_CHECK_ERROR(DgNodeTranslator::setFloat(to, baseObj, 1.0), errorString);

  auto emissiveAttr = usdShader.GetInput(TfToken("emissiveColor"));
  VtValue emissive;
  emissiveAttr.Get(&emissive);
  const GfVec3f emissiveVal = emissive.Get<GfVec3f>();
  MObject emissionColorObj = shaderFn.attribute("emissionColor", &status);
  AL_MAYA_CHECK_ERROR(DgNodeTranslator::setVec3(to, emissionColorObj, emissiveVal[0], emissiveVal[1], emissiveVal[2]), errorString);
  MObject emissionObj = shaderFn.attribute("emission", &status);
  AL_MAYA_CHECK_ERROR(DgNodeTranslator::setFloat(to, emissionObj, 1.0), errorString);

  auto specularWorkflowAttr = usdShader.GetInput(TfToken("useSpecularWorkflow"));
  specularWorkflowAttr.Get(&useSpecularWorkflow);

  if(useSpecularWorkflow == 0)
  {
    auto metallicAttr = usdShader.GetInput(TfToken("metallic"));
    metallicAttr.Get(&metalness);
    MObject metalnessObj = shaderFn.attribute("metalness", &status);
    AL_MAYA_CHECK_ERROR(DgNodeTranslator::setFloat(to, metalnessObj, metalness), errorString);

  }else
  {
    auto specularColorAttr = usdShader.GetInput(TfToken("specularColor"));
    VtValue specul;
    specularColorAttr.Get(&specul);
    const GfVec3f speculVal = specul.Get<GfVec3f>();
    MObject specularColorObj = shaderFn.attribute("specularColor", &status);
    AL_MAYA_CHECK_ERROR(DgNodeTranslator::setVec3(to, specularColorObj, speculVal[0], speculVal[1], speculVal[2]), errorString);
    MObject specularObj = shaderFn.attribute("specular", &status);
    AL_MAYA_CHECK_ERROR(DgNodeTranslator::setFloat(to, specularObj, 1.0), errorString);

    auto roughnessAttr = usdShader.GetInput(TfToken("roughness"));
    roughnessAttr.Get(&specularRoughness);
    MObject specularRoughnessObj = shaderFn.attribute("specularRoughness", &status);
    AL_MAYA_CHECK_ERROR(DgNodeTranslator::setFloat(to, specularRoughnessObj, specularRoughness), errorString);
  }

  auto clearcoatAttr = usdShader.GetInput(TfToken("clearcoat"));
  clearcoatAttr.Get(&coat);
  MObject coatObj = shaderFn.attribute("coat", &status);
  AL_MAYA_CHECK_ERROR(DgNodeTranslator::setFloat(to, coatObj, coat), errorString);

  // In usdPreviewSurface, coat color is assumed to be white, set it corresponding in maya
  MObject coatColorObj = shaderFn.attribute("coatColor", &status);
  AL_MAYA_CHECK_ERROR(DgNodeTranslator::setVec3(to, coatColorObj, coatColor), errorString);

  auto clearcoatRoughnessAttr = usdShader.GetInput(TfToken("clearcoatRoughness"));
  clearcoatRoughnessAttr.Get(&coatRoughness);
  MObject coatRoughnessObj = shaderFn.attribute("coatRoughness", &status);
  AL_MAYA_CHECK_ERROR(DgNodeTranslator::setFloat(to, coatRoughnessObj, coatRoughness), errorString);

  auto iorAttr = usdShader.GetInput(TfToken("ior"));
  iorAttr.Get(&specularIOR);
  MObject iorObj = shaderFn.attribute("specularIOR", &status);
  AL_MAYA_CHECK_ERROR(DgNodeTranslator::setFloat(to, iorObj, specularIOR), errorString);

  // Didn't use transmission, directly map "opacity" over
  auto opacityAttr = usdShader.GetInput(TfToken("opacity"));
  opacityAttr.Get(&opacity);
  MObject opacityObj = shaderFn.attribute("opacity", &status);
  AL_MAYA_CHECK_ERROR(DgNodeTranslator::setFloat(to, opacityObj, opacity), errorString);

  auto normalAttr = usdShader.GetInput(TfToken("normal"));
  VtValue norm;
  normalAttr.Get(&norm);
  const GfVec3f normalVal = norm.Get<GfVec3f>();
  MObject normalObj = shaderFn.attribute("normalCamera", &status);
  AL_MAYA_CHECK_ERROR(DgNodeTranslator::setVec3(to, normalObj, normalVal[0], normalVal[1], normalVal[2]), errorString);

  return status;
}

//----------------------------------------------------------------------------------------------------------------------
UsdPrim AIPreview::exportObject(UsdStageRefPtr stage, MObject obj, const SdfPath& usdPath,
                             const ExporterParams& params)
{
  MStatus status = MS::kSuccess;

  //Retrive all shading information
  float base = 0.0;
  float baseColor[3] = {0.0, 0.0, 0.0};
  float emission = 0.0;
  float emissionColor[3] = {0.0, 0.0, 0.0};
  float metalness = 0.0;
  float specular = 0.0;
  float specularColor[3] = {0.0, 0.0, 0.0};
  float specularRoughness = 0.0;
  float coat = 0.0;
  float coatRoughness = 0.0;
  float specularIOR = 0.0;
  float transmission = 0.0;
  float opacity = 0.0;
  float normal[3] = { 0.0, 0.0, 0.0};
  MFnDependencyNode shaderFn(obj, &status);
  MObject baseAttr= shaderFn.attribute(MString("base"),&status);
  MObject baseColorAttr = shaderFn.attribute(MString("baseColor"), &status);
  MObject emissionAttr = shaderFn.attribute(MString("emission"), &status);
  MObject emissionColorAttr = shaderFn.attribute(MString("emissionColor"), &status);
  MObject metalnessAttr = shaderFn.attribute(MString("metalness"), &status);
  MObject specularAttr = shaderFn.attribute(MString("specular"), &status);
  MObject specularColorAttr = shaderFn.attribute(MString("specularColor"), &status);
  MObject specularRoughAttr = shaderFn.attribute(MString("specularRoughness"), &status);
  MObject coatAttr = shaderFn.attribute(MString("coat"), &status);
  MObject coatRoughnessAttr = shaderFn.attribute(MString("coatRoughness"), &status);
  MObject specularIORAttr = shaderFn.attribute(MString("specularIOR"), &status);
  MObject transmissionAttr = shaderFn.attribute(MString("transmission"), &status);
  MObject opacityAttr = shaderFn.attribute(MString("opacity"), &status);
  MObject normalAttr = shaderFn.attribute(MString("normalCamera"), &status);

  const char* const errorString = "AIPreviewTranslator: error getting maya aiSurfaceShader parameters";
  AL_MAYA_CHECK_ERROR2(AL::usdmaya::utils::DgNodeHelper::getFloat(obj, baseAttr, base), errorString);
  AL_MAYA_CHECK_ERROR2(AL::usdmaya::utils::DgNodeHelper::getVec3(obj, baseColorAttr, baseColor), errorString);
  AL_MAYA_CHECK_ERROR2(AL::usdmaya::utils::DgNodeHelper::getFloat(obj, emissionAttr, emission), errorString);
  AL_MAYA_CHECK_ERROR2(AL::usdmaya::utils::DgNodeHelper::getVec3(obj, emissionColorAttr, emissionColor), errorString);
  AL_MAYA_CHECK_ERROR2(AL::usdmaya::utils::DgNodeHelper::getFloat(obj, metalnessAttr, metalness), errorString);
  AL_MAYA_CHECK_ERROR2(AL::usdmaya::utils::DgNodeHelper::getFloat(obj, specularAttr, specular), errorString);
  AL_MAYA_CHECK_ERROR2(AL::usdmaya::utils::DgNodeHelper::getVec3(obj, specularColorAttr, specularColor), errorString);
  AL_MAYA_CHECK_ERROR2(AL::usdmaya::utils::DgNodeHelper::getFloat(obj, specularRoughAttr, specularRoughness), errorString);
  AL_MAYA_CHECK_ERROR2(AL::usdmaya::utils::DgNodeHelper::getFloat(obj, coatAttr, coat), errorString);
  AL_MAYA_CHECK_ERROR2(AL::usdmaya::utils::DgNodeHelper::getFloat(obj, coatRoughnessAttr, coatRoughness), errorString);
  AL_MAYA_CHECK_ERROR2(AL::usdmaya::utils::DgNodeHelper::getFloat(obj, specularIORAttr, specularIOR), errorString);
  AL_MAYA_CHECK_ERROR2(AL::usdmaya::utils::DgNodeHelper::getFloat(obj, transmissionAttr, transmission), errorString);
  AL_MAYA_CHECK_ERROR2(AL::usdmaya::utils::DgNodeHelper::getFloat(obj, opacityAttr, opacity), errorString);
  AL_MAYA_CHECK_ERROR2(AL::usdmaya::utils::DgNodeHelper::getVec3(obj, normalAttr, normal), errorString);

  //Create the material at the root layer
  SdfPath matPath(SdfPath::AbsoluteRootPath().GetString() + std::string(shaderFn.name(&status).asChar()) + "_" + "Mat");
  UsdShadeMaterial usdMat = UsdShadeMaterial::Define(stage, matPath);
  UsdShadeShader previewShader = UsdShadeShader::Define(stage, SdfPath(usdMat.GetPath().GetString() + "/pbrShader"));
  previewShader.CreateIdAttr(VtValue(UsdImagingTokens->UsdPreviewSurface));

  float diffuseColor[3] = {base * baseColor[0], base * baseColor[1], base * baseColor[2]};
  previewShader.CreateInput(TfToken("diffuseColor"), SdfValueTypeNames->Color3f).Set(GfVec3f(diffuseColor));

  float emissiveColor[3] = {emission * emissionColor[0], emission * emissionColor[1], emission * emissionColor[2]};
  previewShader.CreateInput(TfToken("emissiveColor"), SdfValueTypeNames->Color3f).Set(GfVec3f(emissiveColor));

  UsdShadeInput useSpecular = previewShader.CreateInput(TfToken("useSpecularWorkflow"), SdfValueTypeNames->Int);
  MPlug metalnessPlug(obj, metalnessAttr);
  if(metalness > 0.0 || metalnessPlug.isConnected(&status))
  {
    //ignore the specular workflow
    useSpecular.Set(VtValue(0));
    previewShader.CreateInput(TfToken("metallic"), SdfValueTypeNames->Float).Set(metalness);
  }
  else
  {
    useSpecular.Set(VtValue(1));
    float spec[3] = {specular * specularColor[0], specular * specularColor[1], specular * specularColor[2]};
    previewShader.CreateInput(TfToken("specularColor"), SdfValueTypeNames->Color3f).Set(GfVec3f(spec));
    previewShader.CreateInput(TfToken("roughness"), SdfValueTypeNames->Float).Set(specularRoughness);
  }

  previewShader.CreateInput(TfToken("clearcoat"), SdfValueTypeNames->Float).Set(coat);
  previewShader.CreateInput(TfToken("clearcoatRoughness"), SdfValueTypeNames->Float).Set(coatRoughness);

  previewShader.CreateInput(TfToken("ior"), SdfValueTypeNames->Float).Set(specularIOR);

  // depending if the shader supports refraction or not, it will either raytrace refractions
  // based on the amount of "transmission", or it will **not** raytrace a thing and will
  // instead consider the "opacity" parameter
  if(transmission > 0.0)
      previewShader.CreateInput(TfToken("opacity"), SdfValueTypeNames->Float).Set(transmission);
  else
      previewShader.CreateInput(TfToken("opacity"), SdfValueTypeNames->Float).Set(opacity);

  previewShader.CreateInput(TfToken("normal"), SdfValueTypeNames->Float3).Set(GfVec3f(normal));
  usdMat.CreateSurfaceOutput().ConnectToSource(previewShader,TfToken("surface"));

  stage->GetRootLayer()->Save(); //It looks like material is not written into stage, this didn't help?

  // Note: this is returning material instead of shader
  return usdMat.GetPrim();
}

//----------------------------------------------------------------------------------------------------------------------
MStatus AIPreview::tearDown(const SdfPath& path)
{
  MObjectHandle obj;
  context()->getMObject(path, obj, MFn::kSurfaceShader);  //TODO: need to double check

  context()->removeItems(path);
  return MS::kSuccess;
}

//----------------------------------------------------------------------------------------------------------------------
ExportFlag AIPreview::canExport(const MObject& obj)
{
  MStatus status = MS::kSuccess;
  if(obj.isNull()) return ExportFlag::kNotSupported;
  if(obj.hasFn(MFn::kPluginDependNode))   //The problem is aiSurfaceShader only shows type as kPluginDependNode
  {
    MFnDependencyNode depFn(obj, &status);
    if(std::string(depFn.name().asChar()).find("ai") != std::string::npos)
        return ExportFlag::kFallbackSupport;
  }
  return ExportFlag::kNotSupported;

}

//----------------------------------------------------------------------------------------------------------------------
} // translators
} // fileio
} // usdmaya
} // AL
//----------------------------------------------------------------------------------------------------------------------
