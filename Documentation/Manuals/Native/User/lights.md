Lights						{#lights}
===============

Lights control the rendering of the nearby **Renderable** objects, by attempting to simulate how a real light would affect them. They are represented by the @ref bs::CLight "Light" component.

# Creating a light
**Light** is created as any component, and requires no additional parameters.

~~~~~~~~~~~~~{.cpp}
HSceneObject lightSO = SceneObject::create("Light");
HLight light = lightSO->addComponent<CLight>();
~~~~~~~~~~~~~

Once created light can be positioned and oriented normally using its **SceneObject**.

# Light types

// TODO

# Light properties

## Color

## Intensity

## Range

// TODO - Also mention physically based attenuation

## Shadows

## Spot angles

// TODO - Mention both angle types