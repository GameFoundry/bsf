using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

namespace bs
{
	/** @addtogroup GUI_Engine
	 *  @{
	 */

	/// <summary>Kerning pair representing larger or smaller offset between a specific pair of characters.</summary>
	[StructLayout(LayoutKind.Sequential), SerializeObject]
	public partial struct KerningPair
	{
		public uint otherCharId;
		public int amount;
	}

	/** @} */
}
