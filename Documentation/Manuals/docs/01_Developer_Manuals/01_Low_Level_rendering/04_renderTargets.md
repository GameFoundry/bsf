---
title: Render targets
---

Before we can actually render an object, we must also specify a render target which will serve as a surface to render the output to. We already covered render target creation in [windows](User_Manuals/Advanced_Rendering/windows) and [offscreen rendering](User_Manuals/Advanced_Rendering/offscreenRendering) manuals. The only difference is that in those manuals we used a **Camera** component to set the target, and relied on the renderer to make use of it. 

In this chapter we'll show how to manually bind a render target for rendering, as well as some other render target related operations.

# Binding
Use @bs::ct::RenderAPI::setRenderTarget to bind a render target for rendering.

~~~~~~~~~~~~~{.cpp}
SPtr<RenderTarget> target = ...; // Create a RenderTexture or RenderWindow as described in earlier chapters

RenderAPI& rapi = RenderAPI::instance();
rapi.setRenderTarget(target);
~~~~~~~~~~~~~

This will bind the entirety of the render target surface for rendering. If you just want to render to a portion of the target you can also call @bs::ct::RenderAPI::setViewport. It accepts a 2D rectangle whose coordinates should be in [0, 1] range and it specifies in which portion of the render target should rendering occurr.

~~~~~~~~~~~~~{.cpp}
// Render to the center of the render target, at 50% of its size
RenderAPI& rapi = RenderAPI::instance();
rapi.setViewport(Rect2(0.25f, 0.25f, 0.5f, 0.5f));
~~~~~~~~~~~~~

## Advanced binding
**ct::RenderAPI::setRenderTarget()** also has a couple of parameters to control more advanced behaviour:
 - `readOnlyFlags` - Combination of one or more elements of @bs::FrameBufferType denoting which buffers will be bound for read-only operations. This is useful for depth or stencil buffers which need to be bound both for depth/stencil tests, as well as shader reads. If you don't specify this the render backend will assume you will be writing to the render target which will result in undefined behaviour if you also try reading from that same texture.
 - `loadMask` - Mask described by @bs::RenderSurfaceMaskBits which controls if current contents of any of the render target surfaces should be preserved. By default the system doesn't guarantee the contents will be preserved and data is instead undefined. In certain cases (like blending operations) you want to preserve the contents, in which case specify the necessary flags to tell the system which surfaces need their contents preserved.

~~~~~~~~~~~~~{.cpp}
// Bind a render target with read-only depth/stencil, and preserve the existing contents of depth-stencil buffer on bind
RenderAPI& rapi = RenderAPI::instance();
rapi.setRenderTarget(target, FBT_DEPTH | FBT_STENCIL, RT_DEPTH_STENCIL);
~~~~~~~~~~~~~
 
# Clearing
Usually a render target will be re-used many times. Unless you are sure that every use will completely overwrite the render target contents, it can be beneficial (and in some cases necessary) to clear the render target to some value. Call @bs::ct::RenderAPI::clearRenderTarget to clear the currently bound render target. 

The first parameter represents a **FrameBufferType** of which portions of the target to clear. Second, third and fourth parameters represent the clear values for the color, depth and stencil surfaces, respectively. In case you want to clear only a specific color surface (in case they are multiple), you can use the fifth parameter as a bitmask of which color surfaces to clear.

~~~~~~~~~~~~~{.cpp}
// Clear color and depth surfaces. All color surfaces are cleared to blue color, while depth is cleared to the value of 1
RenderAPI& rapi = RenderAPI::instance();
rapi.clearRenderTarget(FBT_COLOR | FBT_DEPTH, Color::Blue, 1, 0, 0xFF);
~~~~~~~~~~~~~

You can also call @bs::ct::RenderAPI::clearViewport to clear only the viewport portion of the render target. The parameters are identical to **ct::RenderAPI::clearRenderTarget()**.

Clearing the depth buffer is especially important as the GPU will read its contents during rendering, and having old data in the depth buffer pretty much guarantees your rendering will not be valid.

# Swapping
If a render target is a **RenderWindow** you must call @bs::ct::RenderAPI::swapBuffers after rendering. This is because windows are usually double or triple buffered, meaning the rendering happens to a hidden buffer invisible to the user. When **ct::RenderAPI::swapBuffers()** is called this hidden buffer is presented to the user.

~~~~~~~~~~~~~{.cpp}
SPtr<RenderWindow> window;
rapi.setRenderTarget(window);

// ... draw something ...

rapi.swapBuffers(window);
~~~~~~~~~~~~~
