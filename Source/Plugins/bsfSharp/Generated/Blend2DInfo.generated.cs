using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

namespace bs
{
	/** @addtogroup Animation
	 *  @{
	 */

	/// <summary>
	/// Defines a 2D blend where two animation clips are blended between each other using bilinear interpolation.
	/// </summary>
	[StructLayout(LayoutKind.Sequential), SerializeObject]
	public partial struct Blend2DInfo
	{
		public RRef<AnimationClip> topLeftClip;
		public RRef<AnimationClip> topRightClip;
		public RRef<AnimationClip> botLeftClip;
		public RRef<AnimationClip> botRightClip;
	}

	/** @} */
}
