using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

namespace bs
{
	/** @addtogroup Particles
	 *  @{
	 */

	/// <summary>Determines the emission type for the cone particle emitter shape.</summary>
	public enum ParticleEmitterConeType
	{
		/// <summary>Emit particles only from the cone base.</summary>
		Base = 0,
		/// <summary>Emit particles from the entire cone volume.</summary>
		Volume = 1
	}

	/** @} */
}
