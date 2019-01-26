using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

namespace bs
{
	/** @addtogroup Particles
	 *  @{
	 */

	/// <summary>Possible orientations when rendering billboard particles.</summary>
	public enum ParticleOrientation
	{
		/// <summary>Orient towards view (camera) plane.</summary>
		ViewPlane = 0,
		/// <summary>Orient towards view (camera) position.</summary>
		ViewPosition = 1,
		/// <summary>Orient with a user-provided axis.</summary>
		Plane = 2
	}

	/** @} */
}
