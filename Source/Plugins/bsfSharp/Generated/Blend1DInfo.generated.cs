using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

namespace bs
{
	/** @addtogroup Animation
	 *  @{
	 */

	/// <summary>
	/// Defines a 1D blend where multiple animation clips are blended between each other using linear interpolation.
	/// </summary>
	[StructLayout(LayoutKind.Sequential), SerializeObject]
	public partial struct Blend1DInfo
	{
		public BlendClipInfo[] clips;
	}

	/** @} */
}
