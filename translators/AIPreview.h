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
/// \brief Class to translate arnorld attributes in and out of maya.
//----------------------------------------------------------------------------------------------------------------------
class AIPreview
  : public TranslatorBase
{
public:

  AL_USDMAYA_DECLARE_TRANSLATOR(AIPreview);

  MStatus initialize() override;
  MStatus import(const UsdPrim& prim, MObject& parent, MObject& createdObj) override;
  UsdPrim exportObject(UsdStageRefPtr stage, MObject obj, const SdfPath& usdPath,
                       const ExporterParams& params) override;
  MStatus tearDown(const SdfPath& path) override;

  ExportFlag canExport(const MObject& obj) override;
  MStatus updateMayaAttributes(MObject to, UsdShadeShader& usdShader);

private:

};

//----------------------------------------------------------------------------------------------------------------------
} // translators
} // fileio
} // usdmaya
} // AL
//----------------------------------------------------------------------------------------------------------------------
