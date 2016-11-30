Materials 						{#simpleMaterial}
===============

Materials are objects that control how are objects rendered. They are represented using the @ref bs::Material "Material" class. For example, if you wish to render a mesh with a texture, you must create a material that supports textures, bind the texture to the material and bind both to a **Renderable** which will then render the mesh using the supplied material.

Internally material contains a **Shader** and a set of parameters. A **Shader** object must be provided on material contruction, while parameters can be set during program execution.

A shader is a group of GPU programs and render states that fully control the rendering of an object. Shader also exposes a set of parameters the user can change in order to control the operation of the GPU programs. Which parametes are available is controlled by the GPU programs themselves. We will delay further discussion of shaders and GPU programs until later, and for examples in this chapter we will use one of the shaders built into Banshee.

The parameters exposed by the shader can be modified by the interface provided by the material - in fact the main task of the **Material** class is to provide methods for setting those parameters. Essentially materials can be thought of as shader "instances", and shaders can be tought of material "templates". There is always a one shader object (per type), and multiple material instances of that shader, each with different parameters. 

# Material creation
To create a material use the @ref bs::Material::create "Material::create" method, which expects a **Shader** as a parameter.

~~~~~~~~~~~~~{.cpp}
HShader shader = gBuiltinResources().getBuiltinShader(BuiltinShader::Standard);
HMaterial material = Material::create(shader);
~~~~~~~~~~~~~

# Setting parameters

// TODO


