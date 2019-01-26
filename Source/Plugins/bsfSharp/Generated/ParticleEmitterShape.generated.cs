using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

namespace bs
{
	/** @addtogroup Particles
	 *  @{
	 */

	/// <summary>
	/// Base class from all emitter shapes. Emitter shapes determine the position and direction of newly created particles.
	/// </summary>
	[ShowInInspector]
	public partial class ParticleEmitterShape : ScriptObject
	{
		private ParticleEmitterShape(bool __dummy0) { }
		protected ParticleEmitterShape() { }

	}

	/** @} */
}
