//********************************* bs::framework - Copyright 2018-2019 Marko Pintera ************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

#if IS_B3D
namespace BansheeEngine
#else
namespace bs
#endif
{
	/** @addtogroup GUI
	 *  @{
	 */

	/// <summary>Specific texture and text color used in a particular GUI element style.</summary>
	[StructLayout(LayoutKind.Sequential), SerializeObject]
	public partial struct GUIElementStateStyle
	{
		public SpriteTexture texture;
		public Color textColor;
	}

	/** @} */
}
