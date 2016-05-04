//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

namespace BansheeEngine
{
    /** @addtogroup Utility
     *  @{
     */

    /// <summary>
    /// Contains mesh vertex and index data used for initializing, updating and reading mesh data from Mesh.
    /// </summary>
    public class MeshData : ScriptObject
    {
        /// <summary>
        /// Constructor for internal runtime use only.
        /// </summary>
        private MeshData()
        { }

        /// <summary>
        /// Creates a new mesh data that can hold number of vertices described by the provided vertex layout as well as a 
        /// number of indices of the provided type.
        /// </summary>
        /// <param name="numVertices">Number of vertices in the mesh.</param>
        /// <param name="numIndices">Number of indices in the mesh. </param>
        /// <param name="vertex">Controls how are vertices organized in the vertex buffer and what data they contain.</param>
        /// <param name="index">Size of indices, use smaller size for better performance, however be careful not to go over
        ///                     the number of vertices limited by the size.</param>
        public MeshData(int numVertices, int numIndices, VertexType vertex = VertexType.Position,
            IndexType index = IndexType.Index32)
        {
            Internal_CreateInstance(this, numVertices, numIndices, vertex, index);
        }

        /// <summary>
        /// An array of all vertex positions. Only valid if the vertex layout contains vertex positions.
        /// </summary>
        public Vector3[] Positions
        {
            get { return Internal_GetPositions(mCachedPtr); }
            set { Internal_SetPositions(mCachedPtr, value); }
        }

        /// <summary>
        /// An array of all vertex normals. Only valid if the vertex layout contains vertex normals.
        /// </summary>
        public Vector3[] Normals
        {
            get { return Internal_GetNormals(mCachedPtr); }
            set { Internal_SetNormals(mCachedPtr, value); }
        }

        /// <summary>
        /// An array of all vertex tangents. Only valid if the vertex layout contains vertex tangents.
        /// </summary>
        public Vector4[] Tangents
        {
            get { return Internal_GetTangents(mCachedPtr); }
            set { Internal_SetTangents(mCachedPtr, value); }
        }

        /// <summary>
        /// An array of all vertex colors. Only valid if the vertex layout contains vertex colors.
        /// </summary>
        public Color[] Colors
        {
            get { return Internal_GetColors(mCachedPtr); }
            set { Internal_SetColors(mCachedPtr, value); }
        }

        /// <summary>
        /// An array of all vertex texture coordinates in the UV0 channel. Only valid if the vertex layout contains UV0 
        /// coordinates.
        /// </summary>
        public Vector2[] UV
        {
            get { return UV0; }
            set { UV0 = value; }
        }

        /// <summary>
        /// An array of all vertex texture coordinates in the UV0 channel. Only valid if the vertex layout contains UV0 
        /// coordinates.
        /// </summary>
        public Vector2[] UV0
        {
            get { return Internal_GetUV0(mCachedPtr); }
            set { Internal_SetUV0(mCachedPtr, value); }
        }

        /// <summary>
        /// An array of all vertex texture coordinates in the UV1 channel. Only valid if the vertex layout contains UV1
        /// coordinates.
        /// </summary>
        public Vector2[] UV1
        {
            get { return Internal_GetUV1(mCachedPtr); }
            set { Internal_SetUV0(mCachedPtr, value); }
        }

        /// <summary>
        /// An array of all vertex bone weights. Only valid if the vertex layout contains bone weights.
        /// </summary>
        public BoneWeight[] BoneWeights
        {
            get { return Internal_GetBoneWeights(mCachedPtr); }
            set { Internal_SetBoneWeights(mCachedPtr, value); }
        }

        /// <summary>
        /// An array of all indices. Make sure that individual entries do not go over the index count as required by
        /// active index type.
        /// </summary>
        public int[] Indices
        {
            get { return Internal_GetIndices(mCachedPtr); }
            set { Internal_SetIndices(mCachedPtr, value); }
        }

        /// <summary>
        /// Number of vertices contained by this object.
        /// </summary>
        public int VertexCount
        {
            get { return Internal_GetVertexCount(mCachedPtr); }
        }

        /// <summary>
        /// Number of indices contained by this object.
        /// </summary>
        public int IndexCount
        {
            get { return Internal_GetIndexCount(mCachedPtr); }
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_CreateInstance(MeshData instance, int numVertices, 
            int numIndices, VertexType vertex, IndexType index);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern Vector3[] Internal_GetPositions(IntPtr thisPtr);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetPositions(IntPtr thisPtr, Vector3[] value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern Vector3[] Internal_GetNormals(IntPtr thisPtr);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetNormals(IntPtr thisPtr, Vector3[] value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern Vector4[] Internal_GetTangents(IntPtr thisPtr);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetTangents(IntPtr thisPtr, Vector4[] value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern Color[] Internal_GetColors(IntPtr thisPtr);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetColors(IntPtr thisPtr, Color[] value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern Vector2[] Internal_GetUV0(IntPtr thisPtr);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetUV0(IntPtr thisPtr, Vector2[] value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern Vector2[] Internal_GetUV1(IntPtr thisPtr);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetUV1(IntPtr thisPtr, Vector2[] value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern BoneWeight[] Internal_GetBoneWeights(IntPtr thisPtr);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetBoneWeights(IntPtr thisPtr, BoneWeight[] value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern int[] Internal_GetIndices(IntPtr thisPtr);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetIndices(IntPtr thisPtr, int[] value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern int Internal_GetVertexCount(IntPtr thisPtr);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern int Internal_GetIndexCount(IntPtr thisPtr);
    }

    /// <summary>
    /// Available vertex layouts that specify what data is provided per-vertex in a mesh. Combinations other than those 
    /// provided are allowed.
    /// </summary>
    public enum VertexType // Note: Must match C++ enum VertexLayout
    {
        Position = 0x01,
        Color = 0x02,
        Normal = 0x04,
        Tangent = 0x08,
        BlendWeights = 0x10,
        UV0 = 0x20,
        UV1 = 0x40,
        PC = Position | Color,
        PU = Position | UV0,
        PCU = Position | Color | UV0,
        PCN = Position | Color | Normal,
        PCNU = Position | Color | Normal | UV0,
        PCNT = Position | Color | Normal | Tangent,
        PCNTU = Position | Color | Normal | Tangent | UV0,
        PN = Position | Normal,
        PNU = Position | Normal | UV0,
        PNT = Position | Normal | Tangent,
        PNTU = Position | Normal | Tangent | UV0,
    }

    /// <summary>
    /// Determines the size of a single index in a mesh.
    /// </summary>
    public enum IndexType // Note: Must match C++ enum ScriptIndexType
    {
        Index16,
        Index32
    }

    /// <summary>
    /// Contains per-vertex bone weights and indexes used for skinning, for up to four bones.
    /// </summary>
    [StructLayout(LayoutKind.Sequential)]
    public struct BoneWeight // Note: Must match C++ class BoneWeight
    {
        public int index0;
        public int index1;
        public int index2;
        public int index3;

        public float weight0;
        public float weight1;
        public float weight2;
        public float weight3;
    }

    /** @} */
}
