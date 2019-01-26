using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

namespace bs
{
	/** @addtogroup Rendering
	 *  @{
	 */

	/// <summary>Information about a single probe in the light probe volume.</summary>
	[StructLayout(LayoutKind.Sequential), SerializeObject]
	public partial struct LightProbeInfo
	{
		public uint handle;
		public Vector3 position;
	}

	/** @} */
}
