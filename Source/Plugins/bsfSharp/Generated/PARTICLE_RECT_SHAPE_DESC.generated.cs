using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

namespace bs
{
	/** @addtogroup Particles
	 *  @{
	 */

	/// <summary>Information describing a ParticleEmitterRectShape.</summary>
	[StructLayout(LayoutKind.Sequential), SerializeObject]
	public partial struct ParticleRectShapeOptions
	{
		/// <summary>Extents of the rectangle.</summary>
		public Vector2 extents;
	}

	/** @} */
}
