using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

namespace bs
{
	/** @addtogroup Audio
	 *  @{
	 */

	/// <summary>Valid states in which AudioSource can be in.</summary>
	public enum AudioSourceState
	{
		/// <summary>Source is currently playing.</summary>
		Playing = 0,
		/// <summary>Source is currently paused (play will resume from paused point).</summary>
		Paused = 1,
		/// <summary>Source is currently stopped (play will resume from start).</summary>
		Stopped = 2
	}

	/** @} */
}
