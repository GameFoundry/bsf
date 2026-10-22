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

	/// <summary>Descriptor structure used for initialization of a FontFamily.</summary>
	[StructLayout(LayoutKind.Sequential), SerializeObject]
	public partial struct FontFamilyCreateInformation
	{
		/// <summary>Name the family is identified by. Family lookups match this name, ignoring case.</summary>
		public string Name;
		/// <summary>Faces the family provides. Faces with a style already provided by an earlier face are discarded.</summary>
		public FontFamilyFace[] Faces;
	}

	/** @} */
}
