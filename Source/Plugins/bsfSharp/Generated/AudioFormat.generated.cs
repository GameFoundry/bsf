using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

namespace bs
{
	/** @addtogroup Audio
	 *  @{
	 */

	/// <summary>Valid internal engine audio formats.</summary>
	public enum AudioFormat
	{
		/// <summary>Pulse code modulation audio ("raw" uncompressed audio).</summary>
		PCM = 0,
		/// <summary>Vorbis compressed audio.</summary>
		VORBIS = 1
	}

	/** @} */
}
