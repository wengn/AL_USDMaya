# User Stories
1. As an Modelling Artist, I want to be able to create a new asset hierarchy from scratch, this is will allow me to start my work.

2. As an Modelling Artist, I want to be able to iteratively modify:
a) The hierarchy of an existing asset through addition, removal, reorganisation
b) The geometric properties and topology of the meshes or curves on that existing asset 

3. As a Modelling, Environment or other Assets Artist, I want to be able to make modifications to an existing asset in the context of a Set/Environment, and store the change to an override layer.

4. Same as 3 but for artists in Shot Based departments

We think we can support this with 2 different workflows:
# 1. Creation of new Geometry Assets or static Modification of existing assets

Currently we intend to deal with this using static USD->Maya->USD Conversion - i.e we use the existing Static Translators to import a USD geometry hierarchy to maya, allow arbitrary edits/modifications followed by export back to USD. Where we are creating a new asset,  there is no import step (although our pipeline may supply us with a sphere or ball or other proxy to get started with). This is equivalent to importing/exporting an Alembic file to/from maya and removing any Alembic DG Node created

Pros:
+ Import to static maya mesh means we have no live connection to USD making it easier to create new construction history etc

Cons:
+ Having this support every permutation of import/export of  USD and maya data conversion places a heavy burden on the import/export code to be able to deal with every case. 
+ Support for Renderer-specific attributes and other custom bits requires studio customisation of this code also
+ Once imported, we remove any connection to USD, so support for (e.g) animation is not possible without implementing some kind of AlembicMaya-like MpxNode.
+ As there is no "live" connection with a USD Stage, it's up to higher level tools to work out where to export the geometry to, there is no trace of "where it came from" in USD.

# 2. Editing of existing Geometry Assets 
We select a prim or prim hierarchy in the viewport, and run something like this py-pseudocode
```
primPaths = currStage.getSelectedPrimPaths()
for p in primPaths:
    currStage.translatePrim(p)
```
This creates:
+ an AL_USDMaya transform for each transform
+ a real maya shape for each piece of geometry(mesh/curve) 
...under the relevant proxy shape.

We should consider how to deal with unsupported types - maybe need a warning?

Any changes we make to the geometry can be "committed" back to a USD Layer. The changes can  target a specific EditTarget, such as a new or existing Layer or a new or existing variant. 
We want to preserve any other data on these prims such as metadata, attributes etc which has not been part of the geometry import

Changes we would like to support are:
+ Modification of geometry topology, verts, normals, UVs etc
+ Modification of the hierarchy (adding, deleting, renaming prims and general hierarchy re-organisation


# Work to do
+ Selective Translation - currently we have a hardcoded mapping between USD Schemas and their matching translator - when traversing the USD Scene structure we will import any schemas we have a translator for. In the case of geometry, we want to be able to call the translator explicitly on a prim or prim hierarchy outside of this "import USD Stage" context (e.g via a Maya Command)
+ Generalise the Schema Translator Plugin System so it can be used in these other contexts
+ Migrate all static translators such as geometry to the new Schema Translator Plugin System (or duplicate the code if the risk of breakage of the static translators looks high), or more ideally - move the code into a separate low-level USDToMaya utility library that can eventually be separated out into a separate package, and shared/updated with other studios. We will also have to add an export interface to the STP system
+ Update the static translation import/export entry points to use the updated Schema Translator Plugin System
+ In our new GeometrySchemaTranslatorPlugin, we think that the "preTearDown" implementation should push the geometry back to the currently selected edit target of the USD Stage
+ Currently, preTearDown/tearDown will be called when the prim is swapped out of the maya scene by a variant switch, stage reload or other USD operation. We will also want to be able to explicitly trigger a "commit" operation to write the changes back to USD (which probably also needs to remove the maya geometry from the scene)
+ While we have discussed (in the context of our use cases) translation of selected prims by a user in interactive mode, there are also cases where we might like to automatically translate some prims as mesh geometry - this could be achieved by for example, tagging the prim as "alwaysImportToMaya, or reacting to a naming convention (eg always translate prims which start with the name "Maya_" or some other mechanism. Can we support user implemented schemes for this?

# Some areas for investigation/clarification/discussion
+ Currently we would write back "default values" i.e non-sampled data as we don't think we have a use case for the sampled case right now
+ If we import geometry which contains points, connectivity, normals, uvs, and we modify just points, and write back to a USD layer, does USD elide writing the normals, uvs etc as part of the "over"? If not, should it? Should we implement our own diffing and deduplication here? If so does USD provide a way to fingerprint/hash each channel for easy comparison? If not, what scheme should we use
+ Following from this, do we need to be able to selectively push specific channels of data back to the layer? (e.g UVs)
+ Once we have a hierarchy of USD prims in maya - how much can we manipulate this without breaking the translation model, and what happens when if does? If we add new objects to the maya hierarchy, can we write them back as defs to the USD layer. What about renaming and deletion? What about duplication of maya nodes (a common modelling workflow - creation of standard maya transforms rather than AL_USDMaya transforms will probably break things). And - instancing?
+ When we use the "preTearDown" notice handler to save edits, we may need to discuss with Pixar as the notice system is not supposed to be used to edit further USD entities




