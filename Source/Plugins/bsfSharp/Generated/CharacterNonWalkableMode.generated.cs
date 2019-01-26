using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

namespace bs
{
	/** @addtogroup Physics
	 *  @{
	 */

	/// <summary>Controls behaviour when a character controller reaches a slope thats larger than its slope offset.</summary>
	public enum CharacterNonWalkableMode
	{
		/// <summary>Character will be prevented from going further, but will be allowed to move laterally.</summary>
		Prevent = 0,
		/// <summary>Character will be prevented from going further, but also slide down the slope.</summary>
		PreventAndSlide = 1
	}

	/** @} */
}
