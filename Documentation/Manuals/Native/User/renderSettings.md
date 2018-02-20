Render settings						{#renderSettings}
===============

@ref bs::RenderSettings "RenderSettings" is an object present on every **Camera** object. It can be retrieved through @ref bs::CCamera::getRenderSettings() "Camera::getRenderSettings()" and allows you to customize what rendering effects are executed when rendering the scene through that view. 

For complete list of tweakable properties check the API reference, and here we'll just cover the main points.

# HDR and tonemapping
HDR stands for high-dynamic range, and it allows the lights in the scene to use a large range of intensity values that can more closely approximate a real-world scene. Lighting information is first written to a floating point texture that can store a wider range of values than a normal RGB texture. These high range lighting values are then used throughout the calculations in the engine, ensuring a higher quality final result. You can toggle HDR rendering through @ref bs::RenderSettings::enableHDR "RenderSettings::enableHDR".

Before the image is output to the screen it goes through the process called tonemapping, which converts the high range values into low range that a normal output device (like a monitor or a TV) can display. Tonemapping can be toggled through @ref bs::RenderSettings::enableTonemapping "RenderSettings::enableTonemapping", but this is generally something you always want to have on when HDR rendering is enabled.

@ref bs::RenderSettings::tonemapping "RenderSettings::tonemapping" provides you with a variety of tweakable options on how is tonemapping performed. Tonemapping operators are a complex topic, but it's enough to know that these options affect the feel and tone of the final image, and it is generally something you will tweak to suit the best look for your project.

HDR and tonemapping is also closely related to exposure, which we'll cover next.

# Exposure
Exposure determines which part of the high range image should be converted to low range (e.g. the very bright parts, the very dark parts, or somewhere in the middle). Generally this is a property you will only use when HDR is enabled, as LDR doesn't offer a high enough range for this property to be relevant.

By default the system will calculate the exposure automatically, based on how the human eye determines exposure. Generally this means if you are in a very bright area, it will be hard to see into darker areas (imagine standing outside in sunlight and looking into house lit only by artifical light), or when in a very dark area the bright areas will be overexposed. As you move between areas of different light intensity the exposure will slowly adjust accordingly. You can tweak automatic exposure options through @ref bs::RenderSettings::autoExposure "RenderSettings::autoExposure".

Automatic exposure can be disabled through @ref bs::RenderSettings::enableAutoExposure "RenderSettings::enableAutoExposure". In this case you will want to set the exposure manually through @ref bs::RenderSettings::exposureScale "RenderSettings::exposureScale"

# Screen space reflections
This effect provides high quality, real-time reflections at a fairly low performance impact. The main limitation effect is that it is performed in screen-space, and therefore cannot reflect an object that's not currently on the screen. When reflection cannot be found the system will fall back onto reflection probes for reflections. The effect is also generally not suitable for perfect mirror-like reflections due to limited precision.

You can control, and well as toggle the effect through @ref bs::RenderSettings::screenSpaceReflections "RenderSettings::screenSpaceReflections".

# Screen space ambient occlusion
TODO


Also: White balance, color grading, FXAA, depth of field



# Gamma
Tweaks the gamma value that's applied to the image before being sent to the output device. Mainly affects the brightness of the image.

# Shadows
Shadow rendering for a specific view can be completely disabled through @ref bs::RenderSettings::enableShadows "RenderSettings::enableShadows". Shadow options that are view-specific can be controlled through @ref bs::RenderSettings::shadowSettings "RenderSettings::shadowSettings".

# Indirect lighting
Indirect lighting provided by **LightProbeVolume** can be fully disabled for a view through @ref bs::RenderSettings::enableIndirectLighting "RenderSettings::enableIndirectLighting".
