//************************************ B3D Framework - Copyright 2025 Marko Pintera **************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

namespace b3d
{
	/** @addtogroup Text
	 *  @{
	 */

	/// <summary>
	/// Weight of a font face, on the same 100-900 scale used by the OS/2 table of a font file and by CSS. Values other than 
	/// the named ones are valid, as some families ship intermediate weights.
	/// </summary>
	public enum FontWeight : ushort
	{
		Thin = 100,
		ExtraBold = 800,
		Normal = 400,
		ExtraLight = 200,
		Light = 300,
		Medium = 500,
		Bold = 700,
		Black = 900,
		SemiBold = 600
	}

	/** @} */
}
