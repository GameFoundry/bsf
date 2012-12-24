#pragma once

#include "CmPrerequisites.h"
#include "CmModule.h"

namespace CamelotEngine
{
	class CM_EXPORT RenderWindowManager : public Module<RenderWindowManager>
	{
	public:
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
			<td>Parent window handle, for embedding the engine in a child of an external window</td>
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
			<td>On the Mac platform the most diffused method to embed engine in a custom application is to use Interface Builder
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
		RenderWindowPtr create(const String& name, UINT32 width, UINT32 height, 
			bool fullScreen, const map<String, String>::type* miscParams = nullptr);

	protected:
		virtual void createImpl(const String& name, UINT32 width, UINT32 height, 
			bool fullScreen, const map<String, String>::type& miscParams, AsyncOp& asyncOp) = 0;
	};
}