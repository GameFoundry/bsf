//************************************ B3D Framework - Copyright 2025 Marko Pintera **************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

namespace b3d
{
	/** @addtogroup GUI
	 *  @{
	 */

	/// <summary>
	/// States a GUI element may be in. Multiple states can be active at the same time (e.g. an element can be hovered and 
	/// checked), which is why they are combined into GUIElementStates rather than used individually.
	/// </summary>
	public enum GUIElementState
	{
		/// <summary>No state is active, meaning the element is not being interacted with.</summary>
		Normal = 0,
		/// <summary>Element is disabled and does not react to input.</summary>
		Disabled = 8,
		/// <summary>Pointer is hovering over the element.</summary>
		Hover = 1,
		/// <summary>Element is being pressed.</summary>
		Active = 2,
		/// <summary>Element has keyboard/gamepad input focus.</summary>
		Focus = 4,
		/// <summary>Element is toggled on.</summary>
		Checked = 16
	}

	/** @} */
}
