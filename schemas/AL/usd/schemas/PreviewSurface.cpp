#include "./PreviewSurface.h"


#include "pxr/usd/usd/schemaRegistry.h"
#include "pxr/usd/usd/typed.h"

#include "pxr/usd/sdf/types.h"
#include "pxr/usd/sdf/assetPath.h"

PXR_NAMESPACE_OPEN_SCOPE

// Register the schema with the TfType system.
TF_REGISTRY_FUNCTION(TfType)
{
    TfType::Define<OF_USD_PreviewSurface,
        TfType::Bases< UsdShadeShader > >();

    TfType::AddAlias<UsdSchemaBase, OF_USD_PreviewSurface>("OFPreviewSurface");
}

/* virtual */
OF_USD_PreviewSurface::~OF_USD_PreviewSurface()
{
}

/* virtual */
UsdSchemaType OF_USD_PreviewSurface::_GetSchemaType() const {
    return OF_USD_PreviewSurface::schemaType;
}

/* virtual */
const TfType &
OF_USD_PreviewSurface::_GetTfType() const
{
    return _GetStaticTfType();
}

/* static */
const TfType &
OF_USD_PreviewSurface::_GetStaticTfType()
{
    static TfType tfType = TfType::Find<OF_USD_PreviewSurface>();
    return tfType;
}

PXR_NAMESPACE_CLOSE_SCOPE
