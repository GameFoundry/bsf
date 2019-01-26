using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

namespace bs
{
	/** @addtogroup Physics
	 *  @{
	 */

	/// <summary>Information about a single contact point during physics collision.</summary>
	[StructLayout(LayoutKind.Sequential), SerializeObject]
	public partial struct ContactPoint
	{
		/// <summary>Contact point in world space.</summary>
		public Vector3 position;
		/// <summary>Normal pointing from the second shape to the first shape.</summary>
		public Vector3 normal;
		/// <summary>
		/// Impulse applied to the objects to keep them from penetrating. Divide by simulation step to get the force.
		/// </summary>
		public float impulse;
		/// <summary>Determines how far are the objects. Negative value denotes penetration.</summary>
		public float separation;
	}

	/** @} */
}
