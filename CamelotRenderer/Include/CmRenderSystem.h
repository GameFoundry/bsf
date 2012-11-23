/*
-----------------------------------------------------------------------------
This source file is part of OGRE
(Object-oriented Graphics Rendering Engine)
For the latest info, see http://www.ogre3d.org

Copyright (c) 2000-2011 Torus Knot Software Ltd

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
-----------------------------------------------------------------------------
*/
#ifndef __RenderSystem_H_
#define __RenderSystem_H_

// Precompiler options
#include "CmPrerequisites.h"

#include <memory>

#include "CmString.h"

#include "CmSamplerState.h"
#include "CmCommon.h"

#include "CmRenderOperation.h"
#include "CmRenderSystemCapabilities.h"
#include "CmRenderTarget.h"
#include "CmRenderTexture.h"
#include "CmGpuProgram.h"
#include "CmPlane.h"

namespace CamelotEngine
{
	/** \addtogroup Core
	*  @{
	*/
	/** \addtogroup RenderSystem
	*  @{
	*/

	typedef map< String, RenderTarget * >::type RenderTargetMap;
	typedef multimap<UINT8, RenderTarget * >::type RenderTargetPriorityMap;

	class TextureManager;
	/// Enum describing the ways to generate texture coordinates
	enum TexCoordCalcMethod
	{
		/// No calculated texture coordinates
		TEXCALC_NONE,
		/// Environment map based on vertex normals
		TEXCALC_ENVIRONMENT_MAP,
		/// Environment map based on vertex positions
		TEXCALC_ENVIRONMENT_MAP_PLANAR,
		TEXCALC_ENVIRONMENT_MAP_REFLECTION,
		TEXCALC_ENVIRONMENT_MAP_NORMAL,
		/// Projective texture
		TEXCALC_PROJECTIVE_TEXTURE
	};
	/// Enum describing the various actions which can be taken onthe stencil buffer
	enum StencilOperation
	{
		/// Leave the stencil buffer unchanged
		SOP_KEEP,
		/// Set the stencil value to zero
		SOP_ZERO,
		/// Set the stencil value to the reference value
		SOP_REPLACE,
		/// Increase the stencil value by 1, clamping at the maximum value
		SOP_INCREMENT,
		/// Decrease the stencil value by 1, clamping at 0
		SOP_DECREMENT,
		/// Increase the stencil value by 1, wrapping back to 0 when incrementing the maximum value
		SOP_INCREMENT_WRAP,
		/// Decrease the stencil value by 1, wrapping when decrementing 0
		SOP_DECREMENT_WRAP,
		/// Invert the bits of the stencil buffer
		SOP_INVERT
	};


	/** Defines the functionality of a 3D API
	@remarks
	The RenderSystem class provides a base interface
	which abstracts the general functionality of the 3D API
	e.g. Direct3D or OpenGL. Whilst a few of the general
	methods have implementations, most of this class is
	abstract, requiring a subclass based on a specific API
	to be constructed to provide the full functionality.
	Note there are 2 levels to the interface - one which
	will be used often by the caller of the Ogre library,
	and one which is at a lower level and will be used by the
	other classes provided by Ogre. These lower level
	methods are prefixed with '_' to differentiate them.
	The advanced user of the library may use these lower
	level methods to access the 3D API at a more fundamental
	level (dealing direct with render states and rendering
	primitives), but still benefiting from Ogre's abstraction
	of exactly which 3D API is in use.
	@author
	Steven Streeting
	@version
	1.0
	*/
	class CM_EXPORT RenderSystem
	{
	public:
		/** Default Constructor.
		*/
		RenderSystem();

		/** Destructor.
		*/
		virtual ~RenderSystem();

		/** Returns the name of the rendering system.
		*/
		virtual const String& getName(void) const = 0;

		/** Sets an option for this API
		@remarks
		Used to confirm the settings (normally chosen by the user) in
		order to make the renderer able to initialise with the settings as required.
		This may be video mode, D3D driver, full screen / windowed etc.
		Called automatically by the default configuration
		dialog, and by the restoration of saved settings.
		These settings are stored and only activated when
		RenderSystem::initialise or RenderSystem::reinitialise
		are called.
		@par
		If using a custom configuration dialog, it is advised that the
		caller calls RenderSystem::getConfigOptions
		again, since some options can alter resulting from a selection.
		@param
		name The name of the option to alter.
		@param
		value The value to set the option to.
		*/
		virtual void setConfigOption(const String &name, const String &value) = 0;

		/** Validates the options set for the rendering system, returning a message if there are problems.
		@note
		If the returned string is empty, there are no problems.
		*/
		virtual String validateConfigOptions(void) = 0;

		/** Start up the renderer using the settings selected (Or the defaults if none have been selected).
		@remarks
		Called by Root::setRenderSystem. Shouldn't really be called
		directly, although  this can be done if the app wants to.
		@param
		autoCreateWindow If true, creates a render window
		automatically, based on settings chosen so far. This saves
		an extra call to _createRenderWindow
		for the main render window.
		@par
		If an application has more specific window requirements,
		however (e.g. a level design app), it should specify false
		for this parameter and do it manually.
		@returns
		A pointer to the automatically created window, if requested, otherwise null.
		*/
		virtual RenderWindow* _initialise(bool autoCreateWindow, const String& windowTitle = "Camelot Render Window");


		/** Query the real capabilities of the GPU and driver in the RenderSystem*/
		virtual RenderSystemCapabilities* createRenderSystemCapabilities() const = 0;

		/** Force the render system to use the special capabilities. Can only be called
		*    before the render system has been fully initializer (before createWindow is called) 
		*	@param
		*		 capabilities has to be a subset of the real capabilities and the caller is 
		*		 responsible for deallocating capabilities.
		*/
		virtual void useCustomRenderSystemCapabilities(RenderSystemCapabilities* capabilities);

		/** Restart the renderer (normally following a change in settings).
		*/
		virtual void reinitialise(void) = 0;

		/** Shutdown the renderer and cleanup resources.
		*/
		virtual void shutdown(void);

		/** Creates a new rendering window.
		@remarks
		This method creates a new rendering window as specified
		by the paramteters. The rendering system could be
		responible for only a single window (e.g. in the case
		of a game), or could be in charge of multiple ones (in the
		case of a level editor). The option to create the window
		as a child of another is therefore given.
		This method will create an appropriate subclass of
		RenderWindow depending on the API and platform implementation.
		@par
		After creation, this window can be retrieved using getRenderTarget().
		@param
		name The name of the window. Used in other methods
		later like setRenderTarget and getRenderTarget.
		@param
		width The width of the new window.
		@param
		height The height of the new window.
		@param
		fullScreen Specify true to make the window full screen
		without borders, title bar or menu bar.
		@param
		miscParams A NameValuePairList describing the other parameters for the new rendering window. 
		Options are case sensitive. Unrecognised parameters will be ignored silently.
		These values might be platform dependent, but these are present for all platforms unless
		indicated otherwise:
		<table>
		<tr>
			<td><b>Key</b></td>
			<td><b>Type/Values</b></td>
			<td><b>Default</b></td>
			<td><b>Description</b></td>
			<td><b>Notes</b></td>
		</tr>
		<tr>
			<td>title</td>
			<td>Any string</td>
			<td>RenderTarget name</td>
			<td>The title of the window that will appear in the title bar</td>
			<td>&nbsp;</td>
		</tr>
		<tr>
			<td>colourDepth</td>
			<td>16, 32</td>
			<td>Desktop depth</td>
			<td>Colour depth of the resulting rendering window; only applies if fullScreen</td>
			<td>Win32 Specific</td>
		</tr>
		<tr>
			<td>left</td>
			<td>Positive integers</td>
			<td>Centred</td>
			<td>Screen x coordinate from left</td>
			<td>&nbsp;</td>
		</tr>
		<tr>
			<td>top</td>
			<td>Positive integers</td>
			<td>Centred</td>
			<td>Screen y coordinate from left</td>
			<td>&nbsp;</td>
		</tr>
		<tr>
			<td>depthBuffer</td>
			<td>true, false</td>
			<td>true</td>
			<td>Use depth buffer</td>
			<td>DirectX9 specific</td>
		</tr>
		<tr>
			<td>externalWindowHandle</td>
			<td>Win32: HWND as integer<br/>
			    GLX: poslong:posint:poslong (display*:screen:windowHandle) or poslong:posint:poslong:poslong (display*:screen:windowHandle:XVisualInfo*)</td>
			<td>0 (none)</td>
			<td>External window handle, for embedding the OGRE render in an existing window</td>
			<td>&nbsp;</td>
		</tr>
		<tr>
			<td>externalGLControl</td>
			<td>true, false</td>
			<td>false</td>
			<td>Let the external window control OpenGL i.e. don't select a pixel format for the window,
			do not change v-sync and do not swap buffer. When set to true, the calling application
			is responsible of OpenGL initialization and buffer swapping. It should also create an
			OpenGL context for its own rendering, Ogre will create one for its use. Then the calling
			application must also enable Ogre OpenGL context before calling any Ogre function and
			restore its OpenGL context after these calls.</td>
			<td>OpenGL specific</td>
		</tr>
		<tr>
			<td>externalGLContext</td>
			<td>Context as Unsigned Long</td>
			<td>0 (create own context)</td>
			<td>Use an externally created GL context</td>
			<td>OpenGL Specific</td>
		</tr>
		<tr>
			<td>parentWindowHandle</td>
			<td>Win32: HWND as integer<br/>
			    GLX: poslong:posint:poslong (display*:screen:windowHandle) or poslong:posint:poslong:poslong (display*:screen:windowHandle:XVisualInfo*)</td>
			<td>0 (none)</td>
			<td>Parent window handle, for embedding the OGRE in a child of an external window</td>
			<td>&nbsp;</td>
		</tr>
		<tr>
			<td>macAPI</td>
			<td>String: "cocoa" or "carbon"</td>
			<td>"carbon"</td>
			<td>Specifies the type of rendering window on the Mac Platform.</td>
			<td>&nbsp;</td>
		 </tr>
		 <tr>
			<td>macAPICocoaUseNSView</td>
			<td>bool "true" or "false"</td>
			<td>"false"</td>
			<td>On the Mac platform the most diffused method to embed OGRE in a custom application is to use Interface Builder
				and add to the interface an instance of OgreView.
				The pointer to this instance is then used as "externalWindowHandle".
				However, there are cases where you are NOT using Interface Builder and you get the Cocoa NSView* of an existing interface.
				For example, this is happens when you want to render into a Java/AWT interface.
				In short, by setting this flag to "true" the Ogre::Root::createRenderWindow interprets the "externalWindowHandle" as a NSView*
				instead of an OgreView*. See OgreOSXCocoaView.h/mm.
			</td>
			<td>&nbsp;</td>
		 </tr>
         <tr>
             <td>contentScalingFactor</td>
             <td>Positive Float greater than 1.0</td>
             <td>The default content scaling factor of the screen</td>
             <td>Specifies the CAEAGLLayer content scaling factor.  Only supported on iOS 4 or greater.
                 This can be useful to limit the resolution of the OpenGL ES backing store.  For example, the iPhone 4's
                 native resolution is 960 x 640.  Windows are always 320 x 480, if you would like to limit the display
                 to 720 x 480, specify 1.5 as the scaling factor.
             </td>
             <td>&nbsp;</td>
		 </tr>
         <tr>
			<td>FSAA</td>
			<td>Positive integer (usually 0, 2, 4, 8, 16)</td>
			<td>0</td>
			<td>Full screen antialiasing factor</td>
			<td>&nbsp;</td>
		</tr>
		<tr>
			<td>FSAAHint</td>
			<td>Depends on RenderSystem and hardware. Currently supports:<br/>
			"Quality": on systems that have an option to prefer higher AA quality over speed, use it</td>
			<td>Blank</td>
			<td>Full screen antialiasing hint</td>
			<td>&nbsp;</td>
		</tr>
		<tr>
			<td>displayFrequency</td>
			<td>Refresh rate in Hertz (e.g. 60, 75, 100)</td>
			<td>Desktop vsync rate</td>
			<td>Display frequency rate, for fullscreen mode</td>
			<td>&nbsp;</td>
		</tr>
		<tr>
			<td>vsync</td>
			<td>true, false</td>
			<td>false</td>
			<td>Synchronize buffer swaps to monitor vsync, eliminating tearing at the expense of a fixed frame rate</td>
			<td>&nbsp;</td>
		</tr>
		<tr>
			<td>vsyncInterval</td>
			<td>1, 2, 3, 4</td>
			<td>1</td>
			<td>If vsync is enabled, the minimum number of vertical blanks that should occur between renders. 
			For example if vsync is enabled, the refresh rate is 60 and this is set to 2, then the
			frame rate will be locked at 30.</td>
			<td>&nbsp;</td>
		</tr>
		<tr>
			<td>border</td>
			<td>none, fixed, resize</td>
			<td>resize</td>
			<td>The type of window border (in windowed mode)</td>
			<td>&nbsp;</td>
		</tr>
		<tr>
			<td>outerDimensions</td>
			<td>true, false</td>
			<td>false</td>
			<td>Whether the width/height is expressed as the size of the 
			outer window, rather than the content area</td>
			<td>&nbsp;</td>
		</tr>
		<tr>
			<td>useNVPerfHUD</td>
			<td>true, false</td>
			<td>false</td>
			<td>Enable the use of nVidia NVPerfHUD</td>
			<td>&nbsp;</td>
		</tr>
		<tr>
			<td>gamma</td>
			<td>true, false</td>
			<td>false</td>
			<td>Enable hardware conversion from linear colour space to gamma
			colour space on rendering to the window.</td>
			<td>&nbsp;</td>
		</tr>
		*/
		virtual RenderWindow* createRenderWindow(const String &name, unsigned int width, unsigned int height, 
			bool fullScreen, const NameValuePairList *miscParams = 0) = 0;

		/** Creates multiple rendering windows.		
		@param
		renderWindowDescriptions Array of structures containing the descriptions of each render window.
		The structure's members are the same as the parameters of _createRenderWindow:
		* name
		* width
		* height
		* fullScreen
		* miscParams
		See _createRenderWindow for details about each member.		
		@param
		createdWindows This array will hold the created render windows.
		@returns
		true on success.		
		*/
		virtual bool _createRenderWindows(const RenderWindowDescriptionList& renderWindowDescriptions, 
			RenderWindowList& createdWindows);

		
		/**	Create a MultiRenderTarget, which is a render target that renders to multiple RenderTextures
		at once. Surfaces can be bound and unbound at will.
		This fails if mCapabilities->getNumMultiRenderTargets() is smaller than 2.
		*/
		virtual MultiRenderTarget * createMultiRenderTarget(const String & name) = 0; 

		/** Destroys a render window */
		virtual void destroyRenderWindow(const String& name);
		/** Destroys a render texture */
		virtual void destroyRenderTexture(const String& name);
		/** Destroys a render target of any sort */
		virtual void destroyRenderTarget(const String& name);

		/** Attaches the passed render target to the render system.
		*/
		virtual void attachRenderTarget( RenderTarget &target );
		/** Returns a pointer to the render target with the passed name, or NULL if that
		render target cannot be found.
		*/
		virtual RenderTarget * getRenderTarget( const String &name );
		/** Detaches the render target with the passed name from the render system and
		returns a pointer to it.
		@note
		If the render target cannot be found, NULL is returned.
		*/
		virtual RenderTarget * detachRenderTarget( const String &name );

		/// Iterator over RenderTargets
		typedef CamelotEngine::RenderTargetMap::iterator RenderTargetIterator;

		/** Returns a specialised MapIterator over all render targets attached to the RenderSystem. */
		virtual RenderTargetIterator getRenderTargetIterator(void) {
			return mRenderTargets.begin();
		}
		/** Returns a description of an error code.
		*/
		virtual String getErrorDescription(long errorNumber) const = 0;

		/** Defines whether or now fullscreen render windows wait for the vertical blank before flipping buffers.
		@remarks
		By default, all rendering windows wait for a vertical blank (when the CRT beam turns off briefly to move
		from the bottom right of the screen back to the top left) before flipping the screen buffers. This ensures
		that the image you see on the screen is steady. However it restricts the frame rate to the refresh rate of
		the monitor, and can slow the frame rate down. You can speed this up by not waiting for the blank, but
		this has the downside of introducing 'tearing' artefacts where part of the previous frame is still displayed
		as the buffers are switched. Speed vs quality, you choose.
		@note
		Has NO effect on windowed mode render targets. Only affects fullscreen mode.
		@param
		enabled If true, the system waits for vertical blanks - quality over speed. If false it doesn't - speed over quality.
		*/
		void setWaitForVerticalBlank(bool enabled);

		/** Returns true if the system is synchronising frames with the monitor vertical blank.
		*/
		bool getWaitForVerticalBlank(void) const;

		// ------------------------------------------------------------------------
		//                     Internal Rendering Access
		// All methods below here are normally only called by other Camelot classes
		// They can be called by library user if required
		// ------------------------------------------------------------------------

		/** Utility function for setting all the properties of a texture unit at once.
		This method is also worth using over the individual texture unit settings because it
		only sets those settings which are different from the current settings for this
		unit, thus minimising render state changes.
		*/
		virtual void setTextureUnitSettings(size_t texUnit, const TexturePtr& texture, const SamplerState& tl);
		/** Turns off a texture unit. */
		virtual void disableTextureUnit(size_t texUnit);
		/** Disables all texture units from the given unit upwards */
		virtual void disableTextureUnitsFrom(size_t texUnit);

		/** Sets the size of points and how they are attenuated with distance.
		@remarks
		When performing point rendering or point sprite rendering,
		point size can be attenuated with distance. The equation for
		doing this is attenuation = 1 / (constant + linear * dist + quadratic * d^2) .
		@par
		For example, to disable distance attenuation (constant screensize) 
		you would set constant to 1, and linear and quadratic to 0. A
		standard perspective attenuation would be 0, 1, 0 respectively.
		*/
		virtual void setPointParameters(float size, bool attenuationEnabled, 
			float constant, float linear, float quadratic, float minSize, float maxSize) = 0;


		/**
		Sets the texture to bind to a given texture unit.

		User processes would not normally call this direct unless rendering
		primitives themselves.

		@param unit The index of the texture unit to modify. Multitexturing 
		hardware can support multiple units (see 
		RenderSystemCapabilites::getNumTextureUnits)
		@param enabled Boolean to turn the unit on/off
		@param texPtr Pointer to the texture to use.
		*/
		virtual void setTexture(size_t unit, bool enabled, 
			const TexturePtr &texPtr) = 0;

		/** Binds a texture to a vertex sampler.
		@remarks
		Not all rendersystems support separate vertex samplers. For those that
		do, you can set a texture for them, separate to the regular texture
		samplers, using this method. For those that don't, you should use the
		regular texture samplers which are shared between the vertex and
		fragment units; calling this method will throw an exception.
		@see RenderSystemCapabilites::getVertexTextureUnitsShared
		*/
		virtual void setVertexTexture(size_t unit, const TexturePtr& tex);

		/** Sets the filtering options for a given texture unit.
		@param unit The texture unit to set the filtering options for
		@param minFilter The filter used when a texture is reduced in size
		@param magFilter The filter used when a texture is magnified
		@param mipFilter The filter used between mipmap levels, FO_NONE disables mipmapping
		*/
		virtual void setTextureFiltering(size_t unit, FilterOptions minFilter,
			FilterOptions magFilter, FilterOptions mipFilter);

		/** Sets a single filter for a given texture unit.
		@param unit The texture unit to set the filtering options for
		@param ftype The filter type
		@param filter The filter to be used
		*/
		virtual void setTextureFiltering(size_t unit, FilterType ftype, FilterOptions filter) = 0;

		/** Sets the maximal anisotropy for the specified texture unit.*/
		virtual void setTextureAnisotropy(size_t unit, unsigned int maxAnisotropy) = 0;

		/** Sets the texture addressing mode for a texture unit.*/
		virtual void setTextureAddressingMode(size_t unit, const SamplerState::UVWAddressingMode& uvw) = 0;

		/** Sets the texture border color for a texture unit.*/
		virtual void setTextureBorderColor(size_t unit, const Color& colour) = 0;

		/** Sets the mipmap bias value for a given texture unit.
		@remarks
		This allows you to adjust the mipmap calculation up or down for a
		given texture unit. Negative values force a larger mipmap to be used, 
		positive values force a smaller mipmap to be used. Units are in numbers
		of levels, so +1 forces the mipmaps to one smaller level.
		@note Only does something if render system has capability RSC_MIPMAP_LOD_BIAS.
		*/
		virtual void setTextureMipmapBias(size_t unit, float bias) = 0;

		/** Sets the global blending factors for combining subsequent renders with the existing frame contents.
		The result of the blending operation is:</p>
		<p align="center">final = (texture * sourceFactor) + (pixel * destFactor)</p>
		Each of the factors is specified as one of a number of options, as specified in the SceneBlendFactor
		enumerated type.
		By changing the operation you can change addition between the source and destination pixels to a different operator.
		@param sourceFactor The source factor in the above calculation, i.e. multiplied by the texture colour components.
		@param destFactor The destination factor in the above calculation, i.e. multiplied by the pixel colour components.
		@param op The blend operation mode for combining pixels
		*/
		virtual void setSceneBlending(SceneBlendFactor sourceFactor, SceneBlendFactor destFactor, SceneBlendOperation op = SBO_ADD) = 0;

		/** Sets the global blending factors for combining subsequent renders with the existing frame contents.
		The result of the blending operation is:</p>
		<p align="center">final = (texture * sourceFactor) + (pixel * destFactor)</p>
		Each of the factors is specified as one of a number of options, as specified in the SceneBlendFactor
		enumerated type.
		@param sourceFactor The source factor in the above calculation, i.e. multiplied by the texture colour components.
		@param destFactor The destination factor in the above calculation, i.e. multiplied by the pixel colour components.
		@param sourceFactorAlpha The source factor in the above calculation for the alpha channel, i.e. multiplied by the texture alpha components.
		@param destFactorAlpha The destination factor in the above calculation for the alpha channel, i.e. multiplied by the pixel alpha components.
		@param op The blend operation mode for combining pixels
		@param alphaOp The blend operation mode for combining pixel alpha values
		*/
		virtual void setSeparateSceneBlending(SceneBlendFactor sourceFactor, SceneBlendFactor destFactor, SceneBlendFactor sourceFactorAlpha, 
			SceneBlendFactor destFactorAlpha, SceneBlendOperation op = SBO_ADD, SceneBlendOperation alphaOp = SBO_ADD) = 0;

		/** Sets the global alpha rejection approach for future renders.
		By default images are rendered regardless of texture alpha. This method lets you change that.
		@param func The comparison function which must pass for a pixel to be written.
		@param val The value to compare each pixels alpha value to (0-255)
		@param alphaToCoverage Whether to enable alpha to coverage, if supported
		*/
		virtual void setAlphaRejectSettings(CompareFunction func, unsigned char value, bool alphaToCoverage) = 0;

		/**
		* Signifies the beginning of a frame, i.e. the start of rendering on a single viewport. Will occur
		* several times per complete frame if multiple viewports exist.
		*/
		virtual void beginFrame(void) = 0;
		
		/**
		* Ends rendering of a frame to the current viewport.
		*/
		virtual void endFrame(void) = 0;
		/**
		Sets the provided viewport as the active one for future
		rendering operations. This viewport is aware of it's own
		camera and render target. Must be implemented by subclass.

		@param target Pointer to the appropriate viewport.
		*/
		virtual void setViewport(Viewport *vp) = 0;
		/** Get the current active viewport for rendering. */
		virtual Viewport* getViewport(void);

		/** Sets the culling mode for the render system based on the 'vertex winding'.
		A typical way for the rendering engine to cull triangles is based on the
		'vertex winding' of triangles. Vertex winding refers to the direction in
		which the vertices are passed or indexed to in the rendering operation as viewed
		from the camera, and will wither be clockwise or anticlockwise (that's 'counterclockwise' for
		you Americans out there ;) The default is CULL_CLOCKWISE i.e. that only triangles whose vertices
		are passed/indexed in anticlockwise order are rendered - this is a common approach and is used in 3D studio models
		for example. You can alter this culling mode if you wish but it is not advised unless you know what you are doing.
		You may wish to use the CULL_NONE option for mesh data that you cull yourself where the vertex
		winding is uncertain.
		*/
		virtual void setCullingMode(CullingMode mode) = 0;

		virtual CullingMode getCullingMode(void) const;

		/** Sets the mode of operation for depth buffer tests from this point onwards.
		Sometimes you may wish to alter the behaviour of the depth buffer to achieve
		special effects. Because it's unlikely that you'll set these options for an entire frame,
		but rather use them to tweak settings between rendering objects, this is an internal
		method (indicated by the '_' prefix) which will be used by a SceneManager implementation
		rather than directly from the client application.
		If this method is never called the settings are automatically the same as the default parameters.
		@param depthTest If true, the depth buffer is tested for each pixel and the frame buffer is only updated
		if the depth function test succeeds. If false, no test is performed and pixels are always written.
		@param depthWrite If true, the depth buffer is updated with the depth of the new pixel if the depth test succeeds.
		If false, the depth buffer is left unchanged even if a new pixel is written.
		@param depthFunction Sets the function required for the depth test.
		*/
		virtual void setDepthBufferParams(bool depthTest = true, bool depthWrite = true, CompareFunction depthFunction = CMPF_LESS_EQUAL) = 0;

		/** Sets whether or not the depth buffer check is performed before a pixel write.
		@param enabled If true, the depth buffer is tested for each pixel and the frame buffer is only updated
		if the depth function test succeeds. If false, no test is performed and pixels are always written.
		*/
		virtual void setDepthBufferCheckEnabled(bool enabled = true) = 0;
		/** Sets whether or not the depth buffer is updated after a pixel write.
		@param enabled If true, the depth buffer is updated with the depth of the new pixel if the depth test succeeds.
		If false, the depth buffer is left unchanged even if a new pixel is written.
		*/
		virtual void setDepthBufferWriteEnabled(bool enabled = true) = 0;
		/** Sets the comparison function for the depth buffer check.
		Advanced use only - allows you to choose the function applied to compare the depth values of
		new and existing pixels in the depth buffer. Only an issue if the deoth buffer check is enabled
		(see _setDepthBufferCheckEnabled)
		@param  func The comparison between the new depth and the existing depth which must return true
		for the new pixel to be written.
		*/
		virtual void setDepthBufferFunction(CompareFunction func = CMPF_LESS_EQUAL) = 0;
		/** Sets whether or not colour buffer writing is enabled, and for which channels. 
		@remarks
		For some advanced effects, you may wish to turn off the writing of certain colour
		channels, or even all of the colour channels so that only the depth buffer is updated
		in a rendering pass. However, the chances are that you really want to use this option
		through the Material class.
		@param red, green, blue, alpha Whether writing is enabled for each of the 4 colour channels. */
		virtual void setColorBufferWriteEnabled(bool red, bool green, bool blue, bool alpha) = 0;
		/** Sets the depth bias, NB you should use the Material version of this. 
		@remarks
		When polygons are coplanar, you can get problems with 'depth fighting' where
		the pixels from the two polys compete for the same screen pixel. This is particularly
		a problem for decals (polys attached to another surface to represent details such as
		bulletholes etc.).
		@par
		A way to combat this problem is to use a depth bias to adjust the depth buffer value
		used for the decal such that it is slightly higher than the true value, ensuring that
		the decal appears on top.
		@note
		The final bias value is a combination of a constant bias and a bias proportional
		to the maximum depth slope of the polygon being rendered. The final bias
		is constantBias + slopeScaleBias * maxslope. Slope scale biasing is
		generally preferable but is not available on older hardware.
		@param constantBias The constant bias value, expressed as a value in 
		homogeneous depth coordinates.
		@param slopeScaleBias The bias value which is factored by the maximum slope
		of the polygon, see the description above. This is not supported by all
		cards.

		*/
		virtual void setDepthBias(float constantBias, float slopeScaleBias = 0.0f) = 0;

		/** The RenderSystem will keep a count of tris rendered, this resets the count. */
		virtual void beginGeometryCount(void);
		/** Reports the number of tris rendered since the last _beginGeometryCount call. */
		virtual unsigned int getFaceCount(void) const;
		/** Reports the number of batches rendered since the last _beginGeometryCount call. */
		virtual unsigned int getBatchCount(void) const;
		/** Reports the number of vertices passed to the renderer since the last _beginGeometryCount call. */
		virtual unsigned int getVertexCount(void) const;

		/** Generates a packed data version of the passed in ColourValue suitable for
		use as with this RenderSystem.
		@remarks
		Since different render systems have different colour data formats (eg
		RGBA for GL, ARGB for D3D) this method allows you to use 1 method for all.
		@param colour The colour to convert
		@param pDest Pointer to location to put the result.
		*/
		virtual void convertColorValue(const Color& colour, UINT32* pDest);
		/** Get the native VertexElementType for a compact 32-bit colour value
		for this rendersystem.
		*/
		virtual VertexElementType getColorVertexElementType(void) const = 0;

		/** Converts a uniform projection matrix to suitable for this render system.
		@remarks
		Because different APIs have different requirements (some incompatible) for the
		projection matrix, this method allows each to implement their own correctly and pass
		back a generic Camelot matrix for storage in the engine.
		*/
		virtual void _convertProjectionMatrix(const Matrix4& matrix,
			Matrix4& dest, bool forGpuProgram = false) = 0;

		/** Sets how to rasterise triangles, as points, wireframe or solid polys. */
		virtual void setPolygonMode(PolygonMode level) = 0;

		/** Turns stencil buffer checking on or off. 
		@remarks
		Stencilling (masking off areas of the rendering target based on the stencil 
		buffer) can be turned on or off using this method. By default, stencilling is
		disabled.
		*/
		virtual void setStencilCheckEnabled(bool enabled) = 0;
		/** Determines if this system supports hardware accelerated stencil buffer. 
		@remarks
		Note that the lack of this function doesn't mean you can't do stencilling, but
		the stencilling operations will be provided in software, which will NOT be
		fast.
		@par
		Generally hardware stencils are only supported in 32-bit colour modes, because
		the stencil buffer shares the memory of the z-buffer, and in most cards the 
		z-buffer has to be the same depth as the colour buffer. This means that in 32-bit
		mode, 24 bits of the z-buffer are depth and 8 bits are stencil. In 16-bit mode there
		is no room for a stencil (although some cards support a 15:1 depth:stencil option,
		this isn't useful for very much) so 8 bits of stencil are provided in software.
		This can mean that if you use stencilling, your applications may be faster in 
		32-but colour than in 16-bit, which may seem odd to some people.
		*/
		/*virtual bool hasHardwareStencil(void) = 0;*/

		/** This method allows you to set all the stencil buffer parameters in one call.
		@remarks
		The stencil buffer is used to mask out pixels in the render target, allowing
		you to do effects like mirrors, cut-outs, stencil shadows and more. Each of
		your batches of rendering is likely to ignore the stencil buffer, 
		update it with new values, or apply it to mask the output of the render.
		The stencil test is:<PRE>
		(Reference Value & Mask) CompareFunction (Stencil Buffer Value & Mask)</PRE>
		The result of this will cause one of 3 actions depending on whether the test fails,
		succeeds but with the depth buffer check still failing, or succeeds with the
		depth buffer check passing too.
		@par
		Unlike other render states, stencilling is left for the application to turn
		on and off when it requires. This is because you are likely to want to change
		parameters between batches of arbitrary objects and control the ordering yourself.
		In order to batch things this way, you'll want to use OGRE's separate render queue
		groups (see RenderQueue) and register a RenderQueueListener to get notifications
		between batches.
		@par
		There are individual state change methods for each of the parameters set using 
		this method. 
		Note that the default values in this method represent the defaults at system 
		start up too.
		@param func The comparison function applied.
		@param refValue The reference value used in the comparison
		@param mask The bitmask applied to both the stencil value and the reference value 
		before comparison
		@param stencilFailOp The action to perform when the stencil check fails
		@param depthFailOp The action to perform when the stencil check passes, but the
		depth buffer check still fails
		@param passOp The action to take when both the stencil and depth check pass.
		@param twoSidedOperation If set to true, then if you render both back and front faces 
		(you'll have to turn off culling) then these parameters will apply for front faces, 
		and the inverse of them will happen for back faces (keep remains the same).
		*/
		virtual void setStencilBufferParams(CompareFunction func = CMPF_ALWAYS_PASS, 
			UINT32 refValue = 0, UINT32 mask = 0xFFFFFFFF, 
			StencilOperation stencilFailOp = SOP_KEEP, 
			StencilOperation depthFailOp = SOP_KEEP,
			StencilOperation passOp = SOP_KEEP, 
			bool twoSidedOperation = false) = 0;



		/** Sets the current vertex declaration, ie the source of vertex data. */
		virtual void setVertexDeclaration(VertexDeclarationPtr decl) = 0;
		/** Sets the current vertex buffer binding state. */
		virtual void setVertexBufferBinding(VertexBufferBinding* binding) = 0;

		/**
		Render something to the active viewport.

		Low-level rendering interface to perform rendering
		operations. Unlikely to be used directly by client
		applications, since the SceneManager and various support
		classes will be responsible for calling this method.
		Can only be called between _beginScene and _endScene

		@param op A rendering operation instance, which contains
		details of the operation to be performed.
		*/
		virtual void render(const RenderOperation& op);

		/** Gets the capabilities of the render system. */
		const RenderSystemCapabilities* getCapabilities(void) const { return mCurrentCapabilities; }


		/** Returns the driver version.
		*/
		virtual const DriverVersion& getDriverVersion(void) const { return mDriverVersion; }

		/** Binds a given GpuProgram (but not the parameters). 
		@remarks Only one GpuProgram of each type can be bound at once, binding another
		one will simply replace the existing one.
		*/
		virtual void bindGpuProgram(GpuProgram* prg);

		/** Bind Gpu program parameters.
		@param gptype The type of program to bind the parameters to
		@param params The parameters to bind
		@param variabilityMask A mask of GpuParamVariability identifying which params need binding
		*/
		virtual void bindGpuProgramParameters(GpuProgramType gptype, 
			GpuProgramParametersSharedPtr params, UINT16 variabilityMask) = 0;

		/** Unbinds GpuPrograms of a given GpuProgramType.
		@remarks
		This returns the pipeline to fixed-function processing for this type.
		*/
		virtual void unbindGpuProgram(GpuProgramType gptype);

		/** Returns whether or not a Gpu program of the given type is currently bound. */
		virtual bool isGpuProgramBound(GpuProgramType gptype);

		/** Sets the user clipping region.
		*/
		virtual void setClipPlanes(const PlaneList& clipPlanes);

		/** Add a user clipping plane. */
		virtual void addClipPlane (const Plane &p);
		/** Add a user clipping plane. */
		virtual void addClipPlane (float A, float B, float C, float D);

		/** Clears the user clipping region.
		*/
		virtual void resetClipPlanes();

		/** Internal method for swapping all the buffers on all render targets,
		if _updateAllRenderTargets was called with a 'false' parameter. */
		virtual void swapAllRenderTargetBuffers(bool waitForVsync = true);

		/** Sets whether or not vertex windings set should be inverted; this can be important
		for rendering reflections. */
		virtual void setInvertVertexWinding(bool invert);

		/** Indicates whether or not the vertex windings set will be inverted for the current render (e.g. reflections)
		@see RenderSystem::setInvertVertexWinding
		*/
		virtual bool getInvertVertexWinding(void) const;

		/** Sets the 'scissor region' ie the region of the target in which rendering can take place.
		@remarks
		This method allows you to 'mask off' rendering in all but a given rectangular area
		as identified by the parameters to this method.
		@note
		Not all systems support this method. Check the RenderSystemCapabilities for the
		RSC_SCISSOR_TEST capability to see if it is supported.
		@param enabled True to enable the scissor test, false to disable it.
		@param left, top, right, bottom The location of the corners of the rectangle, expressed in
		<i>pixels</i>.
		*/
		virtual void setScissorTest(bool enabled, size_t left = 0, size_t top = 0, 
			size_t right = 800, size_t bottom = 600) = 0;

		/** Clears one or more frame buffers on the active render target. 
		@param buffers Combination of one or more elements of FrameBufferType
		denoting which buffers are to be cleared
		@param colour The colour to clear the colour buffer with, if enabled
		@param depth The value to initialise the depth buffer with, if enabled
		@param stencil The value to initialise the stencil buffer with, if enabled.
		*/
		virtual void clearFrameBuffer(unsigned int buffers, 
			const Color& colour = Color::Black, 
			float depth = 1.0f, unsigned short stencil = 0) = 0;
		/** Returns the horizontal texel offset value required for mapping 
		texel origins to pixel origins in this rendersystem.
		@remarks
		Since rendersystems sometimes disagree on the origin of a texel, 
		mapping from texels to pixels can sometimes be problematic to 
		implement generically. This method allows you to retrieve the offset
		required to map the origin of a texel to the origin of a pixel in
		the horizontal direction.
		*/
		virtual float getHorizontalTexelOffset(void) = 0;
		/** Returns the vertical texel offset value required for mapping 
		texel origins to pixel origins in this rendersystem.
		@remarks
		Since rendersystems sometimes disagree on the origin of a texel, 
		mapping from texels to pixels can sometimes be problematic to 
		implement generically. This method allows you to retrieve the offset
		required to map the origin of a texel to the origin of a pixel in
		the vertical direction.
		*/
		virtual float getVerticalTexelOffset(void) = 0;

		/** Gets the minimum (closest) depth value to be used when rendering
		using identity transforms.
		@remarks
		When using identity transforms you can manually set the depth
		of a vertex; however the input values required differ per
		rendersystem. This method lets you retrieve the correct value.
		@see Renderable::getUseIdentityView, Renderable::getUseIdentityProjection
		*/
		virtual float getMinimumDepthInputValue(void) = 0;
		/** Gets the maximum (farthest) depth value to be used when rendering
		using identity transforms.
		@remarks
		When using identity transforms you can manually set the depth
		of a vertex; however the input values required differ per
		rendersystem. This method lets you retrieve the correct value.
		@see Renderable::getUseIdentityView, Renderable::getUseIdentityProjection
		*/
		virtual float getMaximumDepthInputValue(void) = 0;

		/**
         * Set current render target to target, enabling its device context if needed
         */
        virtual void setRenderTarget(RenderTarget *target) = 0;

		/** Register the an additional thread which may make calls to rendersystem-related 
		objects.
		@remarks
		This method should only be called by additional threads during their
		initialisation. If they intend to use hardware rendering system resources 
		they should call this method before doing anything related to the render system.
		Some rendering APIs require a per-thread setup and this method will sort that
		out. It is also necessary to call unregisterThread before the thread shuts down.
		@note
		This method takes no parameters - it must be called from the thread being
		registered and that context is enough.
		*/
		virtual void registerThread() = 0;

		/** Unregister an additional thread which may make calls to rendersystem-related objects.
		@see RenderSystem::registerThread
		*/
		virtual void unregisterThread() = 0;

		/**
		* Gets the number of display monitors.
		@see Root::getDisplayMonitorCount
		*/
		virtual unsigned int getDisplayMonitorCount() const = 0;
	protected:
		/** The render targets. */
		RenderTargetMap mRenderTargets;
		/** The render targets, ordered by priority. */
		RenderTargetPriorityMap mPrioritisedRenderTargets;
		/** The Active render target. */
		RenderTarget * mActiveRenderTarget;
		/** The Active GPU programs and gpu program parameters*/
		GpuProgramParametersSharedPtr mActiveVertexGpuProgramParameters;
		GpuProgramParametersSharedPtr mActiveGeometryGpuProgramParameters;
		GpuProgramParametersSharedPtr mActiveFragmentGpuProgramParameters;

		// Active viewport (dest for future rendering operations)
		Viewport* mActiveViewport;

		CullingMode mCullingMode;

		bool mVSync;
		unsigned int mVSyncInterval;

		size_t mBatchCount;
		size_t mFaceCount;
		size_t mVertexCount;

		bool mInvertVertexWinding;

		/// Texture units from this upwards are disabled
		size_t mDisabledTexUnitsFrom;

		bool mVertexProgramBound;
		bool mGeometryProgramBound;
		bool mFragmentProgramBound;

		// Recording user clip planes
		PlaneList mClipPlanes;
		// Indicator that we need to re-set the clip planes on next render call
		bool mClipPlanesDirty;

		/// Used to store the capabilities of the graphics card
		RenderSystemCapabilities* mRealCapabilities;
		RenderSystemCapabilities* mCurrentCapabilities;
		bool mUseCustomCapabilities;

		/// Internal method used to set the underlying clip planes when needed
		virtual void setClipPlanesImpl(const PlaneList& clipPlanes) = 0;

		/** Initialize the render system from the capabilities*/
		virtual void initialiseFromRenderSystemCapabilities(RenderSystemCapabilities* caps, RenderTarget* primary) = 0;

		DriverVersion mDriverVersion;
	};
	/** @} */
	/** @} */
}

#endif
