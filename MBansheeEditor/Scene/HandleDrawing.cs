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

        public static void DrawCube(Vector3 position, Vector3 extents, float size = 1.0f)
        {
            Internal_DrawCube(position, extents, size);
        }

        public static void DrawSphere(Vector3 position, float radius, float size = 1.0f)
        {
            Internal_DrawSphere(position, radius, size);
        }

        public static void DrawWireCube(Vector3 position, Vector3 extents, float size = 1.0f)
        {
            Internal_DrawWireCube(position, extents, size);
        }

        public static void DrawWireSphere(Vector3 position, float radius, float size = 1.0f)
        {
            Internal_DrawWireSphere(position, radius, size);
        }

        public static void DrawCone(Vector3 coneBase, Vector3 normal, float height, float radius, float size = 1.0f)
        {
            Internal_DrawCone(coneBase, normal, height, radius, size);
        }

        public static void DrawLine(Vector3 start, Vector3 end, float size = 1.0f)
        {
            Internal_DrawLine(start, end, size);
        }

        public static void DrawDisc(Vector3 position, Vector3 normal, float radius, float size = 1.0f)
        {
            Internal_DrawDisc(position, normal, radius, size);
        }

        public static void DrawWireDisc(Vector3 position, Vector3 normal, float radius, float size = 1.0f)
        {
            Internal_DrawWireDisc(position, normal, radius, size);
        }

        public static void DrawArc(Vector3 position, Vector3 normal, float radius, Degree startAngle, Degree amountAngle, float size = 1.0f)
        {
            Internal_DrawArc(position, normal, radius, startAngle, amountAngle, size);
        }

        public static void DrawWireArc(Vector3 position, Vector3 normal, float radius, Degree startAngle, Degree amountAngle, float size = 1.0f)
        {
            Internal_DrawWireArc(position, normal, radius, startAngle, amountAngle, size);
        }

        public static void DrawRect(Rect3 area, float size = 1.0f)
        {
            Internal_DrawRect(area.Center, area.AxisHorz, area.AxisVert, area.ExtentHorz, area.ExtentVert, size);
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetColor(Color color);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetTransform(Matrix4 transform);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_DrawCube(Vector3 position, Vector3 extents, float size);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_DrawSphere(Vector3 position, float radius, float size);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_DrawWireCube(Vector3 position, Vector3 extents, float size);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_DrawWireSphere(Vector3 position, float radius, float size);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_DrawCone(Vector3 coneBase, Vector3 normal, float height, float radius, float size);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_DrawLine(Vector3 start, Vector3 end, float size);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_DrawDisc(Vector3 position, Vector3 normal, float radius, float size);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_DrawWireDisc(Vector3 position, Vector3 normal, float radius, float size);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_DrawArc(Vector3 position, Vector3 normal, float radius, Degree startAngle, Degree amountAngle, float size);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_DrawWireArc(Vector3 position, Vector3 normal, float radius, Degree startAngle, Degree amountAngle, float size);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_DrawRect(Vector3 center, Vector3 axisH, Vector3 axisV, float extentH, float extentV, float size);
    }
}
