Advanced resources					{#advResources}
===============

We've covered basic resource operations already, and this manual will cover a more advanced topics that allow you to do more with resources.

# Resource lifetime {#advResources_a}
Whenever you load a resource, that resource will be kept loaded until all references to that resource are lost. Each resource handle (e.g. **HMesh**) that exists represents a single reference. By default an additional "internal" reference is also created and held by the system internally. This ensures the resource stays loaded even when all handles are destroyed.

This internal reference must be released by calling @ref bs::Resources::release() "Resources::release()", as we mentioned when we talk about resources before.

You can also force the system to not create the internal reference by passing a custom @ref bs::ResourceLoadFlag "ResourceLoadFlag" to the @ref bs::Resources::load() "Resources::load()" method. This flag should not include the @ref bs::ResourceLoadFlag::KeepInternalRef "ResourceLoadFlag::KeepInternalRef".

~~~~~~~~~~~~~{.cpp}
// The default load flag is a combination of ResourceLoadFlag::KeepInternalRef and
// ResourceLoadFlag::LoadDependencies. We don't include the former to prevent the
// internal reference from being created.
HMesh mesh = gResources().load("myMesh.asset", ResourceLoadFlag::LoadDependencies);
~~~~~~~~~~~~~ 

## Weak handles {#advResources_a_a}
In case you want to keep a reference to a resource without incrementing the reference count you can use a weak handle instead of a normal one. Weak handles are represented by the @ref bs::WeakResourceHandle<T> "WeakResourceHandle<T>" class and can be retrieved from normal handles by calling @ref bs::ResourceHandle<T>::getWeak "ResourceHandle<T>::getWeak()". Other than that they are accessed the same as normal handles.

~~~~~~~~~~~~~{.cpp}
// Load a mesh and store a handle as normal
HMesh mesh = gResources().load("myMesh.asset");

// Create a weak handle
WeakResourceHandle<Mesh> weakMesh = mesh.getWeak();
~~~~~~~~~~~~~ 

# Resource dependencies {#advResources_b}
Whenever you load a resource the system will automatically enumerate all dependencies of that resource and attempt to load them as well. For example when loading a **Material** it will automatically load its **Shader** and any referenced **Texture** resources.

In case you wish to prevent that you can not provide the @ref bs::ResourceLoadFlag::LoadDependencies "ResourceLoadFlag::LoadDependencies" flag when calling **Resources::load()** (provided by default).

~~~~~~~~~~~~~{.cpp}
// Loads a resource without any dependencies, nor keeps an internal reference
HMesh mesh = gResources().load("myMesh.asset", ResourceLoadFlag::None);
~~~~~~~~~~~~~ 

# Asynchronous loading {#advResources_c}
Resources can be loaded asynchronously (in the background) by calling @ref bs::Resources::loadAsync "Resources::loadAsync()". The interface is identical to **Resources::load()**. The main difference is that the returned handle might will contain a reference to a resource that hasn't been loaded yet. 

You can check if a resource handle is pointing to a loaded resource by calling @ref bs::ResourceHandle::isLoaded "ResourceHandle::isLoaded()".

~~~~~~~~~~~~~{.cpp}
HMesh mesh = gResources().loadAsync("myMesh.asset");

if(mesh.isLoaded())
{
	// Check if loaded before doing something
}
~~~~~~~~~~~~~ 

> Note that not-yet-loaded resource handles can be provided to some engine systems, but not all. Generally a system will note in its documentation if it works with such resource handles.

You can block the current thread until a resource is loaded by calling @ref bs::ResourceHandle::blockUntilLoaded "ResourceHandle::blockUntilLoaded()".

~~~~~~~~~~~~~{.cpp}
mesh.blockUntilLoaded();
// Makes sure the mesh is loaded at this point
~~~~~~~~~~~~~ 