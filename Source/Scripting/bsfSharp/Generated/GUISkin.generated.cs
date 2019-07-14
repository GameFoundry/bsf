//********************************* bs::framework - Copyright 2018-2019 Marko Pintera ************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

namespace bs
{
	/** @addtogroup GUI
	 *  @{
	 */

	/// <summary>
	/// Holds a set of styles that control how are GUI element types positioned and displayed in the GUI. Each element type
	/// can be assigned a specific style.
	/// </summary>
	[ShowInInspector]
	public partial class GUISkin : Resource
	{
		private GUISkin(bool __dummy0) { }

		/// <summary>Creates an empty GUI skin and returns a handle to it.</summary>
		public GUISkin()
		{
			Internal_create(this);
		}

		/// <summary>Returns a reference wrapper for this resource.</summary>
		public RRef<GUISkin> Ref
		{
			get { return Internal_GetRef(mCachedPtr); }
		}

		/// <summary>Returns names of all styles registered on this skin.</summary>
		[NativeWrapper]
		public string[] StyleNames
		{
			get { return Internal_getStyleNames(mCachedPtr); }
		}

		/// <summary>Returns a reference wrapper for this resource.</summary>
		public static implicit operator RRef<GUISkin>(GUISkin x)
		{
			if(x != null)
				return Internal_GetRef(x.mCachedPtr);
			else
				return null;
		}

		/// <summary>Checks if the style with the specified name exists.</summary>
		public bool HasStyle(string name)
		{
			return Internal_hasStyle(mCachedPtr, name);
		}

		/// <summary>Returns a style for the specified GUI element type.</summary>
		public GUIElementStyle GetStyle(string guiElemType)
		{
			return Internal_getStyle(mCachedPtr, guiElemType);
		}

		/// <summary>Sets a style for the specified GUI element type.</summary>
		public void SetStyle(string guiElemType, GUIElementStyle style)
		{
			Internal_setStyle(mCachedPtr, guiElemType, style);
		}

		/// <summary>Removes a style for the specified GUI element type.</summary>
		public void RemoveStyle(string guiElemType)
		{
			Internal_removeStyle(mCachedPtr, guiElemType);
		}

		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern RRef<GUISkin> Internal_GetRef(IntPtr thisPtr);
		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern bool Internal_hasStyle(IntPtr thisPtr, string name);
		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern GUIElementStyle Internal_getStyle(IntPtr thisPtr, string guiElemType);
		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern void Internal_setStyle(IntPtr thisPtr, string guiElemType, GUIElementStyle style);
		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern void Internal_removeStyle(IntPtr thisPtr, string guiElemType);
		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern string[] Internal_getStyleNames(IntPtr thisPtr);
		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern void Internal_create(GUISkin managedInstance);
	}

	/** @} */
}
