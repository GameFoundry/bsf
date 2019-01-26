using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

namespace bs
{
	/** @addtogroup Rendering
	 *  @{
	 */

	/// <summary>Available texture types.</summary>
	public enum TextureType
	{
		/// <summary>One dimensional texture. Just a row of pixels.</summary>
		Texture1D = 1,
		/// <summary>Two dimensional texture.</summary>
		Texture2D = 2,
		/// <summary>Three dimensional texture.</summary>
		Texture3D = 3,
		/// <summary>Texture consisting out of six 2D textures describing an inside of a cube. Allows special sampling.</summary>
		TextureCube = 4
	}

	/** @} */
}
