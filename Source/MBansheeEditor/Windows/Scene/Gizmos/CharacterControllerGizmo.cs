//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
using BansheeEngine;

namespace BansheeEditor
{
    /** @addtogroup Gizmos
     *  @{
     */

    /// <summary>
    /// Handles drawing of gizmos for the <see cref="CharacterController"/> component.
    /// </summary>
    internal class CharacterControllerGizmo
    {
        /// <summary>
        /// Method called by the runtime when gizmos are meant to be drawn.
        /// </summary>
        /// <param name="controller">Collider to draw gizmos for.</param>
        [DrawGizmo(DrawGizmoFlags.Selected)]
        private static void Draw(CharacterController controller)
        {
            SceneObject so = controller.SceneObject;

            Quaternion rotation = Quaternion.FromToRotation(Vector3.YAxis, controller.Up);

            // Rotate around origin
            Matrix4 rotMatrix = Matrix4.TRS(Vector3.Zero, rotation, Vector3.One);

            Gizmos.Color = Color.Green;
            Gizmos.Transform = Matrix4.TRS(so.Position, so.Rotation, Vector3.One) * rotMatrix;

            Vector3 scale = so.Scale;
            float scaledHeight = controller.Height * 2.0f * scale.y;
            float scaledRadius = controller.Radius * MathEx.Max(scale.x, scale.z);

            Gizmos.DrawWireCapsule(Vector3.Zero, scaledHeight, scaledRadius);
        }
    }

    /** @} */
}
