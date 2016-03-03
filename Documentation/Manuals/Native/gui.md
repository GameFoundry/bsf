Creating custom GUI elements						{#customGUI}
===============
[TOC]

Sometimes the built-in GUI elements are just not enough, for example when you need a very specific GUI element for your game (e.g. for displaying graphs, charts), want to use a special material or in general just need some more customization options. While in some cases you could construct such elements from the provided GUI elements, it is often more efficient to create a brand new customized type. New GUI elements can be created relatively easily and allow you to fully customize the look and interaction with your element.

Before staring you should familiarize yourself how Banshee's GUI works from the users perspective. Make sure to read [GUI manual](TODOLINK) to understand the basics (although the manual is for scripting, the C++ versions of these methods are essentially equivalent and the same concepts apply).

All GUI elements derive from the base GUIElementBase type. The elements can be categorized into two major groups:
 - GUILayout: They derive from GUILayout and do not have any graphics, but instead they control the placement of all elements attached to them.
 - GUIElement: These are your standard GUI elements like buttons, input boxes, sliders and such.
 
You will almost certainly be interested in GUIElement, and are unlikely to need to create custom GUILayout types.

# Basic GUI element # {#customGUI_a}
When creating a custom GUI element you will need to override the GUIElement type. The minimum set of methods you need to implement for your custom elements are:
 - _getOptimalSize() - Return the optimal size of your GUI element. This is used for controlling the size of the element in a layout (if it's not fixed). For example if your element was displaying a texture 64x64 in size, then the optimal size should probably return 64x64. If you element is displaying text you can use GUIHelper to help you calculate the bounds of the text. If displaying something else then it's up to you to determine what constitutes an optimal size.
 - _getNumRenderElements() - Return the number of separate elements that your GUIElement consists of. Each element has its own mesh, material and texture. In most cases there is only one element.
 - _getNumQuads() - Returns the number of quads for the mesh of the render element at the specified index (e.g. one quad if the GUI element displays just one texture). This allows external systems how big of a buffer to allocate for element's mesh.
 - _fillBuffer() - This is the "meat" of the GUIElement override. It allows you to fill a buffer with vertices, uv coordinates and indices for a mesh for the specified render element. This allows you to fully control the look of your GUI element by providing customized geometry.
 - _getMaterial() - Here you should return a material that will be applied to the mesh output in the previous method. You can choose from a few pre-built material types and provide a texture and a color. If you wish to add a new custom material, check later in this guide.
 - updateRenderElementsInternal() - Called whenever the element's size or style changes. In this method you should rebuild the element's mesh. (While you could build the mesh in _fillBuffer it is inefficient as _fillBuffer will get called much more often than updateRenderElementsInternal due to mesh batching).
 
In order to help you with creating GUI meshes and materials make sure to take a look at ImageSprite and TextSprite classes. ImageSprite can easily generate image geometry of specified size, whether a simple quad or a Scale9grid image. And TextSprite will take a text string, font and additional options as input, and output a set of quads required for text rendering.

When constructing the GUIElement you will also need to provide a style-name (see next chapter) and dimensions to the GUIElement constructor. The dimensions determine the initial size of the element. To use the default dimensions just provide the constructor with return value from GUIDimensions::create(). GUIElement will internally use dimensions provided by its style, but providing custom GUIDimensions allows you to override the dimensions in the style. For that reason you should provide the user with a constructor that accepts GUIOptions object, which will contain optional dimension overrides the user can specify. Then you can create a GUIDimension by providing the GUIOptions object as a parameter.

It is highly suggested you check out an implementation of GUITexture (in BsGUITexture.cpp) for a simple implementation of everything mentioned.

# Style # {#customGUI_b}

Every GUIElement has an access to a specific style (GUIElementStyle). You can access the style by calling _getStyle(). The style controls the look of your elements, including its texture, text style and margins. When creating a custom GUIElement you normally don't need to worry about style options like margins or padding, but you should take the rest into account when creating your mesh & material.

You might also need to override styleUpdated() method that will trigger when the user changes the GUIElement's style, depending on your needs. Although in most cases it will be not necessary as a style change will trigger a call to updateRenderElementsInternal() anyway.

You should also provide a style-name for your element. This is a string that will be used for looking up the default style for the element, and you must provide it to the GUIElement constructor. You should also allow the user to provide a custom style name if needed (e.g. sometimes its nice to be able to style a GUIToggle element so it looks like a GUIButton).

See the GUIWidget chapter for more information about styles.

# Input # {#customGUI_c}

If you implemented everything so far you should have a functional GUIElement that has a custom look, but it still doesn't accept any input. In order to accept input you must override some or all of the following methods:
 - _mouseEvent - Triggered when the mouse moves and/or is clicked
 - _textInputEvent - Triggered when the user inputs some text on the keyboard
 - _virtualButtonEvent - Triggered when a certain virtual button is pressed (see [Input guide](TODOLINK) if you wish to know about virtual buttons)
 - _commandEvent - Triggers when some kind of a specialized event happens, like losing/gainging focus, deleting text, selecting text, forcing redraw and similar. See GUICommandEventType for an explanation of what each does.
 
Check out _mouseEvent implementation in GUIButtonBase for a simple implementation of a mouse hover on/off and click functionality. Check GUIInputBox for a more complex implementation of input.

If working with text input also take a look at GUIInputCaret and GUIInputSelection, as they help with input caret movement, and text selection. Instances of these classes can be accessed from GUIManager with getInputCaretTool() and getInputSelectionTool(). This is completely optional and you may choose to handle these manually. See GUIInputBox for example usage.

# Updating # {#customGUI_c}

If your element allows the user to update certain contents of it, you need to notify the GUI system so it knows when to rebuild the element. For example if you GUI element displays a text string you might provide a setText method that allows the user to change what is displayed. When that is done the GUI system will need to update the batched GUI mesh, update the GUI elements mesh and/or update the layout position/size of the element.

There are three ways to notify the GUI system the element is dirty, each more expensive in performance than the other:
 - _markMeshDirty() - Causes the batched GUI mesh to be re-assembled, will cause a call to GUIElement::_fillBuffer. Call this if element's size didn't change (e.g. when its position, or depth changes). Batched GUI mesh is a mesh managed by the GUI system that consists of multiple GUI elements sharing the same material properties.
 - _markContentDirty() - Has the same effect as _markMeshDirty(), but will also trigger an GUIElement::updateRenderElementsInternal() call so the GUI element mesh will be fully rebuilt. Call this when contents of the GUI element change, but not its size (e.g. changing a texture but it is the same dimensions as the old one.)
 - _markLayoutDirty() - Has the same effect as _markContentDirty(), but will also cause the layout system to recalculate the element's position and size. This can be very expensive as this will generally trigger layout updates for all siblings and children, potentially even parent GUI elements. Call this when element changes and no other dirty calls are appropriate.

When setting GUI contents consider using the provided GUIContent type, in order to stay consistent with the built-in elements. It allows you to specify an image, text and a tooltip all in one. But this is completely optional.
 
# Various # {#customGUI_d}
 
## GUIWidget ## {#customGUI_d_a}
 
Every GUIElement is internally part of a specific GUIWidget. A GUIWidget is a Component (CGUIWidget) that can be attached to a scene object, and transformed like one. This allows you to perform arbitrary transformations on GUI elements.

You attach elements to a GUIWidget by adding them to the widgets GUIPanel. Each widget has a GUIPanel which is a GUILayout object that allows you to manually position GUI elements within it. Every GUI element's top level parent must be such a panel or they will not be rendered.

GUIWidget also allows you to control to which camera should the GUI elements within it be rendered. The camera provides the dimensions of the widget, and in turn the available area for all child GUI elements.

Every style that a GUIElement uses is retrieved from its parent GUIWidget. GUIWidget contain a GUISkin object which has a key-value map where style-names are mapped to particular GUI element styles. Changing the GUISkin, or moving a GUIElement to a widget with a different style will trigger the GUI element rebuild, so it can apply the new style options.

## GUIManager ## {#customGUI_d_b}

GUIManager is the top level GUI manager that handles almost everything GUI related. It keeps tracks of all GUIWidgets and GUIElements. It updates layout when element properties change, rebuilds the GUI meshes, sets up GUI materials and ultimately renders the GUI. It also dispatches all GUI input events.

If you need to do some very deep changes to the GUI system this is where you do it.

And every GUIWidget has a GUISkin which contains a list of all the GUI element styles. The style for a specific element will depend on the GUIWidget it is assigned to, and the GUISkin assigned to the widget. (See below for information on GUIWidgets). 

## Text ##  {#customGUI_d_c}

If you ever require to render text completely manually (perhaps even outside of the GUI system), take a look at TextData. It will take a text string with a font and optional bounds as input, and output (optionally) word wrapped text in the form of words and lines, from which you can easily retrieve a mesh for rendering.
 
## Scripting integration ## {#customGUI_d_d}

Once you have a custom GUI element you might want to expose it to the scripting API. Take a look at the [scripting interop guide](TODOLINK) to learn how to create scripting API objects. 

Every GUI scripting interop object must implement TScriptGUIElement, but other than that creating GUI interop objects is the same as the general case described in the guide above. See ScriptGUITexture for an example.

Creating managed GUI objects is again the same as the general case. Take a look at the managed GUITexture.
