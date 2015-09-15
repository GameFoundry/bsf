using System.Runtime.CompilerServices;
using BansheeEngine;

namespace BansheeEditor
{
    /// <summary>
    /// Provides functionality for drawing gizmos. This class should only be used in methods defined with 
    /// <see cref="DrawGizmo"/> attribute.
    /// </summary>
    public class Gizmos
    {
        /// <summary>
        /// Determines the color that will be used on any following draw method.
        /// </summary>
        public static Color Color
        {
            get
            {
                Color value;
                Internal_GetColor(out value); 
                return value; 
            }

            set
            {
                Internal_SetColor(value);
            }
        }

        /// <summary>
        /// Determines the world transform that will be applied to any following draw method.
        /// </summary>
        public static Matrix4 Transform
        {
            get
            {
                Matrix4 value;
                Internal_GetTransform(out value);
                return value; 
            }

            set
            {
                Internal_SetTransform(value);
            }
        }

        /// <summary>
        /// Draws an axis aligned solid cube.
        /// </summary>
        /// <param name="position">World coordinates of the center of the cube.</param>
        /// <param name="extents">Extents defining the half-size of the cube in each dimension.</param>
        public static void DrawCube(Vector3 position, Vector3 extents)
        {
            Internal_DrawCube(position, extents);
        }

        /// <summary>
        /// Draws a solid sphere.
        /// </summary>
        /// <param name="position">World coordinates of the center of the sphere.</param>
        /// <param name="radius">Sphere radius.</param>
        public static void DrawSphere(Vector3 position, float radius)
        {
            Internal_DrawSphere(position, radius);
        }

        /// <summary>
        /// Draws an axis aligned wireframe cube.
        /// </summary>
        /// <param name="position">World coordinates of the center of the cube.</param>
        /// <param name="extents">Extents defining the half-size of the cube in each dimension.</param>
        public static void DrawWireCube(Vector3 position, Vector3 extents)
        {
            Internal_DrawWireCube(position, extents);
        }

        /// <summary>
        /// Draws a wireframe sphere.
        /// </summary>
        /// <param name="position">World coordinates of the center of the sphere.</param>
        /// <param name="radius">Sphere radius.</param>
        public static void DrawWireSphere(Vector3 position, float radius)
        {
            Internal_DrawWireSphere(position, radius);
        }

        /// <summary>
        /// Draws a 3D line.
        /// </summary>
        /// <param name="start">Starting point for the line.</param>
        /// <param name="end">Ending point for the line.</param>
        public static void DrawLine(Vector3 start, Vector3 end)
        {
            Internal_DrawLine(start, end);
        }

        /// <summary>
        /// Draws a wireframe camera frustum.
        /// </summary>
        /// <param name="position">Origin of the frustum (place where the camera origin would normally be).</param>
        /// <param name="aspect">Aspect radio (width/height).</param>
        /// <param name="FOV">Horizontal field of view.</param>
        /// <param name="near">Distance from the origin to the near plane.</param>
        /// <param name="far">Distance from the origin to the far plane.</param>
        public static void DrawFrustum(Vector3 position, float aspect, Degree FOV, float near, float far)
        {
            Internal_DrawFrustum(position, aspect, FOV, near, far);
        }

        /// <summary>
        /// Draws a texture as a camera-facing quad at a specific position.
        /// </summary>
        /// <param name="position">World position of the center of the quad the texture will be drawn on.</param>
        /// <param name="image">Texture to draw.</param>
        /// <param name="fixedScale">If true the icon will remain consistent size regardless of distance from camera. 
        ///                          If false normal perspective foreshortening effect will occurr.</param>
        public static void DrawIcon(Vector3 position, SpriteTexture image, bool fixedScale)
        {
            Internal_DrawIcon(position, image, fixedScale);
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetColor(Color color);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_GetColor(out Color color);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetTransform(Matrix4 transform);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_GetTransform(out Matrix4 transform);

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