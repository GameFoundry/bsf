---
title: Lights
---

Lights control the rendering of the nearby **Renderable** objects, by attempting to simulate how a real light would affect them. They are represented by the @bs::CLight component. They are essential for creating a realistic 3D scene.

# Creating a light
**Light** is created as any component, and requires no additional parameters.

~~~~~~~~~~~~~{.cpp}
HSceneObject lightSO = SceneObject::create("Light");
HLight light = lightSO->addComponent<CLight>();
~~~~~~~~~~~~~

Once created light can be positioned and oriented normally using its **SceneObject**.

# Light types
Lights have three variants that determine how light affects surrounding objects: radial, spot and directional. You can change between light variants with @bs::CLight::setType.

~~~~~~~~~~~~~{.cpp}
// Make a radial light
light->setType(LightType::Radial);
~~~~~~~~~~~~~

## Radial light

Radial light affects everything within a certain radius from the light (i.e. its sphere of influence). It is the most basic type of light.

[TODO_IMAGE]()

## Spot light

Spot lights only illuminate a certain direction (i.e. like a flash-light). The user can specify the angle of illumination.

[TODO_IMAGE]()

## Directional light

Directional lights are a special type of light primarily used for simulating sun-light, or otherwise far-away objects. Unlike radial or spot lights they cannot be positioned. Only their orientation matters and they equally affect every object in the scene.

[TODO_IMAGE]()

# Light properties
Each light has a set of properties you can adjust (aside from position/orientation, which are handled by the **SceneObject**). Most of the properties are common for all light types but some are only relevant for specific types. We'll go over all of the properties below.

## Color
As the name implies, this controls what color light is being emitted from the source. It can be set by calling @bs::CLight::setColor and it is valid for all light types.

## Attenuation radius
Attenuation radius determines how far away does the light's influence reach. For radial lights this is the radius of the sphere of influence, and for spot lights this is the distance from the origin in the direction of the light. This property is not relevant for directional lights as their range is infinite. Use @bs::CLight::setAttenuationRadius to set the range.

Note that the manually set range will only be used if automatic attenuation is disabled. You can toggle this by calling @bs::CLight::setUseAutoAttenuation. When automatic attenuation is enabled the maximum range is calculated automatically based on the light intensity (described below). This automatic attenuation will smoothly cut off the light influence when it reaches roughly 10% of its intensity, but can result in very large attenuation radius, which can affect performance.

## Intensity
This controls how strong is the light. Although you could technically control light intensity using the color property (by using a lighter or darker color), using the intensity allows the engine to simulate high-dynamic range. 

In nature the range of light intensities varies highly - standing outside at a sunlit day may be hundreds or thousands of times brighter than standing indoors illuminated by a lightbulb. We won't perceive such a large difference because our eyes are able to adjust to different intensities.

bs::f uses a HDR algorithm to try to approximate this adjustment, which results in more realistic and higher quality lighting. Therefore it can be important for realism to set up the light intensities similar to what they would be in nature. 

Use @bs::CLight::setIntensity to change the light intensity. 

The specific units used for intensity (in case you want to look them up for actual light sources) are *luminous flux* for radial/spot lights, and *luminance* for directional lights. Generally this means directional light intensity should be much lower than for radial/spot lights.

## Source radius
By default all lights are considered point (punctual) lights, meaning they have no surface area. In case you want to make an area light set the source radius of the light by calling @bs::CLight::setSourceRadius. The value of this property is interpreted differently depending on light type:
 - Radial - Light represents a spherical area light and source radius is the sphere's radius
 - Spot - Light represents a disc area light (oriented toward spot direction) and the source radius in the disc radius
 - Directional - Light represents a disc area light projected on the sky, and the source radius represents the angular radius (in degrees) of the light. For example, Sun's angular radius is roughly 0.5° (varies slighly).

Area light types are particulary important for physically based rendering, as they can produce realistic looking specular reflections, which is not the case for point lights. 
 
## Shadows
Lights may or may not cast shadows. For realism all lights should cast shadows, but that is not feasible due to the high performance costs of using shadows. Therefore you should enable shadows only for one, or a few important lights. Use @bs::CLight::setCastsShadow to enable or disable shadow casting.

Casting shadows can cause artifacts called "shadow acne" in the scene. These artifacts occur due to an object casting a shadow on itself, caused limited precision of the calculations used. To combat this effect you can tweak the shadow bias property. Shadow bias moves the distance from which the shadow is cast, ensuring incorrect self-shadowing is avoided. Shadow bias can be tweaked by calling @bs::CLight::setShadowBias.

[TODO_IMAGE]()

Valid shadow bias values are from -1 to 1. When value is 0 no shadow bias will be applied, while positive shadow bias values will offset the shadow distance as described above. However offseting the shadow distance may cause the objects to appear like they are floating even if they are in contact with a surface.

[TODO_IMAGE]()

By setting the shadow bias to a negative value you move the shadow backwards, resolving both the shadow acne and the floating object issue. However this only works if your geometry has thickness, otherwise elements behind the object will be incorrectly shadowed. This is generally the best option as long as you can set up your geometry correctly.

## Spot angles
Spot light have a property that defines at how wide an angle do they cast light in. Narrower angle means a more focused light beam, while wider angle means a weaker light covering a larger area. 

You can set the spot light angle (also knows as total angle) with @bs::CLight::setSpotAngle. Note that light intensity will be spread out over the range, so when increasing the angle, you might also want to increase the intensity to keep the perceived brightness the same.

You can also call @bs::CLight::setSpotFalloffAngle to set an angle that allows a smooth falloff at the edge of the lights radius. This must be an angle smaller than the total spot angle. Within this angle the light intensity will be uniform, but as the angle increases past the falloff angle and towards the total angle and intensity will slowly reduce, resulting in a smooth falloff curve. If you don't want the smooth falloff, set this angle to the same value as the total angle.

[TODO_IMAGE]()
