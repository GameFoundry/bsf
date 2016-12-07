Materials 						{#simpleMaterial}
===============

Materials are objects that control how are meshes rendered. They are represented using the @ref bs::Material "Material" class. Each material must have one **Shader** object, and zero or more parameters.

A shader is a set of GPU programs and render states that tell the GPU how is mesh meant to be rendered. Most GPU programs in the shader have parameters that can be used for cutomizing the shader output. The primary use of the material is to allow the user to set those parameters. You can think of shaders as templates, and materials as instances of shaders - similar as you would think of a *class* vs. *object* relationship in a programming language.

We will delay further discussion of shaders and GPU programs to a later chapter.

# Material creation
To create a material use the @ref bs::Material::create "Material::create" method, which expects a **Shader** as a parameter.

~~~~~~~~~~~~~{.cpp}
// Get one of the built-in shaders
HShader shader = gBuiltinResources().getBuiltinShader(BuiltinShader::Standard);

// Or import your own shader (discussed later)
// HShader shader = gImporter().import<Shader>("myShader.bsl");

// Create a material based on the shader
HMaterial material = Material::create(shader);
~~~~~~~~~~~~~

> @ref bs::BuiltinResources "BuiltinResources" object contains a set of common shaders you can use without creating your own. The standard shader we use in the example above provides the most common functionality for rendering 3D objects.

# Setting parameters
As we mentioned, the main purpose of a material is to provide a way to set various parameters exposed by the shader. The standard shader we used in the example above accepts a parameter that allows to user to change which texture to render the object with. 

~~~~~~~~~~~~~{.cpp}
HTexture texture = gImporter().import<Texture>("myTexture.png");

// Set the texture for the "gAlbedoTex" parameter.
material->setTexture("gAlbedoTex", texture);
~~~~~~~~~~~~~

After the texture has been set, anything rendered with that material will now have that particular texture applied. Different shaders will accept different parameters of different types. You can find out parameter names and types by looking at the shader source (discussed later).

In this particular example we have a parameter named "gAlbedoTex" that accepts a **Texture** resource. We set such a parameter by calling @ref bs::Material::setTexture "Material::setTexture". There are other parameter types like floats, ints, colors, as well as multi-dimensional types like vectors and matrices which can be set by calling @ref bs::Material::setFloat "Material::setFloat", @ref bs::Material::setColor "Material::setColor", @ref bs::Material::setVec4 "Material::setVec4" and similar.

~~~~~~~~~~~~~{.cpp}
// Assuming our material has some more parameters, for purposes of the example
material->setColor("color", Color::White);
material->setFloat("time", 30.0f);
material->setVec3("position", Vector3(0, 15.0f, 10.0f));
material->setMat4("someTransform", Matrix4::IDENTITY);
~~~~~~~~~~~~~

## Sampler states
Sampler states are a special type of parameters that can be set by calling @ref bs::Material::setSamplerState "Material::setSamplerState". These states are used to control how is a texture read in a shader - for example they control what type of filtering to use, how to handle out of range texture coordinates and similar. Sampler states are created by calling @ref bs::SamplerState::create "SamplerState::create", while previously filling out the @ref bs::SAMPLER_STATE_DESC "SAMPLER_STATE_DESC" structure.

In most cases you don't need to set sampler states as the default one should be adequate. Shaders can also read textures without samplers by directly accessing their pixels, but this is not wanted for normal rendering as it ruins image quality due to the lack of filtering and it may not be as performance efficient. 

As an example, lets set up a sampler state that enables trilinear filtering for a texture using it, and then assign it to a material. 

~~~~~~~~~~~~~{.cpp}
SAMPLER_STATE_DESC desc;
desc.minFilter = FO_LINEAR;
desc.magFilter = FO_LINEAR;
desc.mipFilter = FO_LINEAR;

SPtr<SamplerState> samplerState = SamplerState::create(desc);

// "gAlbedoSamp" is a sampler state parameter provided by the standard shader we
// used in the example above. It controls options for the texture set on the gAlbedoTex
// parameter.
material->setSamplerState("gAlbedoSamp", samplerState);
~~~~~~~~~~~~~

# Using a material for rendering
Material is used by attaching it to a **Renderable** component. We will cover them in the next chapter.