using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

namespace bs
{
	/** @addtogroup Rendering
	 *  @{
	 */

	/// <summary>Type of parameters that can be defined by a shader.</summary>
	public enum ShaderParameterType
	{
		Texture3D = 8,
		Float = 0,
		Vector2 = 1,
		Vector3 = 2,
		Vector4 = 3,
		Color = 4,
		Matrix3 = 5,
		Matrix4 = 6,
		Texture2D = 7,
		TextureCube = 9,
		Sampler = 10
	}

	/** @} */
}
