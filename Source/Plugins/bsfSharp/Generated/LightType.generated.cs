using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

namespace bs
{
	/** @addtogroup Rendering
	 *  @{
	 */

	/// <summary>Light type that determines how is light information parsed by the renderer and other systems.</summary>
	public enum LightType
	{
		Directional = 0,
		Radial = 1,
		Spot = 2
	}

	/** @} */
}
