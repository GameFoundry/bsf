Resource saving and loading					{#resourceSavingAndLoading}
===============

All resource save and load operations are managed through the @ref bs::Resources "Resources" module, accessible through @ref bs::gResources() "gResources"().

# Saving
Once a resource has been imported you can save it for later use. The advantage of saving a resource (instead of importing it every time) is performance - resource import is usually a costly operation. Saved resources remain in engine-friendly format and can be easily loaded later. 

To save a resource call @ref bs::Resources::save "Resources::save()". Lets see an example where we import a texture and then save it for later use:

~~~~~~~~~~~~~{.cpp}
// Import a texture named "myTexture.jpg" from the disk
HTexture texture = gImporter().import<Texture>("myTexture.jpg");

// Save the texture for later use
gResources().save(texture, "myTexture.asset");
~~~~~~~~~~~~~

Note that resources can also be created within the engine, and don't necessarily have to be imported. e.g. you can populate texture pixels or mesh vertices manually, and then save the resource in this same manner. We will show later how to manually create resources.

# Loading
Once a resource has been saved you can load it at any time using @ref bs::Resources::load "Resources::load()". Lets load the texture we just saved:

~~~~~~~~~~~~~{.cpp}
HTexture loadedTexture = gResources().load<Texture>("myTexture.asset");
~~~~~~~~~~~~~

> If you attempt to load a resource that has already been loaded, the system will return the existing resource.

# Unloading
Note that the resource system by default never unloads a loaded resource, unless told explicitly. To unload a resource call @ref bs::Resources::release "Resources::release()".

To unload the texture we loaded previously:
~~~~~~~~~~~~~{.cpp}
gResources().release(loadedTexture);
~~~~~~~~~~~~~

> Note that if you called **Resources::load()** multiple times, you must also call **Resources::release** the same amount of times.