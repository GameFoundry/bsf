Importing meshes 						{#importingMeshes}
===============

Meshes are used for defining surfaces of 2D and 3D objects, and are the primary building blocks of the scene. They are represented with a set of vertices and triangles. Those vertices and triangles are then transformed by the graphics card when rendering, by converting them to pixels on the screen. 

Each vertex can have one or multiple properties like texture coordinates (used for mapping textures onto mesh surface), normals, tangents, and at minimum, a position property. These properties are used by materials for customizing how a mesh is rendered (explained later).

Vertices are connected into triangles via indices, where every three sequential indices specify which three vertices form a triangle. That information is then used by the graphics card for rendering.

In Banshee meshes are represented with the @ref bs::Mesh "Mesh" class. A mesh is a resource, meaning it can be imported, saved and loaded as we described in the Resource manuals.

![Wireframe mesh](DragonWireframe.png) 

# Importing a mesh
Meshes can be imported from various third party formats, using the importer.

~~~~~~~~~~~~~{.cpp}
// Import a mesh named "dragon.fbx" from the disk
HMesh mesh = gImporter().import<Mesh>("dragon.fbx");
~~~~~~~~~~~~~

Supported formats are:
 - FBX
 - DAE
 - OBJ
 
# Mesh properties
Once a mesh has been imported, you can retrieve its properties like vertex & index counts, as well as its bounds by calling @ref bs::Mesh::getProperties "Mesh::getProperties()", which returns a @ref bs::MeshProperties "MeshProperties" object.

~~~~~~~~~~~~~{.cpp}
// Retrieve and print out various mesh properties
auto& props = mesh->getProperties();

gDebug().logDebug("Num. vertices: " + toString(props.getNumVertices()));
gDebug().logDebug("Num. indices: " + toString(props.getNumIndices()));
gDebug().logDebug("Radius: " + toString(props.getBounds().getSphere().getRadius()));
~~~~~~~~~~~~~

# Rendering meshes
**Mesh** can be assigned to a **Renderable** component, along with a **Material** in order to be rendered. We'll cover rendering in a later chapter.

# Customizing import
Mesh import can be customized by providing a @ref bs::MeshImportOptions "MeshImportOptions" object to the importer.

~~~~~~~~~~~~~{.cpp}
auto importOptions = MeshImportOptions::create();
// Set required options here (as described below)

HMesh mesh = gImporter().import<Mesh>("dragon.fbx", importOptions);
~~~~~~~~~~~~~

Lets see some of the options you can use for customizing import.

## Scale
@ref bs::MeshImportOptions::setImportScale "MeshImportOptions::setImportScale" allows you to apply a uniform scale value to the mesh upon import. Although you can scale the size of a rendered mesh by adjusting the **SceneObject** transform when its placed in the scene, sometimes it is more useful to be able to do it once at import instead of every time you place it.

~~~~~~~~~~~~~{.cpp}
// Reduce the size of the mesh to 10% of its original size
importOptions->setImportScale(0.1f);
~~~~~~~~~~~~~

## Normals
@ref bs::MeshImportOptions::setImportNormals "MeshImportOptions::setImportNormals" controls whether normal vectors are imported from the mesh file. 

Normal vectors are used in lighting and are required for any meshes placed in the 3D scene (unless rendering them manually using some custom method). They allow the mesh to appear smooth even though its surface is made out of triangles.

Most 3D authoring tools generate normals for their meshes, but if normals are not present in the mesh file, Banshee will attempt to generate normals automatically when this option is turned on.

~~~~~~~~~~~~~{.cpp}
// Import or generate normals for the mesh
importOptions->setImportNormals(true);
~~~~~~~~~~~~~

## Tangent
@ref bs::MeshImportOptions::setImportTangents "MeshImportOptions::setImportTangents" controls whether tangent vectors are imported from the mesh file. 

Tangent vectors (along with normal vectors) are required if your rendering shader uses normal maps. Similar to normals, if tangents are not present in the mesh file, Banshee will attempt to generate them automatically.

~~~~~~~~~~~~~{.cpp}
// Import or generate normals for the mesh
importOptions->setImportTangents(true);
~~~~~~~~~~~~~

## Caching
Similar as with textures, sometimes you need to import a mesh you don't want to only use for rendering, but rather for manually reading its contents. When that's the case you can enable the @ref bs::MeshImportOptions::setCPUCached "MeshImportOptions::setCPUCached" option.

This will allow you to call @ref bs::Mesh::readCachedData "Mesh::readCachedData" and to manually read individual vertices and indices of the mesh.

Note that caching a mesh means its data will be available in system memory, essentially doubling its memory usage.

~~~~~~~~~~~~~{.cpp}
// Enable caching
importOptions->setCPUCached(true);

// Import mesh
HMesh mesh = gImporter().import<Mesh>("dragon.fbx", importOptions);

// Allocate a buffer to hold mesh contents
//// Specify vertex properties. Assuming just a single property of a 3D position
SPtr<VertexDataDesc> vertexDesc = VertexDataDesc::create();
vertexDesc->addVertElem(VET_FLOAT3, VES_POSITION);

//// Create enough room for 8 vertices of specified properties, and 36 indices (12 triangles)
SPtr<MeshData> meshData = MeshData::create(8, 36, vertexDesc);

// Read cached data
mesh->readCachedData(*meshData);

// Read vertex positions
Vector<Vector3> vertices(8);
meshData->getVertexData(VES_POSITION, vertices.data(), vertices.size() * sizeof(Vector3));
...
~~~~~~~~~~~~~

> We will explain **MeshData** and mesh manipulation in detail, in a later chapter.