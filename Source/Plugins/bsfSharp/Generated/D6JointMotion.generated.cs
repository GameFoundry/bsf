using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

namespace bs
{
	/** @addtogroup Physics
	 *  @{
	 */

	/// <summary>Specifies type of constraint placed on a specific axis.</summary>
	public enum D6JointMotion
	{
		/// <summary>Axis is immovable.</summary>
		Locked = 0,
		/// <summary>Axis will be constrained by the specified limits.</summary>
		Limited = 1,
		/// <summary>Axis will not be constrained.</summary>
		Free = 2,
		Count = 3
	}

	/** @} */
}
