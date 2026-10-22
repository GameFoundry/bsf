//************************************ B3D Framework - Copyright 2025 Marko Pintera **************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

namespace b3d
{
	/** @addtogroup Text
	 *  @{
	 */

	/// <summary>
	/// A named group of Font faces that differ only in weight and slant. Provides the lookup from a requested style to the 
	/// face that best provides it, so callers can ask for bold or italic text without knowing which faces the family ships.
	/// </summary>
	[ShowInInspector]
	public partial class FontFamily : Resource
	{
		private FontFamily(bool __dummy0) { }
		protected FontFamily() { }

		/// <summary>Returns a reference wrapper for this resource.</summary>
		public RRef<FontFamily> Ref
		{
			get { return Internal_GetRef(mCachedPtr); }
		}

		/// <summary>Returns a reference wrapper for this resource.</summary>
		public static implicit operator RRef<FontFamily>(FontFamily x)
		{
			if(x != null)
				return Internal_GetRef(x.mCachedPtr);
			else
				return null;
		}

		/// <summary>
		/// Returns the name the family is identified by, fixed when the family is created. Family lookups match this name 
		/// ignoring case.
		/// </summary>
		public string GetFamilyName()
		{
			return Internal_GetFamilyName(mCachedPtr);
		}

		/// <summary>
		/// Returns the face that best matches the requested style, loading it if it has not been loaded yet. When no face 
		/// provides the requested style exactly the closest available face is substituted, so this only returns null for a 
		/// family that provides no face that can be loaded. Use HasFace() when an exact match is required.
		/// </summary>
		public Font GetFace(FontFaceStyle style)
		{
			return Internal_GetFace(mCachedPtr, ref style);
		}

		/// <summary>Returns true if the family provides a face with exactly the requested style.</summary>
		public bool HasFace(FontFaceStyle style)
		{
			return Internal_HasFace(mCachedPtr, ref style);
		}

		/// <summary>
		/// Adds a face to the family. The font need not have been loaded, as faces are only loaded once they are used. Returns 
		/// false without modifying the family if <paramref name="font"/> is null, or if the family already provides a face with 
		/// the same style.
		/// </summary>
		public bool AddFace(RRef<Font> font, FontFaceStyle style)
		{
			return Internal_AddFace(mCachedPtr, font, ref style);
		}

		/// <summary>
		/// Removes the face with exactly the provided style. Returns false if the family provides no such face.
		/// </summary>
		public bool RemoveFace(FontFaceStyle style)
		{
			return Internal_RemoveFace(mCachedPtr, ref style);
		}

		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern RRef<FontFamily> Internal_GetRef(IntPtr thisPtr);
		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern string Internal_GetFamilyName(IntPtr thisPtr);
		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern Font Internal_GetFace(IntPtr thisPtr, ref FontFaceStyle style);
		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern bool Internal_HasFace(IntPtr thisPtr, ref FontFaceStyle style);
		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern bool Internal_AddFace(IntPtr thisPtr, RRef<Font> font, ref FontFaceStyle style);
		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern bool Internal_RemoveFace(IntPtr thisPtr, ref FontFaceStyle style);
	}

	/** @} */
}
