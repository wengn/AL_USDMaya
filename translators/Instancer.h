 
//
// Copyright 2018 Animal Logic
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

#pragma once

#include "AL/usdmaya/fileio/translators/TranslatorBase.h"



namespace AL {
namespace usdmaya {
namespace fileio {
namespace translators {

//----------------------------------------------------------------------------------------------------------------------
/// \brief Class to translate a directional light in and out of maya.
//----------------------------------------------------------------------------------------------------------------------
class Instancer
  : public TranslatorBase
{
public:

  AL_USDMAYA_DECLARE_TRANSLATOR(Instancer);

  MStatus initialize() override;
  MStatus import(const UsdPrim& prim, MObject& parent, MObject& createObj) override;

  MStatus postImport(const UsdPrim& prim) override;
  UsdPrim exportObject(UsdStageRefPtr stage, MDagPath dagPath, const SdfPath& usdPath,
                       const ExporterParams& params) override;
  MStatus preTearDown(UsdPrim& prim) override;
  MStatus tearDown(const SdfPath &path) override;
  MStatus update(const UsdPrim& prim) override;

  bool supportsUpdate() const override
    { return true; }
  ExportFlag canExport(const MObject &obj) override
    { return obj.hasFn(MFn::kInstancer) ? ExportFlag::kFallbackSupport : ExportFlag::kNotSupported; }

  bool needsTransformParent() const
    { return false; }

private:
  void setMayaInstancerArrayAttr(MFnArrayAttrsData& inputPointsData, const UsdAttribute& usdAttr, MString attrName);
  MStatus updateMayaAttributes(MObject mayaObj, const UsdPrim& prim);
  bool setupParticleNode(MObject mayaObj, const UsdPrim& prim);
  bool createParticleData(const UsdPrim& prim, MPlug& pointsDataPlug);
  bool updateUsdPrim(UsdStageRefPtr stage, const SdfPath& usdPath, const MObject& obj);
  bool setUSDInstancerArrayAttribute(UsdStageRefPtr stage,MFnArrayAttrsData& inputPointsData,
                                     const SdfPath& usdPath, const size_t numPrototypes, UsdTimeCode usdTime);
  static MObject m_inputPoints;
  static MObject m_inputHierarchy;
  static MObject m_instanceData;
  static MObject m_matrix;
  static MObject m_visibility;

};

} // translators
} // fileio
} // usdmaya
} // AL
//-----------------------------------------------------------------------------
