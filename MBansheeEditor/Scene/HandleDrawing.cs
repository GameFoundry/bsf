using System.Runtime.CompilerServices;
using BansheeEngine;

namespace BansheeEditor
{
    public sealed class HandleDrawing
    {
        public static void SetColor(Color color)
        {
            Internal_SetColor(color);    
        }

        public static void SetTransform(Matrix4 transform)
        {
            Internal_SetTransform(transform);
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

        public static void DrawCone(Vector3 coneBase, Vector3 normal, float height, float radius)
        {
            Internal_DrawCone(coneBase, normal, height, radius);
        }

        public static void DrawLine(Vector3 start, Vector3 end)
        {
            Internal_DrawLine(start, end);
        }

        public static void DrawDisc(Vector3 position, Vector3 normal, float radius)
        {
            Internal_DrawDisc(position, normal, radius);
        }

        public static void DrawWireDisc(Vector3 position, Vector3 normal, float radius)
        {
            Internal_DrawWireDisc(position, normal, radius);
        }

        public static void DrawArc(Vector3 position, Vector3 normal, float radius, Degree startAngle, Degree amountAngle)
        {
            Internal_DrawArc(position, normal, radius, startAngle, amountAngle);
        }

        public static void DrawWireArc(Vector3 position, Vector3 normal, float radius, Degree startAngle, Degree amountAngle)
        {
            Internal_DrawWireArc(position, normal, radius, startAngle, amountAngle);
        }

        public static void DrawRect(Rect3 area)
        {
            Internal_DrawRect(area);
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
        private static extern void Internal_DrawCone(Vector3 coneBase, Vector3 normal, float height, float radius);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_DrawLine(Vector3 start, Vector3 end);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_DrawDisc(Vector3 position, Vector3 normal, float radius);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_DrawWireDisc(Vector3 position, Vector3 normal, float radius);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_DrawArc(Vector3 position, Vector3 normal, float radius, Degree startAngle, Degree amountAngle);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_DrawWireArc(Vector3 position, Vector3 normal, float radius, Degree startAngle, Degree amountAngle);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_DrawRect(Rect3 area);
    }
}
