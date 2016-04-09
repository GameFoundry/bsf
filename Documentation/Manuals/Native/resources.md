Resources						{#resources}
===============
[TOC]

Resources are objects that can be saved and loaded to/from a disk, can be referenced in a unique manner, and have their lifetime managed by the resources manager. Objects like @ref BansheeEngine::Texture "Texture", @ref BansheeEngine::Mesh "Mesh" or @ref BansheeEngine::Shader "Shader" are all resources.

A resource in Banshee is represented by the @ref BansheeEngine::Resource "Resource" class. A @ref BansheeEngine::Resource "Resource" is a @ref BansheeEngine::CoreObject "CoreObject" and if you're not familiar with core objects you should read the [core object](@ref coreThread) manual.

All resources in Banshee are managed by the @ref BansheeEngine::Resources "Resources" manager. Use @ref BansheeEngine::gResources "gResources()" to access its instance easily.

# Loading {#resources_a}
To load a resource call @ref BansheeEngine::Resources::load "Resource::load" with a file path to the resource. If the path is valid a handle to the resource will be returned.

You can also call @ref BansheeEngine::Resources::loadAsync "Resource::loadAsync". Unlike the previous method this method will not block the current thread and will instead proceed with loading the resource on a separate thread. You will receive an uninitialized resource handle. You must use @ref BansheeEngine::ResourceHandleBase::isLoaded "HResource::isLoaded" to check if the handle is loaded before using it, or @ref BansheeEngine::ResourceHandleBase::blockUntilLoaded "HResource::blockUntilLoaded" to force the current thread to wait until it is loaded.

An example of both synchronous and asynchronous loads:
~~~~~~~~~~~~~{.cpp}
// Load a texture synchronously
HTexture textureA = gResources().load<Texture>("textureA.asset");

// Load a texture asynchronously
HTexture textureB = gResources().loadAsync<Texture>("textureB.asset");

while(!textureB.isLoaded())
{
	// Do something else
}
~~~~~~~~~~~~~

The system internally keeps track of all resources, so calling load multiple times in a row will return a previously loaded resource if one exists.

## Resource handles {#resources_a_a}
As we saw in the previous example resources are referenced by @ref BansheeEngine::ResourceHandleBase "resource handles". Different handles exist for each resource type, and they always being with a prefix "H" followed by the resource name, e.g. @ref BansheeEngine::ResourceHandleBase "HTexture".

Handles act like pointers, and you can access the resource within the handle by using the pointer member "->" operator. Using this operator is only valid if the handle points to a loaded resource (@ref BansheeEngine::ResourceHandleBase::isLoaded "HResource::isLoaded" returns true). Using the normal member access "." operator works on the handle data directly and is always safe.

Handles are used instead of pointers because:
 - Lifetime of the resource can be automatically managed
 - User can keep a reference to an unloaded resources
 - Are handled specially when serializing/deserializing objects
 
A little more elaboration for the last point. If you have read the [RTTI](@ref rtti) manual you know that objects can reference different field types. And resource handles can be referenced by "Reflectable" field types, allowing you to easily save/load references to resources within your objects with no additional code.

Each handle internally stores a GUID that uniquely identifies the resource. You can access it by calling @ref BansheeEngine::ResourceHandleBase::getUUID() "HResource::getUUID".

## Resource dependencies {#resource_a_b}
We mentioned in the last section that objects (including resources) can reference resources. For example a @ref BansheeEngine::Font "Font" resource will reference @ref BansheeEngine::Texture "Texture" resources which store its pre-rendered font glyphs.

When loading a resource you have a choice to load only that exact resource, or if you wish to load all of its dependencies as well. This is handled by the `loadDependencies` parameter of both @ref BansheeEngine::Resources::load "Resource::load" and @ref BansheeEngine::Resources::loadAsync "Resource::loadAsync". If you load a resource without dependencies, and need them later you can just call @ref BansheeEngine::Resources::load "Resource::load" or @ref BansheeEngine::Resources::loadAsync "Resource::loadAsync" on that same resource with `loadDependencies` set to true.

## Resource lifetime {#resource_a_c}
Once a resource is loaded it will stay loaded until all references (handles) to it go out of scope. Once the last handle goes out of scope the system will unload the resource.

If you wish to keep a handle to a resource that doesn't keep it loaded, you can use @ref BansheeEngine::TResourceHandle<T, true> "WeakResourceHandle<T>". You retrieve such a handle by calling @ref BansheeEngine::TResourceHandle<T, WeakHandle>::getWeak "HResource::getWeak" method on a normal handle. You can convert a weak handle into a normal handle by calling @ref BansheeEngine::TResourceHandle<T, WeakHandle>::lock "HResource::lock". Before doing so you should verify that the resource is still loaded.

Weak handles can also be used for referencing resources for serialization/deserialization, same as normal handles can. However a weak handle will never force the referenced resource to load, even if `loadDependencies` parameter is set to true when loading. You must call @ref BansheeEngine::Resources::load(const WeakResourceHandle<T>&, bool, bool) "Resource::load" to manually load such a resource.

You can also force a resource to stay loaded, even after all the handles go out of scope. You do it by setting `keepInternalReference` of @ref BansheeEngine::Resources::load "Resource::load" or @ref BansheeEngine::Resources::loadAsync "Resource::loadAsync" to true. In order to free such a resource you should call @ref BansheeEngine::Resources::release "Resources::release". Be aware that each call to @ref BansheeEngine::Resources::load "Resource::load" or @ref BansheeEngine::Resources::loadAsync "Resource::loadAsync" with this parameter enabled will require a separate @ref BansheeEngine::Resources::release "Resources::release" call. @ref BansheeEngine::Resources::release "Resources::release" will not release the resource immediately if there are still active handles to it, it will merely remove the internal reference so the resource can be freed when the handles go out of scope.

You can also use @ref BansheeEngine::Resources::unloadAllUnused "Resources::unloadAllUnused" which will unload any resource that has no handles referencing it.

# Saving {#resources_b}
To save a resource to the disk call @ref BansheeEngine::Resources::save(const HResource&, const Path&, bool) "Resources::save". You need to provide a handle to the resource, and a path to save the resource to.

If a resource already exists on the disk (i.e. it was created by @ref BansheeEngine::Resources::load "Resource::load" instead of manually), you can instead call @ref BansheeEngine::Resources::save(const HResource&) "Resources::save" to overwrite the existing file.

An example:
~~~~~~~~~~~~~{.cpp}
HTexture texture = Texture::create(TEX_TYPE_2D, 1280, 720, 0, PF_R8G8B8A8);
... fill texture with some data ...

gResources().save(texture, "myTexture.asset");
~~~~~~~~~~~~~

# Creating your own resource

// General info, registering with Resources
// Custom resource handle
// RTTI - Link to manual
// Exposing resource to scripting

# Advanced

// Resource dependencies
// Resource listener


// TODO - Resource manifest