//********************************* bs::framework - Copyright 2018-2019 Marko Pintera ************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

namespace bs
{
	/** @addtogroup Particles
	 *  @{
	 */

	/// <summary>Information describing a ParticleEmitterHemisphereShape.</summary>
	[StructLayout(LayoutKind.Sequential), SerializeObject]
	public partial struct ParticleHemisphereShapeOptions
	{
		/// <summary>Initializes the struct with default values.</summary>
		public static ParticleHemisphereShapeOptions Default()
		{
			ParticleHemisphereShapeOptions value = new ParticleHemisphereShapeOptions();
			value.radius = 1f;
			value.thickness = 0f;

			return value;
		}

		/// <summary>Radius of the hemisphere.</summary>
		public float radius;
		/// <summary>
		/// Proportion of the volume that can emit particles. Thickness of 0 results in particles being emitted only from the
		/// edge of the volume, while thickness of 1 results in particles being emitted from the entire volume. In-between values
		/// will use a part of the volume.
		/// </summary>
		public float thickness;
	}

	/** @} */
}
