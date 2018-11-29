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
#pragma once

#include "AL/usdmaya/fileio/translators/TranslatorBase.h"
#include "AL/usd/utils/ForwardDeclares.h"

#include "pxr/usd/usd/stage.h"
#include "pxr/usd/usdShade/shader.h"

namespace AL {
namespace usdmaya {
namespace fileio {
namespace translators {

//----------------------------------------------------------------------------------------------------------------------
/// \brief Class to translate file texture shadng node attributes in and out of maya.
//----------------------------------------------------------------------------------------------------------------------
class FileTexture
  : public TranslatorBase
{
public:

  AL_USDMAYA_DECLARE_TRANSLATOR(FileTexture);

  MStatus initialize() override;
  MStatus import(const UsdPrim& prim, MObject& parent, MObject& createdObj) override;
  UsdPrim exportObject(UsdStageRefPtr stage, MObject obj, const SdfPath& usdPath,
                       const ExporterParams& params) override;
  UsdPrim exportPlace2dTexture(UsdStageRefPtr stage, MObject obj, const SdfPath& usdPath,
                       const ExporterParams& params);
  MStatus tearDown(const SdfPath& path) override;

  ExportFlag canExport(const MObject& obj) override
  {return obj.hasFn(MFn::kFileTexture) ? ExportFlag::kFallbackSupport : ExportFlag::kNotSupported; }

  MStatus updateMayaAttributes(MObject to, const UsdPrim& prim);
  bool needsTransformParent() const override
    { return false; }

private:
  static MObject m_uvTilingMode;
  static MObject m_fileTextureName;
  static MObject m_computedFileTextureNamePattern;
  static MObject m_defaultColor;
  static MObject m_colorGain;
  static MObject m_alphaGain;
  static MObject m_colorOffset;
  static MObject m_alphaOffset;
  static MObject m_outColor;
  static MObject m_outAlpha;
};

//----------------------------------------------------------------------------------------------------------------------
} // translators
} // fileio
} // usdmaya
} // AL
//----------------------------------------------------------------------------------------------------------------------
