//********************************* bs::framework - Copyright 2018-2019 Marko Pintera ************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

namespace bs
{
	/** @addtogroup Math
	 *  @{
	 */

	/// <summary>Rectangle represented in the form of offsets from some parent rectangle.</summary>
	[StructLayout(LayoutKind.Sequential), SerializeObject]
	public partial struct RectOffset
	{
		/// <summary>Initializes the struct with default values.</summary>
		public static RectOffset Default()
		{
			RectOffset value = new RectOffset();
			value.left = 0;
			value.right = 0;
			value.top = 0;
			value.bottom = 0;

			return value;
		}

		public RectOffset(int left, int right, int top, int bottom)
		{
			this.left = left;
			this.right = right;
			this.top = top;
			this.bottom = bottom;
		}

		public int left;
		public int right;
		public int top;
		public int bottom;
	}

	/** @} */
}
