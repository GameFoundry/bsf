using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

namespace bs
{
	/** @addtogroup Physics
	 *  @{
	 */

	/// <summary>Reports in which directions is the character colliding with other objects.</summary>
	public enum CharacterCollisionFlag
	{
		/// <summary>Character is colliding with its sides.</summary>
		Sides = 1,
		/// <summary>Character is colliding with the ceiling.</summary>
		Up = 2,
		/// <summary>Character is colliding with the ground.</summary>
		Down = 4
	}

	/** @} */
}
