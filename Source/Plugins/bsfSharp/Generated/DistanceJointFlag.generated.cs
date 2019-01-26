using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

namespace bs
{
	/** @addtogroup Physics
	 *  @{
	 */

	/// <summary>Controls distance joint options.</summary>
	public enum DistanceJointFlag
	{
		/// <summary>Enables minimum distance limit.</summary>
		MinDistance = 1,
		/// <summary>Enables maximum distance limit.</summary>
		MaxDistance = 2,
		/// <summary>Enables spring when maintaining limits.</summary>
		Spring = 4
	}

	/** @} */
}
