using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

namespace BansheeEngine
{
    public class MeshData : ScriptObject
    {
        public MeshData(int numVertices, int numIndices, VertexType vertex = VertexType.Position,
            IndexType index = IndexType.Index32)
        {
            Internal_CreateInstance(this, numVertices, numIndices, vertex, index);
        }

        public Vector3[] Positions
        {
            get { return Internal_GetPositions(mCachedPtr); }
            set { Internal_SetPositions(mCachedPtr, value); }
        }

        public Vector3[] Normals
        {
            get { return Internal_GetNormals(mCachedPtr); }
            set { Internal_SetNormals(mCachedPtr, value); }
        }

        public Vector4[] Tangents
        {
            get { return Internal_GetTangents(mCachedPtr); }
            set { Internal_SetTangents(mCachedPtr, value); }
        }

        public Color[] Colors
        {
            get { return Internal_GetColors(mCachedPtr); }
            set { Internal_SetColors(mCachedPtr, value); }
        }

        public Vector2[] UV
        {
            get { return UV0; }
            set { UV0 = value; }
        }

        public Vector2[] UV0
        {
            get { return Internal_GetUV0(mCachedPtr); }
            set { Internal_SetUV0(mCachedPtr, value); }
        }

        public Vector2[] UV1
        {
            get { return Internal_GetUV1(mCachedPtr); }
            set { Internal_SetUV0(mCachedPtr, value); }
        }

        public BoneWeight[] BoneWeights
        {
            get { return Internal_GetBoneWeights(mCachedPtr); }
            set { Internal_SetBoneWeights(mCachedPtr, value); }
        }

        public int[] Indices
        {
            get { return Internal_GetIndices(mCachedPtr); }
            set { Internal_SetIndices(mCachedPtr, value); }
        }

        public int VertexCount
        {
            get { return Internal_GetVertexCount(mCachedPtr); }
        }

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

    // Note: Do not modify, it must match the layout of C++ enum VertexLayout
    public enum VertexType
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

    // Note: Do not modify, it must match the layout of C++ enum ScriptIndexType
    public enum IndexType
    {
        Index16,
        Index32
    }

    [StructLayout(LayoutKind.Sequential)]
    public struct BoneWeight
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
}
