## v0.29.1 (2018-08-28)

### Added

* Sub-sample animation export

### Changed

- Only perform UsdImagingGLHdEngine creation when it is Maya interactive mode.
- Issue error in console when some nodes cannot be created in the transform chain, at which case usdMaya will go and parent the chain created right under the proxy transform.
* Built against USD-0.18.9
* Plugins' metadata (`plugInfo.json`) have been moved to **lib/usd** to reflect USD's new layout.
* The initial edit target of proxyShapes has changed from the root layer to the session layer (#102 - @nxkb)
* The mapping from prim to maya dag path is no longer stored on the session layer, but is stored on the proxyShape class (#102 - @nxkb)
* Made translatorProxyShape into a completely separate plugin, AL_USDMayaPxrTranslators (#109 - @elrond79)
* This sandboxes the main AL_USDMayaPlugin from needing to link against any pixar maya libraries (ie, usdMaya) (#109 - @elrond79)
* Also added a cmake option to disable building of the plugin entirely, BUILD_USDMAYA_PXR_TRANSLATORS (#109 - @elrond79)
* Updated FindUSD.cmake to enable finding of usdMaya (#109 - @elrond79)

### Fixed

* Fix for opensource tests
* Cameras not exported on file -> export
* Win32 build error
* Fixed some runtime linkage problems with the main AL_USDMayaPlugin.so and usdMaya.so (#109 - @elrond79)
* Fixed warning about signed/unsigned comparsion (#110 - @elrond79)
* Fixed warning about unused errorString (#110 - @elrond79)

## v0.29.0 (2018-08-06)

### Added

* struct MayaFnTypeId to uniquely identify Maya object types.
* TranslatorAbstract::supportedMayaTypes() returns a vector of Maya object types supported by this translator.
* TranslatorAbstract::claimMayaObjectExporting() provides a place for translator to confirm it wants to export this Maya object.
* TranslatorAbstract::exportObject() interface for translator to pull data from Maya object and fill into Usd prim.
* overloading TranslatorManufacture::get() returns all translator candidates for a Maya object type.
* UsdMaya translator for AL_usdmaya_ProxyShape (#99 - @nxkb)
* Support for files that are not on disk until they are resolved (#100 - @nxkb)
* Added new Pre/PostDestroyProxyShape events (#104 @elron79)

### Changed

* TranslatorAbstract::import(), a third parameter is added to pass out Maya object created in importing process. This interface will be used in both AL_usdmaya_Import and AL_usdmaya_ProxyShapeImport workflows and TranslatorContext need be checked before using.
* A third argument is added to macro AL_USDMAYA_DEFINE_TRANSLATOR. It must be an array of MayaFnTypeId which defines all Maya object types this translator might export.
* Selection(is stored preFileSave() in a MSelectionList, not storing AL_USD proxies (#78 - @wnxntn)
* Selection is restored postFileSave() (#78 - @wnxntn)

### Removed

* fileio/translators/MeshTranslator
* fileio/translators/NurbsCurveTranslator
* fileio/translators/CameraTranslator
* Removed some EXPORT macros from inline methods (no need to export)

### Fixed
* A number of compiler warnings fixed, so PXR_STRICT_BUILD_MODE may be used (#97 - @elron79)
* added missing triggers for Pre/PostStageLoaded events (#103 - @elron79)

## v0.28.5 (2018-07-10)

### Added

* New open-source ALFrameRange schema and translator, which sets up Maya time range and current frame during the translation.
* Tests for ALFrameRange translator.
* Tests for the fix for the over-decref issue in transform chain.

### Fixed

* Over-decref in transform chain during some prim tear-down.

## v0.28.4 (2018-07-05)

### Added

* New events to prevent USDGlimpse from rebuilding the scene when mapping meta data is inserted into the prims during selection.

### Changed

* Transform values that have not changed no longer get written back to USD.
* Only hook up the proxy shape time attribute to the transform nodes that are actually transforms (i.e. ignore prims that are effectively just organisational containers)
* Allowed named events to be able to have callbacks registered against them prior to the events being registered

### Removed

* Some export macros from inline methods

### Fixed

* Regression where transform values were no longer being correctly displayed when translating prims into maya.
* A number of build warnings in the OSS build
* Final two build warnings in the repo. The generated schema code needs to have a flag set in the build C4099 to hide the 'class defined as struct' warning (pixar issue, not ours)
* When you duplicate a proxy shape, you wont see anything in the viewport and it wont be fully initialize until you reopen maya or find a hack (like changing the usd path) to trigger a load stage. (#98 - @nxkb)
* Add missing newlines to some TF_DEBUG statements in ProxyShapeUI (#92 - @elrond79)

## v0.28.3 (2018-06-18)

### Added
+ AL_usdmaya_CreateUsdPrim command added to insert a new prim into the UsdStage of a proxy shape. 

### Changed
+ The proxyShape outStageData is now connectable, and now longer hidden. Allows for manual DG node connections to be made. 

### Fixed
+ The internal mapping between a maya object and a prim now works correctly when specifying the -name flag of AL_usdmaya_ProxyShapeImport. 

## v0.28.2 (2018-06-14)

### Added
+ Viewport will continue to refresh to show updated progressive renderers (like embree) until converged (#91 - @elrond79)

### Changed
+ make getRenderAttris update showRender, as well as showGuides (#87 - @elrond79)

### Fixed
+ Tweaks to allow builds to work with just rpath. (#88 - @elrond79)
+ When importing left handed geometry that has no normals, compute the correct set of inverted normals.

## v0.28.1 (2018-06-06)

### Added
* Documentation for mesh export, interpolation modes, and diffing.
* "-fd" flag added to the import command and the translate command
* Support for prim var interpolation modes in the import/export code paths (currently UV sets only, colour sets & normals will be in a later PR).
* Support for diffing all variations of the interpolation modes on prim vars.
* Routines to determine the interpolation modes on vec2/vec3/vec4 types.
-  AL_usdmaya_LayerManagerCommands to retrieve layers that have been modified and that have been the EditTarget.

### Changed
+ Built using USD-0.8.5
+ ProxyShapePostLoadProcess::createTranformChainsForSchemaPrims now checks all the prims' parents' metadata for their unmerged status. If found, the transform chain creation skips the current prim in place of its parent.
+ ProxyShapePostLoadProcess::createSchemaPrims now checks all the prims' parents' metadata for their unmerged status. If found, the schema prim import will instead take place under the parent node.
+ Mesh::import now checks the prim's parents' metadata for its unmerged status. If found, the created shape node will not have the appended "Shape" string.
* Mesh attributes queries use EarliestTime timecode
* Functions in DgNodeTranslator are moved to DgNodeHelper in AL_USDMayaUtils
* AttributeType.h/.cpp are moved to AL_USDMayaUtils
- LayerManager API has been updated so you can now inspect all layers that have been set as an EditTarget and are Dirty.
+ Updated endToEndMaya tutorial to work with latest USD API changes.
* All hard coded /tmp paths in the unit tests, in favour of a linux/win32 solution.

### Fixed

- Fix for when tearing down a shape, that it's sibling shapes are left alone in Maya.
* time1.outTime is now connected to proxy shape nodes when transforms are selected during creation.
* Selected transforms are no longer renamed when importing a proxy shape.
* Tests build restored.
- Fix for Issue "meshTransform cannot execute twice" #82
* Bug in the import/export of mesh normals
* Normals export now correctly listen to the exporter options.
* AL_usdmaya_plugin can be unloaded correctly.
* Passing the time value into the nurbs, camera, and transform translators on export.

### Removed
* Removed the code that reversed polygon windings if the 'leftHanded' meta data flag was encountered. Instead, the 'opposite' flag is set on the maya mesh.

## v0.27.10 (2018-05-07)

### Added

+ A new plugin translator (translator/NurbsCurve.cpp) to translate prim between USD and Maya, and write back editing result to USD prim.

### Changed

+ Existing static nurbs curve translator re-uses common utils code.

### Fixed

+ Windows build

## v0.27.8 (2018-05-02)

### Changed

+ ExportCommand will now export time samples for the visibility attribute.
+ ImportCommand will now correctly import time samples for the visibility attribute.

## v0.27.7 (2018-05-01)

### Added

+ Experimental Windows support. It has been tested on Windows 7 with Visual Studio 2015 and Maya 2017.

### Removed

+ Qt dependency (Thanks to Sebastien Dalgo from Autodesk)

## v0.27.6 (2018-04-13)

### Added

+ New attribute "assetResolverConfig" on proxy shape
* Export creates master geometry prim thus each instance could reference. If transform and mesh nodes are separated by turning off "mergeTransform" option, transform prim will be marked as "instanceable" and fully benefits from usd instancing feature.
* Import detects shared master geometry prim, ensures maya transform nodes parenting the instancing shape.
* Layer manager is now serialised on file export

## v0.27.5 (2018-04-10)

### Added

+ Add assetResolverConfig string attribute to ProxyShape 

### Changed

+ Built against USD-0.8.4
+ Move initialisation of maya event handler from AL_USDMaya library to mayaUtils library

## v0.27.4 (2018-04-04)

### Added

+ src/plugin/AL_USDMayaTestPlugin/AL/usdmaya/fileio/export_unmerged.cpp has been added, which tests the mergeTransforms parameter.

### Changed

+ src/schemas/AL/usd/schemas/plugInfo.json.in had the metadata "al_usdmaya_mergedTransform" added.
+ src/plugin/AL_USDMayaTestPlugin/AL/usdmaya/test_DiffPrimVar.cpp had the export parameter mergeTransforms changed to 1 to revert to previous test behaviour.
+ fileio/Import.cpp will now check for the mergedTransform metadata on the parent transform, which prevents mesh import from creating the parent transform.
+ fileio/Export.cpp has re-enabled functionality for "mergeTransforms", which will export the Xform prim and then the children mesh prims separately when set to '0'. The parent transform will have the metadata "al_usdmaya_mergedTransform = unmerged" added to tag it as an unmerged node.

## v0.27.3 (2018-03-29)

### Added

- Add -eac/-extensiveAnimationCheck option to AL_usdmaya_ExportCommand
- Add isAnimatedTransform() to AnimationTranslator to perform extensive animation check on transform nodes.

### Changed

- Updated Inactive/Active tests
- By default AL_usdmaya_ExportCommand will perform extensive animation check.
+ Changed the signature of StageCache::Get(bool forcePopulate) to remove forcePopulate as it doesn't exist in the USDUtilsCache. Also affects Python bindings

### Fixed

* Mesh surface normals were not importing correctly

## v0.27.2 (2018-03-27)

### Added

- Internal AL build fixes
- Small fix to nurb width import

## v0.27.1 (2018-03-23)

### Added

- Updated existing command to add layer to the RootLayers sublayer stack
- Support relative path as usd file path, which will be resolved using current maya file path. It does nothing if current maya file path is none.
- Renamed glimpse subdivision related tokens (from glimpse_name to glimpse:subdiv:name)
- Nurb Curve widths now are imported and exported from AL_USDMaya's ImportCommand
- rendererPlugin attribute to ProxyShape
- Mesh Translation tutorial

### Changed

- Built using USD-0.8.3
- Library name AL_Utils -> AL_EventSystem
- Use custom data to store maya_associatedReferenceNode instead of attribute on the prim when import maya reference.
- AL_usdmaya_ProxyShapeImportPrimPathAsMaya, removed the parameters which weren't used.
- Allow for custom pxr namespace - https://github.com/AnimalLogic/AL_USDMaya/pull/68

### Fixed

- "Error : No proxyShape specified/selected " when attempting to add a sublayer via the UI
- The MPLug::source()  error in Maya 2016, the API is only available since Maya 2016 ex2.
- Failing unit test
- AL_usdmaya_TranslatePrim now ignores translation to an already translated prim
- Crash fix for MeshTranslator crash when variant switching
- Fixes several issues with selection in the maya viewport - https://github.com/AnimalLogic/AL_USDMaya/pull/42

## v0.27.0 (2018-03-12)

### Added 
+ Mesh Translation: Add support for glimpse user data attributes during import / export
+ Added a Translate command "AL_usdmaya_TranslatePrim" that allows you to selectively run the Translator for a set of Prim Paths to either Import or Teardown the prim. Tutorial on the way! 
+ preTearDown writes Meshes Translated to Maya to EditTarget 
+ Library Refactor: Refactored to seperate code into multiple libraries: see change [DeveloperDocumentation](README.md#developer-documentation)


### Changed

+ Tests: TranslatorContext.TranslatorContext was failing because there was an extra insert into the context in the MayaReference import. Also there was a filter on the tests which needed to be nuked!
+ Docs: Small update to build.md
+ Mesh Translation: updated glimpse subdivision attribute handling
+ Store maya_associatedReferenceNode in custom data This is to stop USD from complaining about: "Error in 'pxrInternal_v0_8__pxrReserved__::UsdStage::_HandleLayersDidChange' at line 3355 in file stage.cpp : 'Detected usd threading violation.  Concurrent changes to layer(s) composed in stage 0x185f5a50 rooted at @usdfile.usda@.  (serial=6, lastSerial=12).'
+ Updated AL_usdmaya_LayerCreateLayer command to add layers to Sublayers
+ Proxy Shape and transform nodes names now match
+ Update to USD-0.8.3


### Known Bugs/Limitations:
+ Variant Switch with Maya Meshes causes a crash
+ AL_usdmaya_TranslatePrim generates multiple copies of meshes
+ AL_usdmaya_ProxyShapeImportPrimPathAsMaya not using new Mesh Translation functionality



## v0.26.1 (2018-03-02)

### Added

* Doxygen comments, tutorial and unit test for ModelAPI::GetLock(), ModelAPI::SetLock() and ModelAPI::ComputeLock().


### Changed

Change default lock behaviour of al_usdmaya_lock metadata. "transform" will lock current prim and all its children.
-GetSelectabilityValue method is now called GetSelectability
-Implementation of ComputeLock() is modified to reuse common code structure.
-Library name AL_Utils -> AL_EventSystem


### Fixed

- Issue with the ProxyShapeIimport command not loading payloads correctly.
- AL_USDMayaSchema's pixar plugin was not being loaded during it's tests
* Blocked the usd stage being reloaded when the filePath changes, when loading a maya file. This ensures all proxy shape attributes are initialised prior to reloading the stage.
- Crash when moving a locked prim
- Locking not working on maya hierarchies that appear and disappear on selection
- "Error : No proxyShape specified/selected " when attempting to add a sublayer via the UI

## v0.26.0 (2018-02-09)

### Added

* Overloads for the MPxTransform::isBounded() and MPxTransform::boundingBox() methods to provide the transforms with a bounding box to enable usage of frame selected
* ProxyShape::usdStage() to directly return the proxys shape stage without triggering a compute
* Extended Python bindings for ProxyShape class
* Flag to keep temporary test files generated by AL_USDMayaTestPlugin

### Changed

* Uses USD-0.8.2
* Reduced the overall number of pulls on the outStage data attribute
* Replaced the use of MMessage callbacks in favour of a scheduled system allowing control of maya events over a number of plugins and tools
* the -identifiers/-id flag is set.  This makes it more in line with all the other flags for the command (and more
  flexible), but is a change in behavior - previously, the muted layers would ALWAYS be returned as identifier
* Removed QT dependency in favour of Maya's keyboard getModifiers command
* Total revamp of how layers and edited layers are stored by AL_USDMaya

### Removed

* The insertFirst / insertLast concept in the maya events system

### Fixed

* Incorrect spot cone angles were being passed to Hydra in VP2
* Animated attributes could end up being added to the export list multiple times
* Ik chains needed to be inspected to ensure the animated values were correctly exported
* Animation caused via geometry constraints would not be exported
* regression that printed out various malformed sdfpath errors in unit tests
* Add AL pkgutil fallback for environments without pkg_resources
* ProxyShapeImport command now correctly takes `ul` parameter to direct if it should load payloads or not
* Bug where it was possible to select a transform outside the proxyshape using the 'up' key
* Bug with CommandGuiHelper where if it has errors building it would stay hidden, it now deletes and rebuilds itself
* Bug where addListOption would error if any list items had any characters that need to be quoted 
* Crash when a prim is selected multiple times via command
* Crash if set xform values on an invalid prim
* Crash when pushToPrim is triggered but the prim is invalid
* Crash fix for when switching back + forth between a usd-camera and a maya-referenced camera

## v0.25.0 (2017-11-28)

### Added

* New metadata and logic to lock transform and disable pushToPrim.
* A boolean option "meshUV" or "muv" in AL_usdmaya_ExportCommand to indicate only export mesh uv data to a scene hierarchy with empty over prims.
* A boolean option "leftHandedUV" or "luv" in AL_usdmaya_ExportCommand to indicate whether to adjust uv indices orientation. This option only works with "meshUV"

### Changed

* Add special rules for some Maya-USD attribute type mismatches so the maya attribute values transfer correctly to USD prim, e.g. interpolateBoundary attribute on mesh.
* When translate, rotate, scale attributes are locked, TransformationMatrix blocks any changes of these attributes.
* In mesh import, leftHanded flag retrieved from USD will also decide if uv indices need be adjusted.

### Removed

* Host Driven Transforms

## v0.24.5 (2017-11-06)

### Fixed

* Remove leftover file

## v0.24.4 (2017-10-18)

### Changed

* Update to USD-0.8.1

## v0.24.3 (2017-10-18)

### Fixed

In same cases when there was geometry in VP2 behind some maya objects, some maya objects wouldn't be selected

## v0.24.2 (2017-10-17)

### Fixed

* Fix for [#24](https://github.com/AnimalLogic/AL_USDMaya/issues/24). This is really to avoid poping objects while selecting and does not represent a long term solution.
Selecting maya nodes that were outside of the proxyshape were causing a "Root path must be absolute (<>) " error.

## v0.24.1 (2017-10-10)

### Added

SelectionDB that stores all the paths that are selectable.

### Changed

Move all the selection commands into a different cpp file.

## v0.24.0 (2017-10-09)

### Fixed

* Update test_PrimFilter to not rely on external files

## v0.23.9 (2017-09-20)
### Added
* Added missing header files from nodes/proxy directory into released package
* AL_usdmaya_UsdDebugCommand to allow you to modify and query the TfDebug notifications
* Added a new Debug GUI that lets you enable/disable which TfDebug notices are output to the command prompt
* ProxyShape now calls ConfigureResolverForAsset with the USD file path that is being open, which is similar to what usdview does.

## v0.23.7 (2017-09-15)
### Fixed
* bug where enabling/disabling certain prims could cause an infinite loop.

## v0.23.6 (2017-09-12)
### Fixed
+ Bug where if the variant selection is called twice before it gets processed

## v0.23.5
* Prevent viewport refresh at end of animation export that can cause a VP2 crash.

## v0.23.2
* Consolidated debug traces to use TF_DEBUG, see developers documentation for available flags.

## v0.23.1
* Bug Fix: Maya 2016 build.

## v0.22.1
* Bug Fix: Fixed playblast issue that caused drift in caches when using animated film offsets on the camera

## v0.22.0
* Bug Fix: Fixed crash in prim resync command when passed an invalid prim path
* Removed unused pxr directory
* Bug Fix: Schemas generation
* Bug Fix: the maya reference update code was double-loading references when switching paths.

## v0.21.0
* Removed legacy TRA Array attributes from the proxy shape
* Bug Fix: Fixed issue that could cause translator plugins to be uninitialised when proxy shape was first created
* Updated to USD 0.8.0
* Various cmake/build improvements

## v0.20.1
* Improvements to colour set export on mesh geometry
* Bug Fix: Force parallel evaluation when running unit tests
* Removed unused asset resolver config
* Bug Fix: Improve selection between maya geometry and usd geometry

## v0.20.0
* Hooked up display Guides + displayRenderGuides attributes to usdImaging
* Added MEL command to provide a simple selection mechanism in USD imaging layer
* Reduced the number of times the translators were being initialized to once per proxy shape instance. 
* Added ability for the translators to say they support an inactive state (so nodes don't get deleted)
* Added reference counting for kRequired transforms. 
* Bug Fix: If a prim changed type as a result of a variant switch, the type info was not updated in the translator context
* Bug Fix: Shapes, Transforms, and DG nodes now correctly deleted (without leaving transforms)
* Bug Fix: Fixed issue where we could end up with invalid transform references in some very rare edge cases
* Bug Fix: Fixed issue where invalid MObjects could be generated within transform reference generation. 
* Bug Fix: Switching from a plugin prim type, to an ignored cache prim, could leave transforms in the scene
* Added the AL_usdmaya_ProxyShapeSelect command to select prims via a command (supports undo/redo). 
* Promoted the CameraTranslator to a properly implemented translator plugin

## v0.19.0
* Added Open Source Licencing
* Documentation Refactor
* Addded End-to-End Tutorial
* Added MayaReference and HostDrivenTransformInfo schema (_the opensource repository is now deprecated_)
* Added MayaReference translator (_the opensource repository is now deprecated_)

## v0.18.1
**Change Log**
* Bug Fix: Fixed issue with custom transform types not being correctly serialised on file Save
* Bug Fix: Fixed issue with transforms with identical names not being correctly deserialised
* Added support for the Intel F16C half-float conversion intrinsics

## v0.18.0
**Change Log**
* AL_usdmaya_LayerCurrentEditTarget command can now take a layer identifier to specify the target layer
* New command AL_usdmaya_LayerCreateLayer added 
* Updated docker config
* Bug Fix: Unchecked pointer access in SchemaNodeRefDB::removeEntries
* proxyShapeImport GUI now displays all usd file types
* Bug Fix: Colour sets now correctly applied on import
* Bug Fix: Maya 2018 compatibility changes

## v0.17.0
**Change Log**
* Built against usd-0.7.5
* Switching StageData to use a UsdStageWeakPtr to avoid keeping some stages alive forever.
* Copy the edit target instead and re-use it when setting it back.
* Add CHANGELOG.md
* Bug Fix: Fixes to make the unit tests run in mayabatch mode.
* Bug Fix: Use shared_ptr to manage driven transform data life time. [#263]
* Bug Fix: Fixing a possible crash with the curve importer.
* Bug Fix: Avoiding a crash when importing a camera.

## v0.16.9
**Change Log**
* Bug Fix: Previous selection crash-fix, that re-parented custom transforms under a temporary, would cause a change in the 
           selection list, which resulted in a crash. 
* Bug Fix: Fixed crash in removeUsdTransforms as a result of Maya deleting the parent of a custom transform
* Bug Fix: Hydra selection highlighting was being overwritten when performing Shift+Select
* The proxy shape now responds to changes of the Active state of plugin translator prims
* Maya 2016 now supported
 
## v0.16.8
**Change Log**
* Bug Fix: fixing a regression that would cause the transform hierarchy to be incorrect
 
## v0.16.7
**Change Log**
* Changes to the driven transforms on the proxy shape
* Ported from CPP unit to googletest, and moved all tests into a test plugin.
* Code now compiles against Maya 2018 beta77
* Bug Fix: session layer handle was incorrectly being wiped after a file load, which could cause a crash
* Bug Fix: Layer::getSubLayers would fail after the scene was reloaded. 
* Bug Fix: Incorrectly warned of storable message attributes
* Bug Fix: Unitialised layer handles could cause a crash
* Bug Fix: Deleting AL_usdmaya_Transform nodes would cause parent transforms to be deleted. 
* Bug Fix: Layer::getParentLayer returning invalid value
* Bug Fix: getAttr "layerNode.framePrecision" now works as expected
* Bug Fix: Chaning the USD edit target now correctly updates the 'hasBeenEditTarget' flag
* Bug Fix: Animated Shear 
* Bug Fix: AL_usdmaya_TransformationMatrix::getTimeCode now correctly returns the animated time values
* Bug Fix: AL_usdmaya_TransformationMatrix::enablePushToPrim would incorrectly create a scalePivot transformation op
* Bug Fix: AL_usdmaya_TransformationMatrix could fail to update if frame 0 was the first animation frame in a sequence. 
* Bug Fix: Selecting a parent of a selected transform, would cause a crash in Maya. 

## v0.16.6
**Change Log**
* Bug Fix: Matrix driven transform node could write an invalid key into the session layer, nuking animation cache data. 
* Bug Fix: Excluded geometry became visible on reload. 
* Bug Fix: Removed option box from Import Proxy Shape (was causing a crash).
* Improvement: Proxy Shape now runs the post-load process immediately, rather than waiting on a defferred MEL call. 

## v0.16.5
**Change Log**
* Ability to set Edit Targets with a map function
* Bug Fix: Edit Targets not correctly preserved during selection changes
* Bug Fix: Export command strips namespaces to avoid crash
* Bug Fix: Camera transforms now correctly animate
* readFromTimeline attribute added to AL_usdmaya_Transform to control when transforms display custom or animated values.

## v0.16.4
**Change Log**
* Bug Fix: playblasts were coming out black
* Bug Fix: OpenGL state not preserved in VP1

## v0.16.3
**Change Log**
* Bug Fix: excluded objects not hidden after file load
* Bug Fix: Prevented crash within draw override. 

## v0.16.2
**Change Log**
* Bug Fix: excluded objects not hidden after variant switch

## v0.16.1
**Change Log**
* Bug Fix: Fixed incorrect depth settings when rendering in Hydra
* Bug Fix: Fixed complexity issue that caused warnings to be spammed into the command prompt. 

## v0.16.0
**Change Log**
* Switched code over to use UsdImaging rather than UsdMayaGL library
* Updated USD base to 0.7.4
* Selection highlighting now visible in the maya viewport. 

## v0.12.1
**Change Log**
* Variant Switching now supported
* Minor Menu GUI improvements
* Dead code removal
* open sourcing prep work, and documentation

## v0.9.14
**Change Log**
*Updated to support ALMayaReferences

**Known Issues**
*Having objects that have a MayaReference to the same path on disk has been reported to cause problems.

## v0.9.13
**Change Log**
*Disabled Asset Resolver Configuration

## v0.9.12
**Change Log**
* "add AL_USDMaya library to python bindings linked libraries"
* Importing of animated attributes

## v0.9.10
**Change Log**
* Updated the AL/__init__.py to merge in with the existing AL module to avoid  UsdMaya from stomping on PythongLibs' AL module.

## v0.9.8
**Change Log**
* Bug Fix: Scenes no longer crash in parallel evaluation mode. http://github.al.com.au/rnd/usdMaya/issues/41
* Bug Fix: Maya no longer crashes when modifying the filepath attribute of a proxyshape node. http://github.al.com.au/rnd/usdMaya/issues/121
* Bug Fix: Frame range incorrectly set when opening a shot. http://github.al.com.au/rnd/usdMaya/issues/72
* Bug Fix: maya crash if rig ref missing. http://github.al.com.au/rnd/usdMaya/issues/84
* Massive rename of nodes, and commands. The previous names (e.g. alUsdProxyShape) have been standardised with the rest of AL, so now it's 'AL_usdmaya_ProxyShape'.
* Support for the export of animated cameras via the 'AL usdmaya Export' translator
* The AL usdmaya plugin has been divorced from the orignal PXR maya plugin
* Remaining python code has been moved, previously you had to import the module by 'from pxrUsd import UsdMaya', now you should use 'from AL import UsdMaya'. 

**Known issues**
* Missing usdImport and usdExport command for animated data - http://github.al.com.au/rnd/usdMaya/issues/108
* TLS Problem in maya 2017 - you may have to switch off other plugins when working with our USD Plugin. See https://groups.google.com/forum/#!topic/usd-interest/wJr8c_iTO7k

## v0.9.6
**Change Log**
* Prim->MayaPath: Prim's that translate into a corresponding maya shape now point to the transform above the shape instead of ths shape
* Enabled backface culling in the proxyshapeUi
* Fixed "picaso" bug

**Known issues**
* Missing usdImport and usdExport command for animated data - http://github.al.com.au/rnd/usdMaya/issues/108
* TLS Problem in maya 2017 - you may have to switch off other plugins when working with our USD Plugin. See https://groups.google.com/forum/#!topic/usd-interest/wJr8c_iTO7k
* DG Parallel Eval: http://github.al.com.au/rnd/usdMaya/issues/41
* Frame Range http://github.al.com.au/rnd/usdMaya/issues/72
* Crash if ref rig missing http://github.al.com.au/rnd/usdMaya/issues/84

## v0.9.5
**Change Log**
* Simple profiler
* Global Command Executor problems
* Removed pixar mayaUsd source that isn't being used

**Known issues**
* Missing usdImport and usdExport command for animated data - http://github.al.com.au/rnd/usdMaya/issues/108
* TLS Problem in maya 2017 - you may have to switch off other plugins when working with our USD Plugin. See https://groups.google.com/forum/#!topic/usd-interest/wJr8c_iTO7k
* DG Parallel Eval: http://github.al.com.au/rnd/usdMaya/issues/41
* Frame Range http://github.al.com.au/rnd/usdMaya/issues/72
* Crash if ref rig missing http://github.al.com.au/rnd/usdMaya/issues/84

## v0.9.4
**Change Log**
* New patch for Maya PR71

**Known issues**
* TLS Problem in maya 2017 - you may have to switch off other plugins when working with our USD Plugin. See https://groups.google.com/forum/#!topic/usd-interest/wJr8c_iTO7k
* DG Parallel Eval: http://github.al.com.au/rnd/usdMaya/issues/41
* Frame Range http://github.al.com.au/rnd/usdMaya/issues/72
* Crash if ref rig missing http://github.al.com.au/rnd/usdMaya/issues/84

## v0.9.3
**Change Log**
* Added all_tests target which will be the target ran by the usdMaya_BUILD jenkins job.
* BugFix: Fixed MayaCache: http://github.al.com.au/rnd/usdMaya/issues/37

**Known issues**
* TLS Problem in maya 2017 - you may have to switch off other plugins when working with our USD Plugin. See https://groups.google.com/forum/#!topic/usd-interest/wJr8c_iTO7k
* DG Parallel Eval: http://github.al.com.au/rnd/usdMaya/issues/41
* Frame Range http://github.al.com.au/rnd/usdMaya/issues/72
* Crash if ref rig missing http://github.al.com.au/rnd/usdMaya/issues/84

## v0.9.2
**Documentation**
http://github.al.com.au/rnd/usdMaya/wiki

**Change Log**
* Initial UsdMaya unit tests.
* Fix for rabbit vertex windings, rabbit now shows correctly as white.
* Fix for UsdPrim->usdMaya path.
* Fix for crash when burnin can't find parent path.
* Fix for multiple rabbits, now correctly shows one rabbit

**Known issues**
* TLS Problem in maya 2017 - you may have to switch off other plugins when working with our USD Plugin. See https://groups.google.com/forum/#!topic/usd-interest/wJr8c_iTO7k
* MayaCache: http://github.al.com.au/rnd/usdMaya/issues/37
* DG Parallel Eval: http://github.al.com.au/rnd/usdMaya/issues/41
* Frame Range http://github.al.com.au/rnd/usdMaya/issues/72
* Crash if ref rig missing http://github.al.com.au/rnd/usdMaya/issues/84


