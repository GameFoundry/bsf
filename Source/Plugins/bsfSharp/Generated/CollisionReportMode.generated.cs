using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

namespace bs
{
	/** @addtogroup Physics
	 *  @{
	 */

	/// <summary>Determines which collision events will be reported by physics objects.</summary>
	public enum CollisionReportMode
	{
		/// <summary>No collision events will be triggered.</summary>
		None = 0,
		/// <summary>Collision events will be triggered when object enters and/or leaves collision.</summary>
		Report = 1,
		/// <summary>
		/// Collision events will be triggered when object enters and/or leaves collision, but also every frame the object 
		/// remains in collision.
		/// </summary>
		ReportPersistent = 2
	}

	/** @} */
}
