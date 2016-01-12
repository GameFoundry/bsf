using System.Runtime.CompilerServices;
using BansheeEngine;
using System;

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
            set { Internal_SetColor(ref value); }
        }

        /// <summary>
        /// Determines the world transform that will be applied to any following draw method.
        /// </summary>
        public static Matrix4 Transform
        {
            set { Internal_SetTransform(ref value); }
        }

        /// <summary>
        /// Layer bitfield that controls whether a handle is considered visible in a specific camera. Handle layer 
        /// must match camera layer in order for the camera to render it.
        /// </summary>
        public static UInt64 Layer
        {
            set { Internal_SetLayer(value); }
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
            Internal_DrawCube(ref position, ref extents, size);
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
            Internal_DrawSphere(ref position, radius, size);
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
            Internal_DrawWireCube(ref position, ref extents, size);
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
            Internal_DrawWireSphere(ref position, radius, size);
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
            Internal_DrawCone(ref coneBase, ref normal, height, radius, size);
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
            Internal_DrawLine(ref start, ref end, size);
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
            Internal_DrawDisc(ref position, ref normal, radius, size);
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
            Internal_DrawWireDisc(ref position, ref normal, radius, size);
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
        public static void DrawArc(Vector3 position, Vector3 normal, float radius, Degree startAngle, Degree amountAngle,
            float size = 1.0f)
        {
            Internal_DrawArc(ref position, ref normal, radius, ref startAngle, ref amountAngle, size);
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
        public static void DrawWireArc(Vector3 position, Vector3 normal, float radius, Degree startAngle,
            Degree amountAngle, float size = 1.0f)
        {
            Internal_DrawWireArc(ref position, ref normal, radius, ref startAngle, ref amountAngle, size);
        }

        /// <summary>
        /// Draws a single-sided rectangle in 3D.
        /// </summary>
        /// <param name="area">Determines the position, orientation and size of the rectangle.</param>
        /// <param name="size">Uniform scale to apply on top of the existing transform. Primarily used for maintaining
        ///                    handle size regardless of distance from camera.</param>
        public static void DrawRect(Rect3 area, float size = 1.0f)
        {
            Vector3 center = area.Center;
            Vector3 axisHorz = area.AxisHorz;
            Vector3 axisVert = area.AxisVert;

            Internal_DrawRect(ref center, ref axisHorz, ref axisVert, area.ExtentHorz, area.ExtentVert, size);
        }

        /// <summary>
        /// Draws camera aligned text at the specified position.
        /// </summary>
        /// <param name="position">World position to center the text on.</param>
        /// <param name="text">String to draw.</param>
        /// <param name="font">Font used for drawing the characters.</param>
        /// <param name="fontSize">Size of the characters, in points.</param>
        /// <param name="size">Uniform scale to apply on top of the existing transform. Primarily used for maintaining
        ///                    handle size regardless of distance from camera.</param>
        public static void DrawText(Vector3 position, string text, Font font = null, int fontSize = 16, float size = 1.0f)
        {
            IntPtr scriptFont = IntPtr.Zero;
            if (font != null)
                scriptFont = font.GetCachedPtr();

            Internal_DrawText(ref position, text, scriptFont, fontSize, size);
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetColor(ref Color color);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetTransform(ref Matrix4 transform);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetLayer(UInt64 layer);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_DrawCube(ref Vector3 position, ref Vector3 extents, float size);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_DrawSphere(ref Vector3 position, float radius, float size);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_DrawWireCube(ref Vector3 position, ref Vector3 extents, float size);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_DrawWireSphere(ref Vector3 position, float radius, float size);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_DrawCone(ref Vector3 coneBase, ref Vector3 normal, float height, float radius, float size);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_DrawLine(ref Vector3 start, ref Vector3 end, float size);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_DrawDisc(ref Vector3 position, ref Vector3 normal, float radius, float size);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_DrawWireDisc(ref Vector3 position, ref Vector3 normal, float radius, float size);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_DrawArc(ref Vector3 position, ref Vector3 normal, float radius, ref Degree startAngle, ref Degree amountAngle, float size);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_DrawWireArc(ref Vector3 position, ref Vector3 normal, float radius, ref Degree startAngle, ref Degree amountAngle, float size);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_DrawRect(ref Vector3 center, ref Vector3 axisH, ref Vector3 axisV, float extentH, float extentV, float size);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_DrawText(ref Vector3 position, string text, IntPtr font, int fontSize, float size);
    }
}
