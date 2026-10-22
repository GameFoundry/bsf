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

	/// <summary>Identifies a single face within a font family.</summary>
	[StructLayout(LayoutKind.Sequential), SerializeObject]
	public partial struct FontFaceStyle
	{
		/// <summary>Initializes the struct with default values.</summary>
		public static FontFaceStyle Default()
		{
			FontFaceStyle value = new FontFaceStyle();
			value.Weight = FontWeight.Normal;
			value.Slant = FontSlant.Normal;

			return value;
		}

		public FontFaceStyle(FontWeight weight, FontSlant slant = FontSlant.Normal)
		{
			this.Weight = weight;
			this.Slant = slant;
		}

		public FontWeight Weight;
		public FontSlant Slant;
	}

	/** @} */
}
