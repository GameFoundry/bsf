---
title: Advanced materials
---

We have already talked about how to create materials, assign them parameters and bind them to a **Renderable** component. In this section we'll show an advanced way to assign material parameters, how to use materials for rendering directly (without using **Renderable** component) and how to create your own shaders without the use of BSL. 

# Material parameters
Previously we have shown to how to set **Material** parameters by calling methods like @bs::Material::setTexture, @bs::Material::setFloat, @bs::Material::setColor, @bs::Material::setVec4 and similar.

As an alternative you can also set materials through material parameter handles. Once a material handle is retrieved it allows you to set material parameters much more efficiently than by calling the methods above directly. 

To retrieve the handles call any of the following methods, depending on material parameter type:
 - @bs::ct::Material::getParamTexture - Outputs a @bs::TMaterialParamTexture<Core> handle that can be used for reading & writing the parameter value.
 - @bs::ct::Material::getParamLoadStoreTexture - Outputs a @bs::TMaterialParamLoadStoreTexture<Core> handle that can be used for reading & writing the parameter value.
 - @bs::ct::Material::getParamBuffer - Outputs a @bs::TMaterialParamBuffer<Core> handle that can be used for reading & writing the parameter value.
 - @bs::ct::Material::getParamSamplerState - Outputs a @bs::TMaterialParamSampState<Core> handle that can be used for reading & writing the parameter value.
 - @bs::ct::Material::getParamFloat - Outputs a @MaterialParamFloat handle that can be used for reading & writing the parameter value.
 - @bs::ct::Material::getParamVec2 - Outputs a @MaterialParamVec2 handle that can be used for reading & writing the parameter value.
 - @bs::ct::Material::getParamVec3 - Outputs a @MaterialParamVec3 handle that can be used for reading & writing the parameter value.
 - @bs::ct::Material::getParamVec4 - Outputs a @MaterialParamVec4 handle that can be used for reading & writing the parameter value.
 - @bs::ct::Material::getParamColor - Outputs a @MaterialParamColor handle that can be used for reading & writing the parameter value.
 - @bs::ct::Material::getParamMat3 - Outputs a @MaterialParamMat3 handle that can be used for reading & writing the parameter value.
 - @bs::ct::Material::getParamMat4 - Outputs a @MaterialParamMat4 handle that can be used for reading & writing the parameter value.
 - @bs::ct::Material::getParamFloatCurve - Outputs a @MaterialParamFloatCurve handle that can be used for assigning an animation curve to a *float* parameter. This can be used as an alternative to **ct::Material::getParamFloat()** in that the value will now be animated over the range of the curve, instead of being just a static value.
 - @bs::ct::Material::getParamColorGradient - Outputs a @bs::TMaterialColorGradientParam<Core> handle that can be used for assigning a color gradient to a *Color* parameter. This can be used as an alternative to **ct::Material::getParamColor()** in that the value will now be animated over the range of the gradient, instead of being just a static value.
 - @bs::ct::Material::getParamSpriteTexture - Outputs a @bs::TMaterialParamSpriteTexture<Core> handle that can be used for assigning a sprite texture to a *texture* parameter. This can be used as an alternative to **ct::Material::getParamTexture()** in that the sprite texture can be animated while a normal texture is always static.
 
Handles provide **set()** and **get()** methods that can be used for writing and reading the parameter values. 
 
~~~~~~~~~~~~~{.cpp}
HMaterial material = ...;

MaterialParamMat4 myMatParam;
MaterialParamTexture myTextureParam;

material->getParamMat4("vertProjMatrix", myMatParam);
material->getParamTexture("mainTexture", myTextureParam);

Matrix4 viewProjMat = ...;
SPtr<Texture> someTexture = ...;

myVectorParam.set(viewProjMat);
myTextureParam.set(someTexture);
~~~~~~~~~~~~~ 
 
Material handles are very similar as **GpuParams** handles we talked about earlier. There are three major differences:
 - **GpuParams** handles will only set the parameter value for a specific **GpuProgram**, while material handles will set the values for all **GpuProgram**%s that map to that handle.
 - **GpuProgram** parameters are retrieved directly from program source code, while **Material** parameters need to be explicitly defined in the **Shader** (shown below). **Material** parameters always map to one or multiple **GpuProgram** parameters. 
 - **GpuProgram** parameters do not support animated parameter types.

# Creating a shader manually
So far when we wanted to create a shader we would create a BSL file which would then be imported, creating a @bs::Shader. But you can also create shaders manually by explicitly providing HLSL/GLSL code for **GpuProgram**%s and non-programmable states. Most of the things outlined in this section are performed by BSL compiler internally when a **Shader** is imported.

Each shader definition contains two things:
 - A list of parameters, with a mapping of each parameter to one or multiple variables in a GPU program
 - One or multiple @bs::Technique%s. Each technique is essentially a fully fledged shader of its own. Techniques are chosen by the renderer depending on the context. For example some techniques only support the DirectX backend, while others only the Vulkan backend. Different techniques will also exist for different shader variations (e.g. a high- and low-end version of the same shader).
  - Each technique contains one or multiple @bs::Pass%es. A pass is a set of GPU programs and non-programmable states. When rendering using a certain technique each pass will be executed one after another. This allows you to render objects that require more complex rendering that requires multiple separate steps - althrough in practice most techniques have only a single pass.

To summarize, the relationship between materials, shaders, techniques and passes is:
 - **Material** [contains one]-> **Shader** [contains one or multiple]-> **Technique** [contains one or multiple]-> **Pass**
 
Optionally, the shader definition can also contain a set of @bs::SubShader objects. As explained previously sub-shaders are a set of techniques that are used to override specific renderer behaviour. 
 
## Creating a pass
A **Pass** can be created by filling out a @bs::PASS_DESC descriptor and passing it to @bs::Pass::create method. **PASS_DESC** is fairly simple and it expects a set of GPU program and non-programmable state descriptors.

~~~~~~~~~~~~~{.cpp}
GPU_PROGRAM_DESC vertexProgDesc = ...;
GPU_PROGRAM_DESC fragmentProgDesc = ...;

BLEND_STATE_DESC blendStateDesc = ...;

// Create a pass with a vertex and fragment program, and a non-default blend state
PASS_DESC desc;
desc.vertexProgram = vertexProgDesc;
desc.fragmentProg = fragmentProgDesc;
desc.blendState = blendStateDesc;

SPtr<Pass> pass = Pass::create(desc);
~~~~~~~~~~~~~

The descriptors for GPU programs and non-programmable states are filled out as described in the low-level rendering API manual.

## Creating a technique
Now that we know how to create a pass, we can use one or multiple passes to initialize a **Technique**. A technique is just a container for one or multiple passes.

To create a technique call @bs::Technique::create and provide it with:
 - Shading language name - This should be "HLSL" or "GLSL". The engine will not use this technique unless this language is supported by the current render API. 
 - Array containing one or multiple passes
 
For example:
~~~~~~~~~~~~~{.cpp}
SPtr<Pass> pass = ...;

// Create a technique that uses HLSL and has a single pass
SPtr<Technique> technique = Technique::create("HLSL", { pass });
~~~~~~~~~~~~~
  
## Creating a shader
Now that we have a technique we can create the shader by calling @bs::Shader::create, which expects the name of the shader and a @bs::TSHADER_DESC<T> structure as input.
	
@bs::TSHADER_DESC<T> contains a list of techniques to initialize the shader with, as well as a set of optional parameters to control how the shader rendering works:
  - @bs::TSHADER_DESC<T>::queueSortType - Controls how should objects rendered with this shader be sorted. Either front to back, back to front, or without sorting. This property can be used for the active renderer to properly render objects, as certain effects (like transparency) require their objects to be rendered in a specific order.
  - @bs::TSHADER_DESC<T>::queuePriority - Controls at what point should objects rendered with this shader be rendered relative to other objects. Objects with higher priority will be rendered before ones with lower priority, and sorting due to **SHADER_DESC::queueSortType** will never sort outside of priority groups. This can allow you to render a certain type of objects before all others (e.g. all opaque objects should be rendered before transparent ones).
  - @bs::TSHADER_DESC<T>::separablePasses - An optimization hint to the renderer that can improve performance when turned on. Only relevant if the shader has techniques with multiple passes. When true the renderer will not necessarily execute passes right after another, but might render other objects in-between passes. This can reduce state switching as multiple objects can be rendered with a single pass, but is only relevant for algorithms that can handle such a process (most can't).

~~~~~~~~~~~~~{.cpp}
SPtr<Technique> technique = ...;
	
SHADER_DESC desc;
desc.queueSortType = QueueSortType::None;
desc.queuePriority = 0;
desc.separablePasses = false;
desc.techniques = { technique };

SPtr<Shader> shader = Shader::create("MyShader", desc);
~~~~~~~~~~~~~ 
  
## Shader parameters
Shader parameters allow you to change values of parameters in GPU programs through the **Material** interface. They are similar to GPU program parameters described earlier, but they set the values on all GPU programs on all passes in the active technique, instead of doing so only on a single GPU program. Additionally they also support renderer semantics (see below).

To create the parameter interface you must populate the **SHADER_DESC** structure by calling one of the @bs::TSHADER_DESC<T>::addParameter overloads.

Parameters come in two variants:
 - Data - These are primitive types like float, int or bool. This includes their vector and array variants. Check @bs::GpuParamDataType for a list of all data parameter types. In **SHADER_DESC** they are represented with the @bs::SHADER_DATA_PARAM_DESC type.
 - Object - These are object types like texture, buffer or sampler state. Check @bs::GpuParamObjectType for a list of all object parameter types. In **SHADER_DESC** they are represented with the @bs::SHADER_OBJECT_PARAM_DESC type.

For each parameter you must specify:
 - Its name. This will be the name accessible through **Material**. It can be anything you like, as long as it is unique.
 - Name of the GPU variable it maps to. This must be a variable defined in source code of one or multiple GPU programs used in the shader (across all techniques/passes).
 - Type of the GPU variable, as described above.

~~~~~~~~~~~~~{.cpp}
// Extended example from above
SPtr<Technique> technique = ...;
	
SHADER_DESC desc;
desc.queueSortType = QueueSortType::None;
desc.queuePriority = 0;
desc.separablePasses = false;
desc.techniques = { technique };

// Add a 4x4 transform matrix data parameter
desc.addParameter(SHADER_DATA_PARAM_DESC("WorldTfrm", "WorldTfrm", GPDT_MATRIX_4X4));

// Add a texture parameter
desc.addParameter(SHADER_OBJECT_PARAM_DESC("AlbedoTex", "AlbedoTex", GPOT_TEXTURE2D));

SPtr<Shader> shader = Shader::create("MyShader", desc);
~~~~~~~~~~~~~

### Advanced parameters
When adding parameters you can also specify two additional properties we didn't touch on in the previous section: renderer semantic and default value.

Renderer semantic allows you to give the parameter a unique tag that can be recognized by the active renderer. The renderer can then use these semantics to automatically assign values to them while rendering. For example the "WVP" semantic might notify the renderer to populate this parameter with the world-view-projection matrix. This way the user is not responsible for setting such parameters manually. The actual semantics supported depend on the active renderer. If provided and renderer doesn't support a semantic, it will be ignored. We'll talk more on how to access semantics in the renderer manual.

The parameter default value allows you to provide a value that will be used for initializing the parameter when a **Material** is initially constructed. For data parameters the default value is a provided as a raw block of memory, and for object parameters it can be a reference to a **Texture** or a **SamplerState**.

~~~~~~~~~~~~~{.cpp}
// An extended example from above with semantics and default values:
SPtr<Technique> technique = ...;
	
SHADER_DESC desc;
desc.queueSortType = QueueSortType::None;
desc.queuePriority = 0;
desc.separablePasses = false;
desc.techniques = { technique };

// Add a 4x4 transform matrix data parameter with a "W" semantic and identity matrix as default
desc.addParameter(SHADER_DATA_PARAM_DESC("WorldTfrm", "WorldTfrm", GPDT_MATRIX_4X4, "W"), &Matrix4::Identity);

// Add a texture parameter with an "Albedo" semantic and a white texture as default
desc.addParameter(SHADER_OBJECT_PARAM_DESC("AlbedoTex", "AlbedoTex", GPOT_TEXTURE2D, "Albedo"), Texture::White);

HShader shader = Shader::create("MyShader", desc);
~~~~~~~~~~~~~  
  
# Manually rendering using the material
In an earlier manual we have shown how to render using a **Material** by attaching it to a **Renderable** component and letting the renderer do the rest. You can however render using the material completely manually, using the low-level rendering API.

Material is a **CoreObject** which means it also has a core-thread interface accessible through @bs::Material::getCore(). The interface is the same as the non-core interface we have described so far.

## Binding material
**Material** cannot be bound directly to the low level rendering API. You must instead manually retrieve a pipeline state for one of its passes.

You can retrieve a specific pass from a material by calling @bs::Material::getPass(). The method expects an index of a technique and an index of a pass. To get the number of supported techniques call @bs::Material::getNumTechniques(), and to get the number of passes for a specific technique call @bs::Material::getNumPasses() with a specific technique index.

Once you have a **Pass** you can retrieve from it either a **GraphicsPipelineState** or a **ComputePipelineState** by calling @bs::Pass::getGraphicsPipelineState() and @bs::Pass::getComputePipelineState(), respectively. Those can then be bound for rendering as shown in the low level rendering API manual.

~~~~~~~~~~~~~{.cpp}
SPtr<Material> material = ...;

int passIdx = 0;
int techniqueIdx = 0;
SPtr<Pass> pass = material->getPass(passIdx, techniqueIdx);

RenderAPI& rapi = RenderAPI::instance();
rapi.setGraphicsPipeline(pass->getGraphicsPipelineState());
~~~~~~~~~~~~~

Alternatively you can use the helper methods @bs::ct::RendererUtility::setPass or @bs::ct::RendererUtility::setComputePass.

## Binding material parameters
In order to bind material parameters we need to somehow get access to a **GpuParams** object from the material. This is done through an intermediate class @bs::GpuParamsSet, created by a call to @bs::Material::createParamsSet(), which as a parameter takes a technique index.

**GpuParams** for a specific pass can then be retrieved by calling @bs::GpuParamsSet::getGpuParams() with the pass index. They can then be bound as described in the low level render API manual.

~~~~~~~~~~~~~{.cpp}
SPtr<Material> material = ...;

int passIdx = 0;
int techniqueIdx = 0;
SPtr<GpuParamsSet> paramsSet = material->createParamsSet(techniqueIdx);

RenderAPI& rapi = RenderAPI::instance();
rapi.setGpuParams(paramsSet->getGpuParams(passIdx));
~~~~~~~~~~~~~

Note that creation of a **GpuParamsSet** object is expensive, and the intent is that it will be created once (or just a few times) per material. **GpuParamsSet** contains a completely separate storage from the **Material** it was created from, therefore whenever material parameters are updated you must transfer its contents into **GpuParams** by calling @bs::GpuParamsSet::update().

~~~~~~~~~~~~~{.cpp}
SPtr<GpuParamsSet> paramsSet = material->createParamsSet(techniqueIdx);

// ...update some parameters on the material...

// Transfer the updated data
paramsSet->update(material);
~~~~~~~~~~~~~

In case your material contains any animated properties like animation curves, color gradients or color textures, you can also provide a `time` parameter to **GpuParamsSet::update()**, which determines the point at which to sample animated properties.

~~~~~~~~~~~~~{.cpp}
// Sample animated properties at 0.5 seconds into the animation
paramsSet->update(material, 0.5f);
~~~~~~~~~~~~~

Once both the material's pipeline state and parameters are bound, you can then proceed to render as normally, as described in the low-level rendering manual.
