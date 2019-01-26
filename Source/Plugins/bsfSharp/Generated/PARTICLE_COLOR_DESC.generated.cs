using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

namespace bs
{
	/** @addtogroup Particles
	 *  @{
	 */

	/// <summary>Structure used for initializing a ParticleColor object.</summary>
	[StructLayout(LayoutKind.Sequential), SerializeObject]
	public partial struct ParticleColorOptions
	{
		/// <summary>Determines the color of the particles evaluated over particle lifetime.</summary>
		public ColorDistribution color;
	}

	/** @} */
}
