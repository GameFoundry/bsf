Image based lighting						{#imageBasedLighting}
===============

Image based lighting is a form of lighting that uses cubemap textures to provide an approximation of the environment in order to provide more realistic lighting. This form of lighting is primarily important when rendering glossy reflections on metal surfaces that use physically based materials. Without some form of image based lighting set up in your scene those reflections will not reflect anything, and won't look proper. 

It is of lesser importance for diffuse (non-specular) lighting, but can also provide a benefit that provides a cheap global illumination solution, by using irradiance maps to calculate indirect lighting.

There are two main types of image based lights:
 - Skybox - Represented by the @ref bs::CSkybox "Skybox" component. 
 - Reflection probe - Represented by the @ref bs::CReflectionProbe "ReflectionProbe" component. 
 
# Skybox
Skyboxes use a user-provided HDR cubemap texture in order to display an image of the sky when the camera is looking at the scene when no other object is ocluding the sky. The same image is also used to provide both specular reflections and indirect lighting on objects lit by the sky. By default all objects not in radius of a reflection probe (more about them below) is considered lit by the sky.

Skybox is represented by the **Skybox** component, which requires only a HDR texture of the sky to work.

~~~~~~~~~~~~~{.cpp}
// Import a sky cubemap from a cylindrical (panoramic) image
SPtr<ImportOptions> tio = TextureImportOptions::create();
tio->setIsCubemap(true);
tio->setCubemapSourceType(CubemapSourceType::Cylindrical);
tio->setFormat(PF_FLOAT_R11G11B10); // Or the 16-bit floating point format

HTexture skyTexture = gImporter().import<Texture>("MySkybox.hdr", tio);

// Set up the skybox
HSceneObject skyboxSO = SceneObject::create("Skybox");
HSkybox skybox = skyboxSO->addComponent<CSkybox>();

skybox->setTexture(skyTexture);
~~~~~~~~~~~~~

Optionally you might also want to increase or decrease the brightness of the sky by calling @ref bs::CSkybox::setBrightness "CSkybox::setBrightness()". Note that this will not effect the visual appearance of the sky but will only affect the lighting cast by the sky on other surfaces.

## Combining sky and analytical lights
Lights represented by the **Light** component we talked earlier are called analytical lights. When using an HDR skybox texture recorded outside of the engine you might need to tweak the texture so it works well with such analytical lights. Some cameras might record HDR values in non-physical units in which case sky might appear too bright or too dark, in which case analytical lights might appear over- or underpowered.

Additionally, depending on your project, you might want to cut out the Sun from the recorded HDR image (for example, replace it with a bit of sky), and then use the directional analytical light for the Sun instead.

# Reflection probes
While the skybox is used to provide outdoor reflections, reflection probes are used to create reflection cubemaps for indoor environments. Reflection probes have an origin and a radius of influence. Reflection probes also use HDR cubemaps, but instead of using external textures those cubemaps are generated in-engine, at the position of the reflection probe. They are represented using the **ReflectionProbe** component.

~~~~~~~~~~~~~{.cpp}
HSceneObject reflProbeSO = SceneObject::create("Refl. probe");
HReflectionProbe reflProbe = reflProbeSO->addComponent<CReflectionProbe>();
~~~~~~~~~~~~~

You must provide the extents of the geometry covered by the reflection probe. These extents serve both to determine a range of influence, and to approximate the surrounding geometry. For example if you are placing a reflection probe that covers a room, you should strive to match the reflection probe extents with the room walls.

You can assign extents in two ways, depending on reflection probe type:
 - @ref bs::ReflectionProbeType::Box "ReflectionProbeType::Box" - Reflection probe is represented by a box and extents are set by calling @ref bs::CReflectionProbe::setExtents "CReflectionProbe::setExtents()".
 - @ref bs::ReflectionProbeType::Sphere "ReflectionProbeType::Sphere" - Reflection probe is represented by a sphere an extents are set by calling @ref bs::CReflectionProbe::setRadius "CReflectionProbe::setRadius()".
 
You can change the type of the reflection probe (and therefore extents) by calling @ref bs::CReflectionProbe::setType "CReflectionProbe::setType()".

~~~~~~~~~~~~~{.cpp}
reflProbe->setType(ReflectionProbeType::Box);
reflProbe->setExtents(Vector3(2.0f, 2.0f, 2.0f));
~~~~~~~~~~~~~

## Generating reflection probe
Reflection probe cubemap will be generated automatically when the reflection probe is first added to the scene, and whenever it is moved. You can also force the cubemap to regenerate by calling @ref bs::CReflectionProbe::generate "CReflectionProbe::generate()". This is required when surrounding geometry changes and you wish to update the probe cubemap.

~~~~~~~~~~~~~{.cpp}
reflProbe->generate();
~~~~~~~~~~~~~

## Using external textures
In case you want to use an external HDR texture, similar to a skybox, you can call @ref bs::CReflectionProbe::setCustomTexture "CReflectionProbe::setCustomTexture()". The system will no longer use the automatically generated cubemap and use the provided one instead. If you wish to switch back to the automatic generator, call the method with a null value.

~~~~~~~~~~~~~{.cpp}
HTexture myCubemap = ...;

reflProbe->setCustomTexture(myCubemap);
~~~~~~~~~~~~~

## Reflection probe interpolation
When multiple reflection probes overlap the system will blend between the reflection probes based on the distance from the origin and the probe extents. If system can't blend with other reflection probes it will instead blend with the sky. This means in most cases you want to ensure that reflection probes overlap, in order to provide clean transitions.