using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

namespace BansheeEngine
{
    public class Mesh : Resource
    {
        public Mesh(int numVertices, int numIndices, MeshTopology topology = MeshTopology.TriangleList,
            MeshUsage usage = MeshUsage.Default, VertexType vertex = VertexType.Position, 
            IndexType index = IndexType.Index32)
        {
            SubMesh[] subMeshes = {new SubMesh(0, numIndices, topology)};

            Internal_CreateInstance(this, numVertices, numIndices, subMeshes, usage, vertex, index);
        }

        public Mesh(int numVertices, int numIndices, SubMesh[] subMeshes, MeshUsage usage = MeshUsage.Default,
            VertexType vertex = VertexType.Position, IndexType index = IndexType.Index32)
        {
            Internal_CreateInstance(this, numVertices, numIndices, subMeshes, usage, vertex, index);
        }

        public Mesh(MeshData data, MeshTopology topology = MeshTopology.TriangleList, MeshUsage usage = MeshUsage.Default)
        {
            int numIndices = 0;
            IntPtr dataPtr = IntPtr.Zero;

            if (data != null)
            {
                numIndices = data.IndexCount;
                dataPtr = data.GetCachedPtr();
            }

            SubMesh[] subMeshes = { new SubMesh(0, numIndices, topology) };

            Internal_CreateInstanceMeshData(this, dataPtr, subMeshes, usage);
        }

        public Mesh(MeshData data, SubMesh[] subMeshes, MeshUsage usage = MeshUsage.Default)
        {
            IntPtr dataPtr = IntPtr.Zero;
            if (data != null)
                dataPtr = data.GetCachedPtr();

            Internal_CreateInstanceMeshData(this, dataPtr, subMeshes, usage);
        }

        public int SubMeshCount
        {
            get { return Internal_GetSubMeshCount(mCachedPtr); }
        }

        public SubMesh[] SubMeshes
        {
            get { return Internal_GetSubMeshes(mCachedPtr); }
        }

        public Bounds Bounds
        {
            get
            {
                AABox box;
                Sphere sphere;

                Internal_GetBounds(mCachedPtr, out box, out sphere);

                return new Bounds(box, sphere);
            }
        }

        public MeshData GetMeshData()
        {
            return Internal_GetMeshData(mCachedPtr);
        }

        public void SetMeshData(MeshData data)
        {
            IntPtr dataPtr = IntPtr.Zero;
            if (data != null)
                dataPtr = data.GetCachedPtr();

            Internal_SetMeshData(mCachedPtr, dataPtr);
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_CreateInstance(Mesh instance, int numVertices,
            int numIndices, SubMesh[] subMeshes, MeshUsage usage, VertexType vertex, IndexType index);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_CreateInstanceMeshData(Mesh instance, IntPtr data, SubMesh[] subMeshes, 
            MeshUsage usage);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern SubMesh[] Internal_GetSubMeshes(IntPtr thisPtr);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern int Internal_GetSubMeshCount(IntPtr thisPtr);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_GetBounds(IntPtr thisPtr, out AABox box, out Sphere sphere);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern MeshData Internal_GetMeshData(IntPtr thisPtr);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetMeshData(IntPtr thisPtr, IntPtr value);
    }

    [StructLayout(LayoutKind.Sequential)]
    public struct SubMesh
    {
        public SubMesh(int indexOffset, int indexCount, MeshTopology topology = MeshTopology.TriangleList)
        {
            IndexOffset = indexOffset;
            IndexCount = indexCount;
            Topology = topology;
        }

        public int IndexOffset;
        public int IndexCount;
        public MeshTopology Topology;
    }

    // Note: Values must match C++ enum MeshTopology
    public enum MeshTopology
    {
        PointList = 1,
        LineList = 2,
        LineStrip = 3,
        TriangleList = 4,
        TriangleStrip = 5,
        TriangleFan = 6
    }

    // Note: Do not modify IDs as they must match TextureUsage C++ enum
    public enum MeshUsage
    {
        Default = 0x1,
        Dynamic = 0x2,
        CPUCached = 0x1000
    }
}
