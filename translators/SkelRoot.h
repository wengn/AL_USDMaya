 
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
#include "AL/usd/utils/ForwardDeclares.h"

namespace AL {
namespace usdmaya {
namespace fileio {
namespace translators {

//----------------------------------------------------------------------------------------------------------------------
/// \brief Class to translate UsdSkelRoot in and out of maya.
/// The whole purpose of the a SkelRoot translator is to create a parent transform and have it recorded in the system
/// so that UsdSkelSkeleton can find an appropriate parent node
//----------------------------------------------------------------------------------------------------------------------
class SkelRoot
  : public TranslatorBase
{
public:

  AL_USDMAYA_DECLARE_TRANSLATOR(SkelRoot);

  MStatus initialize() override;
  MStatus import(const UsdPrim& prim, MObject& parent, MObject& createObj) override;

  MStatus tearDown(const SdfPath& path) override;
  MStatus update(const UsdPrim& prim) override;

private:

};

} // translators
} // fileio
} // usdmaya
} // AL
//-----------------------------------------------------------------------------

