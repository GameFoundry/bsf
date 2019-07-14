//********************************* bs::framework - Copyright 2018-2019 Marko Pintera ************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

namespace bs
{
#if !IS_B3D
	/** @addtogroup Importer
	 *  @{
	 */

	/// <summary>
	/// Base class for creating import options from. Import options are specific for each importer and control how is data
	/// imported.
	/// </summary>
	[ShowInInspector]
	public partial class ImportOptions : ScriptObject
	{
		private ImportOptions(bool __dummy0) { }
		protected ImportOptions() { }

	}

	/** @} */
#endif
}
