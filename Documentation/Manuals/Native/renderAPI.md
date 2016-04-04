Render API									{#renderAPI}
===============
[TOC]

Render API is an interface that allows you to perform low-level rendering operations akin to DirectX or OpenGL. In Banshee this API is provided through the @ref BansheeEngine::RenderAPI "RenderAPI" for the simulation thread, and @ref BansheeEngine::RenderAPICore "RenderAPICore" for the core thread. If you are confused by the dual nature of  objects, read the [core thread](@ref coreThread) manual. 

For the remainder of this manual we'll focus on the core thread portion of the API, but both provide essentially identical functionality, with the main difference that the simulation thread one expects a @ref BansheeEngine::CoreThreadAccessor "CoreAccessor" as input, and the execution of the actual commands is delayed.

Render API lets you manipulate the GPU pipeline by setting various states (depth stencil, blend and rasterizer), binding GPU programs, textures and executing draw calls. In this manual we'll cover how to use this API to perform rendering manually. 

To render something using this API you need to:
 - Create and bind a render target
  - Set up a viewport (if rendering to a part of a render target)
  - Clear render target (if we're re-using a render target)
 - Create and bind depth stencil, blend and/or rasterizer states (optionally)
 - Create and bind GPU programs
   - Create and bind vertex/fragment GPU program
   - Create and bind geometry/hull/domain GPU program (optionally)
 - Bind GPU program parameters (if any)
  - Bind textures
  - Bind samplers
  - Bind parameter buffers (better known constant/uniform buffers)
 - Create and bind vertices
  - Create and bind the vertex declaration
  - Create and bind the vertex buffer
 - Create and bind the index buffer (optionally)
 - Issue a draw command
 
We'll cover each step of this process, and at the end also show an alternate pipeline for compute operations.

# Render targets {#renderAPI_a}
Before any rendering can be done you must bind at least one render target. To learn more about, and how to create a render target check out the [render target](@ref renderTargets) manual. 

Binding a render target involves calling @ref BansheeEngine::RenderAPICore::setRenderTarget "RenderAPICore::setRenderTarget". This will cause any rendering to be output to the entirety of the render target. Optionally you can also call @ref BansheeEngine::RenderAPICore::setViewport "RenderAPICore::setViewport" to select a sub-rectangle of the target to render to. 

Binding a render target means you cannot use it for reading within a GPU program. However if your render target has a depth-buffer you can optionally set the `readOnlyDepthStencil` parameter of @ref BansheeEngine::RenderAPICore::setRenderTarget "RenderAPICore::setRenderTarget" to true, which will allow you to have a depth buffer be bound for both depth-testing and reading from the GPU program.
 
Before doing any rendering it's always good to clear the render target to some default value. Use @ref BansheeEngine::RenderAPICore::clearRenderTarget "RenderAPICore::clearRenderTarget" to clear the entire target, or @ref BansheeEngine::RenderAPICore::clearViewport "RenderAPICore::clearViewport" to clear just the viewport portion of the target. When clearing you can choose whether to clear color, depth or stencil buffers (or all) determined by @ref BansheeEngine::FrameBufferType "FrameBufferType" flags. You can also choose the values to clear each of the buffers to. And finally if your render target has multiple surfaces, you can choose to clear only some of the surfaces by providing a bitmask.

Once you are done rendering make sure to call @ref BansheeEngine::RenderAPICore::swapBuffers "RenderAPICore::swapBuffers" if your render target has multiple buffers (like a window). This will swap the buffers and present the rendered image on the screen.

A simple example covering all these commands:
~~~~~~~~~~~~~{.cpp}
SPtr<RenderTarget> myRt = ...; // Assuming we created this earlier, and its a double-buffered window

RenderAPICore& rapi = RenderAPICore::instance();
rapi.setRenderTarget(myRt);
rapi.setViewport(Rect2(0.5f, 0.0f, 0.5f, 1.0f)); // Draw only to the right side of the target

// Clear all buffers: color to white, depth to 1.0, stencil to 0
rapi.clearViewport(FBT_COLOR | FBT_DEPTH | FBT_STENCIL, Color::White, 1.0f, 0);
... execute some draw calls ...
rapi.swapBuffers();
~~~~~~~~~~~~~
 
# Pipeline states {#renderAPI_b}
Before executing the drawing operation you can optionally set up depth-stencil, rasterizer or blend states. If you do not set them, the default values will be used.

Use @ref BansheeEngine::RenderAPICore::setDepthStencilState "RenderAPICore::setDepthStencilState" to set a @ref BansheeEngine::DepthStencilStateCore "DepthStencilState".
Use @ref BansheeEngine::RenderAPICore::setBlendState "RenderAPICore::setBlendState" to set a @ref BansheeEngine::BlendStateCore "BlendState".
Use @ref BansheeEngine::RenderAPICore::setRasterizerState "RenderAPICore::setRasterizerState" to set a @ref BansheeEngine::RasterizerStateCore "RasterizerState".

We won't explain what each of the states does. For that you can check out the class documentation of the states themselves, or familiarize yourself with the modern GPU pipeline in general, as the states mirror it exactly.

# GPU programs {#renderAPI_c}

## GPU program parameters {#renderAPI_c_a}

# Vertices {#renderAPI_d}

# Indices {#renderAPI_e}

# Drawing {#renderAPI_f}
 
# Compute {#renderAPI_g}
TODO - Explain compute pipeline