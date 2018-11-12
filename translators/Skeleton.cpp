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

#include "Skeleton.h"

#include "pxr/usd/usdSkel/skeleton.h"

namespace AL {
namespace usdmaya {
namespace fileio {
namespace translators {

AL_USDMAYA_DEFINE_TRANSLATOR(Skeleton, pxr::UsdSkelSkeleton)

//----------------------------------------------------------------------------------------------------------------------
MStatus Skeleton::initialize()
{
    MStatus status = MS::kSuccess;
    return status;
}

//----------------------------------------------------------------------------------------------------------------------
MStatus Skeleton::update(const UsdPrim& prim)
{
  MStatus status = MS::kSuccess;
  return status;
}

//----------------------------------------------------------------------------------------------------------------------
MStatus Skeleton::tearDown(const SdfPath& path)
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
