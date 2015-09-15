using System.Runtime.CompilerServices;
using BansheeEngine;

namespace BansheeEditor
{
    /// <summary>
    /// Contains various method that can be used for drawing handles. These methods should only be called from 
    /// <see cref="Handle.Draw"/> method or its overrides.
    /// </summary>
    public sealed class HandleDrawing
    {
        /// <summary>
        /// Determines the color that will be used on any following draw method.
        /// </summary>
        public static Color Color
        {
            set { Internal_SetColor(value); }
        }

        /// <summary>
        /// Determines the world transform that will be applied to any following draw method.
        /// </summary>
        public static Matrix4 Transform
        {
            set { Internal_SetTransform(value); }
        }

        /// <summary>
        /// Draws an axis aligned solid cube.
        /// </summary>
        /// <param name="position">World coordinates of the center of the cube.</param>
        /// <param name="extents">Extents defining the half-size of the cube in each dimension.</param>
        /// <param name="size">Uniform scale to apply on top of the existing transform. Primarily used for maintaining 
        ///                    handle size regardless of distance from camera.</param>
        public static void DrawCube(Vector3 position, Vector3 extents, float size = 1.0f)
        {
            Internal_DrawCube(position, extents, size);
        }

        /// <summary>
        /// Draws a solid sphere.
        /// </summary>
        /// <param name="position">World coordinates of the center of the sphere.</param>
        /// <param name="radius">Sphere radius.</param>
        /// <param name="size">Uniform scale to apply on top of the existing transform. Primarily used for maintaining
        ///                    handle size regardless of distance from camera.</param>
        public static void DrawSphere(Vector3 position, float radius, float size = 1.0f)
        {
            Internal_DrawSphere(position, radius, size);
        }

        /// <summary>
        /// Draws an axis aligned wireframe cube.
        /// </summary>
        /// <param name="position">World coordinates of the center of the cube.</param>
        /// <param name="extents">Extents defining the half-size of the cube in each dimension.</param>
        /// <param name="size">Uniform scale to apply on top of the existing transform. Primarily used for maintaining
        ///                    handle size regardless of distance from camera.</param>
        public static void DrawWireCube(Vector3 position, Vector3 extents, float size = 1.0f)
        {
            Internal_DrawWireCube(position, extents, size);
        }

        /// <summary>
        /// Draws a wireframe sphere.
        /// </summary>
        /// <param name="position">World coordinates of the center of the sphere.</param>
        /// <param name="radius">Sphere radius.</param>
        /// <param name="size">Uniform scale to apply on top of the existing transform. Primarily used for maintaining
        ///                    handle size regardless of distance from camera.</param>
        public static void DrawWireSphere(Vector3 position, float radius, float size = 1.0f)
        {
            Internal_DrawWireSphere(position, radius, size);
        }

        /// <summary>
        /// Draws a solid cone.
        /// </summary>
        /// <param name="coneBase">Location of the cone base (center of the cone disc).</param>
        /// <param name="normal">Normal pointing from the base to the cone point.</param>
        /// <param name="height">Distance from the origin to the cone point.</param>
        /// <param name="radius">Radius of the cone disc.</param>
        /// <param name="size">Uniform scale to apply on top of the existing transform. Primarily used for maintaining
        ///                    handle size regardless of distance from camera.</param>
        public static void DrawCone(Vector3 coneBase, Vector3 normal, float height, float radius, float size = 1.0f)
        {
            Internal_DrawCone(coneBase, normal, height, radius, size);
        }

        /// <summary>
        /// Draws a 3D line.
        /// </summary>
        /// <param name="start">Starting point for the line.</param>
        /// <param name="end">Ending point for the line.</param>
        /// <param name="size">Uniform scale to apply on top of the existing transform. Primarily used for maintaining
        ///                    handle size regardless of distance from camera.</param>
        public static void DrawLine(Vector3 start, Vector3 end, float size = 1.0f)
        {
            Internal_DrawLine(start, end, size);
        }

        /// <summary>
        /// Draws a solid double-sided disc.
        /// </summary>
        /// <param name="position">Center of the disc.</param>
        /// <param name="normal">Normal towards which to orient the disc.</param>
        /// <param name="radius">Radius of the disc.</param>
        /// <param name="size">Uniform scale to apply on top of the existing transform. Primarily used for maintaining
        ///                    handle size regardless of distance from camera.</param>
        public static void DrawDisc(Vector3 position, Vector3 normal, float radius, float size = 1.0f)
        {
            Internal_DrawDisc(position, normal, radius, size);
        }

        /// <summary>
        /// Draws a wireframe disc.
        /// </summary>
        /// <param name="position">Center of the disc.</param>
        /// <param name="normal">Normal towards which to orient the disc.</param>
        /// <param name="radius">Radius of the disc.</param>
        /// <param name="size">Uniform scale to apply on top of the existing transform. Primarily used for maintaining
        ///                    handle size regardless of distance from camera.</param>
        public static void DrawWireDisc(Vector3 position, Vector3 normal, float radius, float size = 1.0f)
        {
            Internal_DrawWireDisc(position, normal, radius, size);
        }

        /// <summary>
        /// Draws a solid double-sided arc.
        /// </summary>
        /// <param name="position">Center of the disc out of which the arc is cut out of.</param>
        /// <param name="normal">Normal towards which to orient the arc.</param>
        /// <param name="radius">Radius of the  disc out of which the arc is cut out of.</param>
        /// <param name="startAngle">Angle at which the arc starts.</param>
        /// <param name="amountAngle">Length of the arc.</param>
        /// <param name="size">Uniform scale to apply on top of the existing transform. Primarily used for maintaining
        ///                    handle size regardless of distance from camera.</param>
        public static void DrawArc(Vector3 position, Vector3 normal, float radius, Degree startAngle, Degree amountAngle, float size = 1.0f)
        {
            Internal_DrawArc(position, normal, radius, startAngle, amountAngle, size);
        }

        /// <summary>
        /// Draws a wireframe arc.
        /// </summary>
        /// <param name="position">Center of the disc out of which the arc is cut out of.</param>
        /// <param name="normal">Normal towards which to orient the arc.</param>
        /// <param name="radius">Radius of the  disc out of which the arc is cut out of.</param>
        /// <param name="startAngle">Angle at which the arc starts.</param>
        /// <param name="amountAngle">Length of the arc.</param>
        /// <param name="size">Uniform scale to apply on top of the existing transform. Primarily used for maintaining
        ///                    handle size regardless of distance from camera.</param>
        public static void DrawWireArc(Vector3 position, Vector3 normal, float radius, Degree startAngle, Degree amountAngle, float size = 1.0f)
        {
            Internal_DrawWireArc(position, normal, radius, startAngle, amountAngle, size);
        }

        /// <summary>
        /// Draws a single-sided rectangle in 3D.
        /// </summary>
        /// <param name="area">Determines the position, orientation and size of the rectangle.</param>
        /// <param name="size">Uniform scale to apply on top of the existing transform. Primarily used for maintaining
        ///                    handle size regardless of distance from camera.</param>
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
