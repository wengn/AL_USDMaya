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

#include "FileTexture.h"

#include "AL/usdmaya/utils/DgNodeHelper.h"
#include "AL/usdmaya/fileio/translators/DgNodeTranslator.h"
#include "pxr/usdImaging/usdImaging/tokens.h"
#include "pxr/usd/usdShade/connectableAPI.h"

#include "maya/MNodeClass.h"
#include "maya/MPlugArray.h"
#include "maya/MStatus.h"

namespace AL {
namespace usdmaya {
namespace fileio {
namespace translators {

AL_USDMAYA_DEFINE_TRANSLATOR(FileTexture, PXR_NS::UsdShadeShader)

//----------------------------------------------------------------------------------------------------------------------
MObject FileTexture::m_uvTilingMode;
MObject FileTexture::m_fileTextureName;
MObject FileTexture::m_computedFileTextureNamePattern;
MObject FileTexture::m_defaultColor;
MObject FileTexture::m_colorGain;
MObject FileTexture::m_alphaGain;
MObject FileTexture::m_colorOffset;
MObject FileTexture::m_alphaOffset;
MObject FileTexture::m_outColor;
MObject FileTexture::m_outAlpha;

//----------------------------------------------------------------------------------------------------------------------
// Utility function
MStatus ConnectPlug(MDGModifier& dgMod, MFnDependencyNode& srcFn, MFnDependencyNode& destFn, MString plugName)
{
  MStatus status = MS::kSuccess;
  MPlug srcPlug = srcFn.findPlug(plugName,&status);
  MPlug destPlug = destFn.findPlug(plugName, &status);
  status = dgMod.connect(srcPlug, destPlug);
  return status;
}

//----------------------------------------------------------------------------------------------------------------------
MStatus FileTexture::initialize()
{
  MStatus status = MS::kSuccess;
  MNodeClass nodeClass("file");

  m_uvTilingMode = nodeClass.attribute("uvt", &status);
  m_fileTextureName = nodeClass.attribute("ftn", &status);
  m_computedFileTextureNamePattern = nodeClass.attribute("cfnp", &status);
  m_defaultColor = nodeClass.attribute("dc", &status);
  m_colorGain = nodeClass.attribute("cg", &status);
  m_alphaGain = nodeClass.attribute("ag", &status);
  m_colorOffset = nodeClass.attribute("co", &status);
  m_alphaOffset = nodeClass.attribute("ao", &status);
  m_outColor = nodeClass.attribute("oc", &status);
  m_outAlpha = nodeClass.attribute("oa", &status);
  return status;
}

//----------------------------------------------------------------------------------------------------------------------
MStatus FileTexture::import(const UsdPrim& prim, MObject& parent, MObject& createdObj)
{
  MStatus status = MS::kSuccess;

  TfToken id;
  UsdShadeShader shaderNode(prim);
  if(shaderNode.GetIdAttr().Get(&id))
  {
    if (id == UsdImagingTokens->UsdPrimvarReader_float2)   //Do not handle UsdPrimvarReader_float2, as I will create place2dTexture on the file node
      return status;
    else if(id == UsdImagingTokens->UsdUVTexture)
   {
      MFnDependencyNode fileNodeFn;
      createdObj = fileNodeFn.create("file", MString(std::string(prim.GetName()).c_str()), &status);
      status = updateMayaAttributes(createdObj, prim);

      TranslatorContextPtr ctx = context();
      if(ctx)
      {
        ctx->insertItem(prim, createdObj);
      }

      // Connect place2dTexture node to file node
      MFnDependencyNode place2dFn;
      place2dFn.create("place2dTexture");
      MDGModifier mod;
      MPlug uvCoordPlug = fileNodeFn.findPlug("uvCoord", &status);
      MPlug outUVPlug = place2dFn.findPlug("outUV", &status);
      status = mod.connect(outUVPlug, uvCoordPlug);

      ConnectPlug(mod, place2dFn, fileNodeFn, "fs");
      ConnectPlug(mod, place2dFn, fileNodeFn, "vc1");
      ConnectPlug(mod, place2dFn, fileNodeFn, "vt1");
      ConnectPlug(mod, place2dFn, fileNodeFn, "vt2");
      ConnectPlug(mod, place2dFn, fileNodeFn, "vt3");
      ConnectPlug(mod, place2dFn, fileNodeFn, "coverage");
      ConnectPlug(mod, place2dFn, fileNodeFn, "mu");
      ConnectPlug(mod, place2dFn, fileNodeFn, "mv");
      ConnectPlug(mod, place2dFn, fileNodeFn, "noiseUV");
      ConnectPlug(mod, place2dFn, fileNodeFn, "offset");
      ConnectPlug(mod, place2dFn, fileNodeFn, "repeatUV");
      ConnectPlug(mod, place2dFn, fileNodeFn, "rotateFrame");
      ConnectPlug(mod, place2dFn, fileNodeFn, "rotateUV");
      ConnectPlug(mod, place2dFn, fileNodeFn, "stagger");
      ConnectPlug(mod, place2dFn, fileNodeFn, "translateFrame");
      ConnectPlug(mod, place2dFn, fileNodeFn, "wrapU");
      ConnectPlug(mod, place2dFn, fileNodeFn, "wrapV");

      status = mod.doIt();
    }
  }
  return status;
}

//----------------------------------------------------------------------------------------------------------------------
MStatus FileTexture::updateMayaAttributes(MObject to, const UsdPrim& prim)
{
  MStatus status = MS::kSuccess;
  UsdShadeShader fileShader(prim);

  int tilingMode = 0;
  VtValue defaultVal;
  std::string filePath;

  const char* const errorString = "FileTextureTranslator: error setting file node parameters";
  if(UsdAttribute tileAttr = prim.GetAttribute(TfToken("maya_uvTilingMode")))
  {
    tileAttr.Get(&tilingMode);
    if(auto fileAttr = fileShader.GetInput(TfToken("file")))
    {
      fileAttr.Get(&defaultVal);
      const SdfAssetPath defaultPath = defaultVal.Get<SdfAssetPath>();
      filePath = defaultPath.GetAssetPath();
    }


    if(tilingMode == 0)
      AL_MAYA_CHECK_ERROR(DgNodeTranslator::setString(to, m_fileTextureName, filePath), errorString);
    if(tilingMode == 3)
      AL_MAYA_CHECK_ERROR(DgNodeTranslator::setString(to, m_computedFileTextureNamePattern, filePath), errorString);
    AL_MAYA_CHECK_ERROR(DgNodeTranslator::setInt64(to, m_uvTilingMode, tilingMode), errorString);
  }

  auto fallbackAttr = fileShader.GetInput(TfToken("fallback"));
  VtValue fallback;
  fallbackAttr.Get(&fallback);
  const GfVec4f fallbackVal = fallback.Get<GfVec4f>();
  AL_MAYA_CHECK_ERROR(DgNodeTranslator::setVec3(to, m_defaultColor, fallbackVal[0], fallbackVal[1], fallbackVal[2]), errorString);

  auto scaleAttr = fileShader.GetInput(TfToken("scale"));
  VtValue scale;
  scaleAttr.Get(&scale);
  const GfVec4f scaleVal = scale.Get<GfVec4f>();
  AL_MAYA_CHECK_ERROR(DgNodeTranslator::setVec3(to, m_colorGain, scaleVal[0], scaleVal[1], scaleVal[2]), errorString);
  AL_MAYA_CHECK_ERROR(DgNodeTranslator::setFloat(to, m_alphaGain, scaleVal[3]), errorString);

  auto biasAttr = fileShader.GetInput(TfToken("bias"));
  VtValue bias;
  biasAttr.Get(&bias);
  const GfVec4f biasVal = bias.Get<GfVec4f>();
  AL_MAYA_CHECK_ERROR(DgNodeTranslator::setVec3(to, m_colorOffset, biasVal[0], biasVal[1], biasVal[2]), errorString);
  AL_MAYA_CHECK_ERROR(DgNodeTranslator::setFloat(to, m_alphaOffset, biasVal[3]), errorString);

  return status;
}

//----------------------------------------------------------------------------------------------------------------------
UsdPrim FileTexture::exportObject(UsdStageRefPtr stage, MObject obj, const SdfPath& usdPath,
                                  const ExporterParams& params)
{
  MStatus status = MS::kSuccess;
  MFnDependencyNode depFn(obj, &status);

  const char* const errorString = "FileTextureTranslator: error getting maya file texture node parameters";

  int uvTilingMode = 0;
  std::string fileTextureName;
  std::string computedUDIMTextureName;
  float defaultColor[3] = {0.0, 0.0, 0.0};
  float colorGain[3] = {0.0, 0.0, 0.0};
  float alphaGain = 0.0;
  float colorOffset[3] = {0.0, 0.0, 0.0};
  float alphaOffset = 0.0;
  float outColor[3] = {0.0, 0.0, 0.0};
  float outAlpha = 0.0;

  AL_MAYA_CHECK_ERROR2(AL::usdmaya::utils::DgNodeHelper::getInt32(obj, m_uvTilingMode, uvTilingMode), errorString);
  AL_MAYA_CHECK_ERROR2(AL::usdmaya::utils::DgNodeHelper::getString(obj, m_fileTextureName, fileTextureName), errorString);
  AL_MAYA_CHECK_ERROR2(AL::usdmaya::utils::DgNodeHelper::getString(obj, m_computedFileTextureNamePattern, computedUDIMTextureName), errorString);
  AL_MAYA_CHECK_ERROR2(AL::usdmaya::utils::DgNodeHelper::getVec3(obj, m_defaultColor, defaultColor), errorString);
  AL_MAYA_CHECK_ERROR2(AL::usdmaya::utils::DgNodeHelper::getVec3(obj, m_colorGain, colorGain), errorString);
  AL_MAYA_CHECK_ERROR2(AL::usdmaya::utils::DgNodeHelper::getFloat(obj, m_alphaGain, alphaGain), errorString);
  AL_MAYA_CHECK_ERROR2(AL::usdmaya::utils::DgNodeHelper::getVec3(obj, m_colorOffset, colorOffset), errorString);
  AL_MAYA_CHECK_ERROR2(AL::usdmaya::utils::DgNodeHelper::getFloat(obj, m_alphaOffset, alphaOffset), errorString);
  AL_MAYA_CHECK_ERROR2(AL::usdmaya::utils::DgNodeHelper::getVec3(obj, m_outColor, outColor), errorString);
  AL_MAYA_CHECK_ERROR2(AL::usdmaya::utils::DgNodeHelper::getFloat(obj, m_outAlpha, outAlpha), errorString);

  // Create UsdUVTexture prim at root layer
  std::string primName;
  if (uvTilingMode == 3)
      primName = computedUDIMTextureName;
  else
      primName = fileTextureName;
  std::string textPath = primName;
  primName = primName.substr(primName.find_last_of("/")+1, std::string::npos);
  primName = primName.substr(0, primName.find_first_of("."));
  SdfPath uvTexturePath(usdPath.GetString() + "/" + primName);

  UsdShadeShader uvTextureShader = UsdShadeShader::Define(stage, uvTexturePath);
  if (!uvTextureShader)
      TF_DEBUG(ALUSDMAYA_TRANSLATORS).Msg("FileTexture Translator: UV texture node was not created successfully!");

  uvTextureShader.CreateIdAttr(VtValue(UsdImagingTokens->UsdUVTexture));

  uvTextureShader.CreateInput(TfToken("file"), SdfValueTypeNames->Asset).Set(SdfAssetPath(textPath));  //Do not consider Mudbox or other mode
  uvTextureShader.GetPrim().CreateAttribute(TfToken("maya_uvTilingMode"), SdfValueTypeNames->Int).Set(uvTilingMode);

  UsdShadeInput uvInput = uvTextureShader.CreateInput(TfToken("st"), SdfValueTypeNames->Float2);
  uvTextureShader.CreateInput(TfToken("wrapS"), SdfValueTypeNames->Token).Set(TfToken("useMetadata"));
  uvTextureShader.CreateInput(TfToken("wrapT"), SdfValueTypeNames->Token).Set(TfToken("useMetadata"));
  uvTextureShader.CreateInput(TfToken("fallback"), SdfValueTypeNames->Color4f).Set(GfVec4f(defaultColor[0], defaultColor[1], defaultColor[2], 1.0));
  uvTextureShader.CreateInput(TfToken("scale"), SdfValueTypeNames->Float4).Set(GfVec4f(colorGain[0], colorGain[1], colorGain[2], alphaGain));
  uvTextureShader.CreateInput(TfToken("bias"),SdfValueTypeNames->Float4).Set(GfVec4f(colorOffset[0], colorOffset[1], colorOffset[2], alphaOffset));
  UsdShadeOutput test1 = uvTextureShader.CreateOutput(TfToken("rgb"), SdfValueTypeNames->Float3);
  UsdShadeOutput test2 = uvTextureShader.CreateOutput(TfToken("a"), SdfValueTypeNames->Float);

  // If this node is actually connecting to bump node, set up "scale" and "bias" value so that
  // it can directly connect to UsdPreviewSurface
  MPlug outColorPlug = depFn.findPlug("outColor");
  MPlug outAlphaPlug = depFn.findPlug("outAlpha");
  MPlugArray colorConnectedPlugs;
  MPlugArray alphaConnectedPlugs;
  outColorPlug.connectedTo(colorConnectedPlugs, false, true, &status);
  outAlphaPlug.connectedTo(alphaConnectedPlugs, false, true, &status);
  if(colorConnectedPlugs.length() > 0 && colorConnectedPlugs[0].name() == MString("input")
     || (alphaConnectedPlugs.length() >0 && alphaConnectedPlugs[0].name() == MString("bumpValue"))) //This node is connected to aiNormalMap or a maya bump2d node
    uvTextureShader.GetInput(TfToken("scale")).Set(GfVec4f(2.0, 2.0, 2.0, 2.0));
    uvTextureShader.GetInput(TfToken("bias")).Set(GfVec4f(-1.0, -1.0, -1.0, -1.0));

  // Handle the place2dtexture node
  UsdPrim primvarReader = exportPlace2dTexture(stage, obj, usdPath, params);
  uvInput.ConnectToSource(UsdShadeShader(primvarReader).GetOutput(TfToken("result")));


  return uvTextureShader.GetPrim();
}

//----------------------------------------------------------------------------------------------------------------------
UsdPrim FileTexture::exportPlace2dTexture(UsdStageRefPtr stage, MObject obj, const SdfPath& usdPath,
                                          const ExporterParams& params)
{
  MStatus status = MS::kSuccess;
  UsdShadeShader primvarReader;
  MFnDependencyNode fileNodeFn(obj, &status);
  MPlug uvPlug = fileNodeFn.findPlug("uvCoord", &status);
  MPlugArray srcPlugs;
  uvPlug.connectedTo(srcPlugs, true, false, &status);

  if(srcPlugs.length() > 0)
  {
    MFnDependencyNode place2dFn(srcPlugs[0].node(), &status);
    SdfPath primvarReaderPath(usdPath.GetString() + "/" + place2dFn.name(&status).asChar());
    primvarReader = UsdShadeShader::Define(stage, primvarReaderPath);
    if (!primvarReader)
        TF_DEBUG(ALUSDMAYA_TRANSLATORS).Msg("FileTexture Translator: Primvar reader shader was not created successfully!");

    primvarReader.CreateIdAttr(VtValue(UsdImagingTokens->UsdPrimvarReader_float2));
    primvarReader.CreateInput(TfToken("varname"), SdfValueTypeNames->Token).Set(TfToken("st"));
    primvarReader.CreateInput(TfToken("fallback"), SdfValueTypeNames->Float2).Set(GfVec2f(0.0, 0.0));
    primvarReader.CreateOutput(TfToken("result"), SdfValueTypeNames->Float2);
  }
  return primvarReader.GetPrim();
}

//----------------------------------------------------------------------------------------------------------------------
MStatus FileTexture::tearDown(const SdfPath& path)
{
  MStatus status = MS::kSuccess;
  MObjectHandle obj;
  context()->getMObject(path, obj, MFn::kFileTexture);  //TODO: need to double check

  context()->removeItems(path);
  return status;
}

//----------------------------------------------------------------------------------------------------------------------
} // translators
} // fileio
} // usdmaya
} // AL
//----------------------------------------------------------------------------------------------------------------------

