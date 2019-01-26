using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

namespace bs
{
	/** @addtogroup Image
	 *  @{
	 */

	/// <summary>Single key in a ColorGradient.</summary>
	[StructLayout(LayoutKind.Sequential), SerializeObject]
	public partial struct ColorGradientKey
	{
		/// <summary>Initializes the struct with default values.</summary>
		public static ColorGradientKey Default()
		{
			ColorGradientKey value = new ColorGradientKey();
			value.color = new Color();
			value.time = 0f;

			return value;
		}

		public ColorGradientKey(Color color, float time)
		{
			this.color = color;
			this.time = time;
		}

		public Color color;
		public float time;
	}

	/** @} */
}
