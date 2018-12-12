//
// Copyright 2018 Original Force
//
// -- Author: Naiqi Weng
// This was a test schema class OF_USD_PreviewSurface, for
// creating corresponding prims for arnold surface shader
// node. It didn't work correctly. For now, just use AIPreview class
// for importing/exporting UsdshadeMaterial and its shader node
// and use FileTexture class for importing/exporting UsdUVTexture node.

#ifndef AL_USDMAYASCHEMAS_GENERATED_PREVIEWSURFACE_H
#define AL_USDMAYASCHEMAS_GENERATED_PREVIEWSURFACE_H

#include "./api.h"
#include "pxr/pxr.h"
#include "pxr/usd/usd/typed.h"
#include "pxr/usd/usd/prim.h"
#include "pxr/usd/usd/stage.h"
#include "pxr/usd/usdShade/shader.h"
#include "./tokens.h"

#include "pxr/base/vt/value.h"

#include "pxr/base/gf/vec3d.h"
#include "pxr/base/gf/vec3f.h"
#include "pxr/base/gf/matrix4d.h"

#include "pxr/base/tf/token.h"
#include "pxr/base/tf/type.h"

PXR_NAMESPACE_OPEN_SCOPE


class OF_USD_PreviewSurface : public UsdShadeShader
{
public:

    static const UsdSchemaType schemaType = UsdSchemaType::ConcreteTyped;

    explicit OF_USD_PreviewSurface(const UsdPrim& prim=UsdPrim())
        : UsdShadeShader(prim)
    {
    }


    explicit OF_USD_PreviewSurface(const UsdSchemaBase& schemaObj)
        : UsdShadeShader(schemaObj)
    {
    }

    /// Destructor.
    AL_USDMAYASCHEMAS_API
    virtual ~OF_USD_PreviewSurface();

/*
    AL_USDMAYASCHEMAS_API
    static OF_USD_PreviewSurface
    Define(const UsdStagePtr &stage, const SdfPath &path);
*/
protected:
    /// Returns the type of schema this class belongs to.
    ///
    /// \sa UsdSchemaType
    AL_USDMAYASCHEMAS_API
    virtual UsdSchemaType _GetSchemaType() const;

private:
    // needs to invoke _GetStaticTfType.
    friend class UsdSchemaRegistry;
    AL_USDMAYASCHEMAS_API
    static const TfType &_GetStaticTfType();


    // override SchemaBase virtuals.
    AL_USDMAYASCHEMAS_API
    virtual const TfType &_GetTfType() const;
};

PXR_NAMESPACE_CLOSE_SCOPE

#endif
