//************************************ B3D Framework - Copyright 2025 Marko Pintera **************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

namespace b3d
{
	/// <summary>
	/// Font specific resource meta-data recording which face of which family the font contains. Meta-data is stored alongside 
	/// the resource in its package and remains available without loading the font itself, allowing font families to be 
	/// indexed without paying for the font data of every face.
	/// </summary>
	[ShowInInspector]
	public partial class FontMetaData : ResourceMetaData
	{
		private FontMetaData(bool __dummy0) { }
		protected FontMetaData() { }

		/// <summary>
		/// Name of the family the face belongs to, as reported by the font file. Empty if the font file provides no name.
		/// </summary>
		[ShowInInspector]
		[NativeWrapper]
		public string FamilyName
		{
			get { return Internal_GetFamilyName(mCachedPtr); }
			set { Internal_SetFamilyName(mCachedPtr, value); }
		}

		/// <summary>Weight and slant of this particular face, as reported by the font file.</summary>
		[ShowInInspector]
		[NativeWrapper]
		public FontFaceStyle Style
		{
			get
			{
				FontFaceStyle temp;
				Internal_GetStyle(mCachedPtr, out temp);
				return temp;
			}
			set { Internal_SetStyle(mCachedPtr, ref value); }
		}

		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern string Internal_GetFamilyName(IntPtr thisPtr);
		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern void Internal_SetFamilyName(IntPtr thisPtr, string value);
		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern void Internal_GetStyle(IntPtr thisPtr, out FontFaceStyle __output);
		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern void Internal_SetStyle(IntPtr thisPtr, ref FontFaceStyle value);
	}
}
