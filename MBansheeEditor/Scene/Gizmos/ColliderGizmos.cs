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

            Vector3 offset = so.WorldTransform.MultiplyAffine(collider.Center);
            Quaternion rotation = Quaternion.FromToRotation(Vector3.YAxis, collider.Normal);
            Gizmos.Transform = Matrix4.TRS(-offset, rotation, Vector3.One);

            Gizmos.Color = Color.Green;

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

            Gizmos.Transform = so.WorldTransform;
            Gizmos.Color = Color.Green;

            MeshData meshData = mesh.MeshData;

            int numTriangles = meshData.IndexCount/3;
            int[] indices = meshData.Indices;
            Vector3[] vertices = meshData.Positions;
            Vector3[] linePoints = new Vector3[numTriangles*6];

            for (int i = 0; i < numTriangles; i++)
            {
                int lineIdx = i*6;

                linePoints[lineIdx + 0] = vertices[indices[i * 3 + 0]];
                linePoints[lineIdx + 1] = vertices[indices[i * 3 + 1]];

                linePoints[lineIdx + 2] = vertices[indices[i * 3 + 1]];
                linePoints[lineIdx + 3] = vertices[indices[i * 3 + 2]];

                linePoints[lineIdx + 4] = vertices[indices[i * 3 + 2]];
                linePoints[lineIdx + 5] = vertices[indices[i * 3 + 0]];
            }

            Gizmos.DrawLineList(linePoints);
        }

        /// <summary>
        /// Method called by the runtime when gizmos are meant to be drawn.
        /// </summary>
        /// <param name="collider">Collider to draw gizmos for.</param>
        [DrawGizmo(DrawGizmoFlags.Selected | DrawGizmoFlags.ParentSelected)]
        private static void DrawPlaneCollider(PlaneCollider collider)
        {
            SceneObject so = collider.SceneObject;

            Gizmos.Color = Color.Green;

            Vector3 localNormal = collider.Normal;
            Vector3 localCenter = localNormal * collider.Distance;

            Vector3 bottomLeft = new Vector3(0.0f, -0.5f, -0.5f);
            Vector3 topLeft = new Vector3(0.0f, 0.5f, -0.5f);
            Vector3 topRight = new Vector3(0.0f, 0.5f, 0.5f);
            Vector3 bottomRight = new Vector3(0.0f, -0.5f, 0.5f);

            bottomLeft += localCenter;
            topLeft += localCenter;
            topRight += localCenter;
            bottomRight += localCenter;

            bottomLeft = so.WorldTransform.MultiplyAffine(bottomLeft);
            topLeft = so.WorldTransform.MultiplyAffine(topLeft);
            topRight = so.WorldTransform.MultiplyAffine(topRight);
            bottomRight = so.WorldTransform.MultiplyAffine(bottomRight);

            Vector3 center = so.WorldTransform.MultiplyAffine(localCenter);
            Vector3 normal = localNormal;
            normal = so.WorldTransform.MultiplyDirection(normal);

            // Draw sides
            Gizmos.DrawLine(bottomLeft, topLeft);
            Gizmos.DrawLine(topLeft, topRight);
            Gizmos.DrawLine(topRight, bottomRight);
            Gizmos.DrawLine(bottomRight, bottomLeft);

            // Draw normal
            Gizmos.DrawLine(center, center + normal*0.5f);
        }
    }
}
