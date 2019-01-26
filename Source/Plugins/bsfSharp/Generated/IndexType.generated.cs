using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

namespace bs
{
	/** @addtogroup Rendering
	 *  @{
	 */

	/// <summary>Type of mesh indices used, used for determining maximum number of vertices in a mesh.</summary>
	public enum IndexType
	{
		/// <summary>16-bit indices.</summary>
		Index16 = 0,
		/// <summary>32-bit indices.</summary>
		Index32 = 1
	}

	/** @} */
}
