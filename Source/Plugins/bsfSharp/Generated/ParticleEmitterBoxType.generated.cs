using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

namespace bs
{
	/** @addtogroup Particles
	 *  @{
	 */

	/// <summary>Determines the emission type for the cone particle emitter shape.</summary>
	public enum ParticleEmitterBoxType
	{
		/// <summary>Particles will be emitted from the entire volume.</summary>
		Volume = 0,
		/// <summary>Particles will be emitted only from box surface.</summary>
		Surface = 1,
		/// <summary>Particles will be emitted only from box edge.</summary>
		Edge = 2
	}

	/** @} */
}
