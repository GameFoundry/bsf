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

	/// <summary>A single face belonging to a font family.</summary>
	[StructLayout(LayoutKind.Sequential), SerializeObject]
	public partial struct FontFamilyFace
	{
		/// <summary>Initializes the struct with default values.</summary>
		public static FontFamilyFace Default()
		{
			FontFamilyFace value = new FontFamilyFace();
			value.Font = null;
			value.Style = FontFaceStyle.Default();

			return value;
		}

		public FontFamilyFace(RRef<Font> font, FontFaceStyle style)
		{
			this.Font = font;
			this.Style = style;
		}

		/// <summary>
		/// Font providing the face. May not have been loaded yet, as faces are only loaded once they are used.
		/// </summary>
		public RRef<Font> Font;
		public FontFaceStyle Style;
	}

	/** @} */
}
