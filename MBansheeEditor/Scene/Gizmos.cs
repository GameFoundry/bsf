using System.Runtime.CompilerServices;
using BansheeEngine;

namespace BansheeEditor
{
    public class Gizmos
    {
        private static Color _color;
        private static Matrix4 _transform;

        public static Color color
        {
            get { return _color; }
            set { _color = value; Internal_SetColor(_color); }
        }

        public static Matrix4 transform
        {
            get { return _transform; }
            set { _transform = value; Internal_SetTransform(_transform); }
        }

        public static void DrawCube(Vector3 position, Vector3 extents)
        {
            Internal_DrawCube(position, extents);
        }

        public static void DrawSphere(Vector3 position, float radius)
        {
            Internal_DrawSphere(position, radius);
        }

        public static void DrawWireCube(Vector3 position, Vector3 extents)
        {
            Internal_DrawWireCube(position, extents);
        }

        public static void DrawWireSphere(Vector3 position, float radius)
        {
            Internal_DrawWireSphere(position, radius);
        }

        public static void DrawLine(Vector3 start, Vector3 end)
        {
            Internal_DrawLine(start, end);
        }

        public static void DrawFrustum(Vector3 position, float aspect, Degree FOV, float near, float far)
        {
            Internal_DrawFrustum(position, aspect, FOV, near, far);
        }

        public static void DrawIcon(Vector3 position, SpriteTexture image, bool fixedScale)
        {
            Internal_DrawIcon(position, image, fixedScale);
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetColor(Color color);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetTransform(Matrix4 transform);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_DrawCube(Vector3 position, Vector3 extents);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_DrawSphere(Vector3 position, float radius);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_DrawWireCube(Vector3 position, Vector3 extents);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_DrawWireSphere(Vector3 position, float radius);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_DrawLine(Vector3 start, Vector3 end);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_DrawFrustum(Vector3 position, float aspect, Degree FOV, float near, float far);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_DrawIcon(Vector3 position, SpriteTexture image, bool fixedScale);
    }
}