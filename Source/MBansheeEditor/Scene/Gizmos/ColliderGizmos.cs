//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
using BansheeEngine;

namespace BansheeEditor
{
    /// <summary>
    /// Handles drawing of gizmos for all the types of <see cref="Collider"/> component.
    /// </summary>
    internal class ColliderGizmos
    {
        /// <summary>
        /// Method called by the runtime when gizmos are meant to be drawn.
        /// </summary>
        /// <param name="collider">Collider to draw gizmos for.</param>
        [DrawGizmo(DrawGizmoFlags.Selected | DrawGizmoFlags.ParentSelected)]
        private static void DrawBoxCollider(BoxCollider collider)
        {
            SceneObject so = collider.SceneObject;

            Gizmos.Color = Color.Green;
            Gizmos.Transform = so.WorldTransform;

            Vector3 scaledExtents = collider.Extents*so.Scale;
            Gizmos.DrawWireCube(so.WorldTransform.MultiplyAffine(collider.Center), scaledExtents);
        }

        /// <summary>
        /// Method called by the runtime when gizmos are meant to be drawn.
        /// </summary>
        /// <param name="collider">Collider to draw gizmos for.</param>
        [DrawGizmo(DrawGizmoFlags.Selected | DrawGizmoFlags.ParentSelected)]
        private static void DrawSphereCollider(SphereCollider collider)
        {
            SceneObject so = collider.SceneObject;

            Gizmos.Color = Color.Green;
            Gizmos.Transform = so.WorldTransform;

            Vector3 scale = so.Scale;
            float scaledRadius = collider.Radius * MathEx.Max(scale.x, scale.y, scale.z);

            Gizmos.DrawWireSphere(so.WorldTransform.MultiplyAffine(collider.Center), scaledRadius);
        }

        /// <summary>
        /// Method called by the runtime when gizmos are meant to be drawn.
        /// </summary>
        /// <param name="collider">Collider to draw gizmos for.</param>
        [DrawGizmo(DrawGizmoFlags.Selected | DrawGizmoFlags.ParentSelected)]
        private static void DrawCapsuleCollider(CapsuleCollider collider)
        {
            SceneObject so = collider.SceneObject;

            Vector3 offset = collider.Center;
            Quaternion rotation = Quaternion.FromToRotation(Vector3.YAxis, collider.Normal);

            // Rotate around origin
            Matrix4 rotMatrix = Matrix4.TRS(offset, Quaternion.Identity, Vector3.One) * 
                Matrix4.TRS(Vector3.Zero, rotation, Vector3.One) * 
                Matrix4.TRS(-offset, Quaternion.Identity, Vector3.One);

            Gizmos.Color = Color.Green;
            Gizmos.Transform = so.WorldTransform * rotMatrix;

            Vector3 scale = so.Scale;
            float scaledHeight = collider.HalfHeight*2.0f*scale.y;
            float scaledRadius = collider.Radius*MathEx.Max(scale.x, scale.z);

            Gizmos.DrawWireCapsule(offset, scaledHeight, scaledRadius);
        }

        /// <summary>
        /// Method called by the runtime when gizmos are meant to be drawn.
        /// </summary>
        /// <param name="collider">Collider to draw gizmos for.</param>
        [DrawGizmo(DrawGizmoFlags.Selected | DrawGizmoFlags.ParentSelected)]
        private static void DrawMeshCollider(MeshCollider collider)
        {
            PhysicsMesh mesh = collider.Mesh;
            if (mesh == null)
                return;

            SceneObject so = collider.SceneObject;

            Gizmos.Color = Color.Green;
            Gizmos.Transform = so.WorldTransform;

            MeshData meshData = mesh.MeshData;
            Gizmos.DrawWireMesh(meshData);
        }

        /// <summary>
        /// Method called by the runtime when gizmos are meant to be drawn.
        /// </summary>
        /// <param name="collider">Collider to draw gizmos for.</param>
        [DrawGizmo(DrawGizmoFlags.Selected | DrawGizmoFlags.ParentSelected)]
        private static void DrawPlaneCollider(PlaneCollider collider)
        {
            SceneObject so = collider.SceneObject;

            Vector3 normal = collider.Normal;
            Vector3 center = normal * collider.Distance;

            // Rotate around origin
            Quaternion rotation = Quaternion.FromToRotation(Vector3.XAxis, normal);
            Matrix4 rotMatrix = Matrix4.TRS(center, Quaternion.Identity, Vector3.One) *
                Matrix4.TRS(Vector3.Zero, rotation, Vector3.One) *
                Matrix4.TRS(-center, Quaternion.Identity, Vector3.One);

            Gizmos.Color = Color.Green;
            Gizmos.Transform = so.WorldTransform * rotMatrix;

            Vector3 bottomLeft = new Vector3(0.0f, -0.5f, -0.5f);
            Vector3 topLeft = new Vector3(0.0f, 0.5f, -0.5f);
            Vector3 topRight = new Vector3(0.0f, 0.5f, 0.5f);
            Vector3 bottomRight = new Vector3(0.0f, -0.5f, 0.5f);

            bottomLeft += center;
            topLeft += center;
            topRight += center;
            bottomRight += center;

            // Draw sides
            Gizmos.DrawLine(bottomLeft, topLeft);
            Gizmos.DrawLine(topLeft, topRight);
            Gizmos.DrawLine(topRight, bottomRight);
            Gizmos.DrawLine(bottomRight, bottomLeft);

            // Draw normal
            Gizmos.DrawLine(center, center + Vector3.XAxis*0.5f);
        }
    }
}
