using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

namespace bs
{
	/** @addtogroup Physics
	 *  @{
	 */

	/// <summary>Information about a collision between two physics objects.</summary>
	[StructLayout(LayoutKind.Sequential), SerializeObject]
	public partial struct CollisionData
	{
		/// <summary>Components of the colliders that have collided.</summary>
		public Collider[] collider;
		/// <summary>Information about all the contact points for the hit.</summary>
		public ContactPoint[] contactPoints;
	}

	/** @} */
}
