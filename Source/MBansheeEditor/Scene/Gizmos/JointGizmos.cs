//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//

using System;
using BansheeEngine;

namespace BansheeEditor
{
    /// <summary>
    /// Handles drawing of gizmos for all the types of <see cref="Joint"/> component.
    /// </summary>
    internal class JointGizmos
    {
        /// <summary>
        /// Returns the anchor position for the specified joint body. Anchor represents the world position of the rigidbody
        /// added to the offset provided by the joint.
        /// </summary>
        /// <param name="joint">Joint from which to retrieve the body.</param>
        /// <param name="body">Body to retrieve the anchor for.</param>
        /// <returns>Anchor position in world space.</returns>
        private static Vector3 GetAnchor(Joint joint, JointBody body)
        {
            Rigidbody rigidbody = joint.GetRigidbody(body);
            Vector3 anchor = joint.GetPosition(body);

            if (rigidbody != null)
            {
                Quaternion worldRot = rigidbody.SceneObject.Rotation;

                anchor = worldRot.Rotate(anchor) + rigidbody.SceneObject.Position;
            }

            return anchor;
        }

        /// <summary>
        /// Method called by the runtime when joints are meant to be drawn.
        /// </summary>
        /// <param name="joint">Joint to draw gizmos for.</param>
        [DrawGizmo(DrawGizmoFlags.Selected | DrawGizmoFlags.ParentSelected)]
        private static void DrawFixedJoint(FixedJoint joint)
        {
            Vector3 anchorA = GetAnchor(joint, JointBody.A);
            Vector3 anchorB = GetAnchor(joint, JointBody.B);

            Gizmos.Color = Color.White;
            Gizmos.DrawSphere(anchorA, 0.05f);
            Gizmos.DrawSphere(anchorB, 0.05f);

            Gizmos.Color = Color.Green;
            Gizmos.DrawLine(anchorA, anchorB);
        }

        /// <summary>
        /// Method called by the runtime when joints are meant to be drawn.
        /// </summary>
        /// <param name="joint">Joint to draw gizmos for.</param>
        [DrawGizmo(DrawGizmoFlags.Selected | DrawGizmoFlags.ParentSelected)]
        private static void DrawDistanceJoint(DistanceJoint joint)
        {
            Vector3 anchorA = GetAnchor(joint, JointBody.A);
            Vector3 anchorB = GetAnchor(joint, JointBody.B);

            Gizmos.Color = Color.White;
            Gizmos.DrawSphere(anchorA, 0.05f);
            Gizmos.DrawSphere(anchorB, 0.05f);

            Gizmos.Color = Color.Red;
            Vector3 diff = anchorB - anchorA;

            float length = diff.Length;
            Vector3 normal = diff.Normalized;

            float min = 0.0f;
            float max = length;

            Vector3 center = anchorA + diff*0.5f;
            if (joint.EnableMinDistanceLimit)
            {
                min = MathEx.Max(0.0f, joint.MinDistance);

                Gizmos.DrawLine(center - normal*min*0.5f, center + normal*min*0.5f);
            }

            if (joint.EnableMaxDistanceLimit)
            {
                max = MathEx.Min(10000.0f, joint.MaxDistance);

                Gizmos.DrawLine(center - normal * max * 0.5f, center - normal * length * 0.5f);
                Gizmos.DrawLine(center + normal * max * 0.5f, center + normal * length * 0.5f);
            }

            Gizmos.Color = Color.Green;
            Gizmos.DrawLine(center - normal * min * 0.5f, center - normal * max * 0.5f);
            Gizmos.DrawLine(center + normal * min * 0.5f, center + normal * max * 0.5f);
        }

        /// <summary>
        /// Method called by the runtime when joints are meant to be drawn.
        /// </summary>
        /// <param name="joint">Joint to draw gizmos for.</param>
        [DrawGizmo(DrawGizmoFlags.Selected | DrawGizmoFlags.ParentSelected)]
        private static void DrawSliderJoint(SliderJoint joint)
        {
            Vector3 anchorA = GetAnchor(joint, JointBody.A);
            Vector3 anchorB = GetAnchor(joint, JointBody.B);

            Gizmos.Color = Color.White;
            Gizmos.DrawSphere(anchorA, 0.05f);
            Gizmos.DrawSphere(anchorB, 0.05f);

            Gizmos.Color = Color.Red;
            Vector3 diff = anchorB - anchorA;

            float length = diff.Length;
            Vector3 normal = diff.Normalized;

            float min = 0.0f;
            float max = length;

            Vector3 center = anchorA + diff * 0.5f;
            if (joint.EnableLimit)
            {
                LimitLinearRange limit = joint.Limit;

                min = MathEx.Max(0.0f, limit.Lower);
                Gizmos.DrawLine(center - normal * min * 0.5f, center + normal * min * 0.5f);

                max = MathEx.Min(10000.0f, limit.Upper);

                Gizmos.DrawLine(center - normal * max * 0.5f, center - normal * length * 0.5f);
                Gizmos.DrawLine(center + normal * max * 0.5f, center + normal * length * 0.5f);
            }

            Gizmos.Color = Color.Green;
            Gizmos.DrawLine(center - normal * min * 0.5f, center - normal * max * 0.5f);
            Gizmos.DrawLine(center + normal * min * 0.5f, center + normal * max * 0.5f);
        }

        /// <summary>
        /// Method called by the runtime when joints are meant to be drawn.
        /// </summary>
        /// <param name="joint">Joint to draw gizmos for.</param>
        [DrawGizmo(DrawGizmoFlags.Selected | DrawGizmoFlags.ParentSelected)]
        private static void DrawSphericalJoint(SphericalJoint joint)
        {
            Vector3 anchorA = GetAnchor(joint, JointBody.A);
            Vector3 anchorB = GetAnchor(joint, JointBody.B);

            Gizmos.Color = Color.White;
            Gizmos.DrawSphere(anchorA, 0.05f);
            Gizmos.DrawSphere(anchorB, 0.05f);

            Gizmos.Color = Color.Green;
            if (joint.EnableLimit)
            {
                LimitConeRange limit = joint.Limit;

                Vector2 scale;
                scale.x = MathEx.Cos(limit.ZLimitAngle * 0.5f);
                scale.y = MathEx.Cos(limit.YLimitAngle * 0.5f);

                Gizmos.Transform = joint.SceneObject.WorldTransform;
                Gizmos.DrawCone(Vector3.Zero, Vector3.XAxis, 1.0f, 1.0f, scale);
            }
            else
            {
                Gizmos.DrawWireSphere(joint.SceneObject.Position, 1.0f);
            }
        }

        /// <summary>
        /// Method called by the runtime when joints are meant to be drawn.
        /// </summary>
        /// <param name="joint">Joint to draw gizmos for.</param>
        [DrawGizmo(DrawGizmoFlags.Selected | DrawGizmoFlags.ParentSelected)]
        private static void DrawHingeJoint(HingeJoint joint)
        {
            Vector3 anchorA = GetAnchor(joint, JointBody.A);
            Vector3 anchorB = GetAnchor(joint, JointBody.B);

            Gizmos.Color = Color.White;
            Gizmos.DrawSphere(anchorA, 0.05f);
            Gizmos.DrawSphere(anchorB, 0.05f);

            if (joint.EnableLimit)
            {
                Gizmos.Transform = joint.SceneObject.WorldTransform;

                LimitAngularRange limit = joint.Limit;

                Action<float> drawLimitedArc = x =>
                {
                    // Arc zero to lower limit
                    Gizmos.Color = Color.Red;
                    Gizmos.DrawWireArc(Vector3.XAxis * x, Vector3.XAxis, 0.25f, -limit.Lower * 0.5f, limit.Lower);

                    // Arc lower to upper limit
                    Degree validRange = limit.Upper - limit.Lower;

                    Gizmos.Color = Color.Green;
                    Gizmos.DrawWireArc(Vector3.XAxis * x, Vector3.XAxis, 0.25f, -limit.Lower - validRange * 0.5f, validRange * 0.5f);
                    Gizmos.DrawWireArc(Vector3.XAxis * x, Vector3.XAxis, 0.25f, limit.Lower * 0.5f, validRange * 0.5f);

                    // Arc upper to full circle
                    Degree remainingRange = new Degree(360) - MathEx.WrapAngle(limit.Upper);

                    Gizmos.DrawWireArc(Vector3.XAxis * x, Vector3.XAxis, 0.25f, -limit.Upper - remainingRange * 0.5f, remainingRange * 0.5f);
                    Gizmos.DrawWireArc(Vector3.XAxis * x, Vector3.XAxis, 0.25f, limit.Upper * 0.5f, remainingRange * 0.5f);
                };

                drawLimitedArc(-0.5f);
                drawLimitedArc(0.5f);
            }
            else
            {
                Gizmos.Color = Color.Green;
                Gizmos.Transform = joint.SceneObject.WorldTransform;

                Gizmos.DrawWireDisc(Vector3.XAxis * -0.5f, Vector3.XAxis, 0.25f);
                Gizmos.DrawWireDisc(Vector3.XAxis * 0.5f, Vector3.XAxis, 0.25f);
            }

            Vector3[] lineStartPoints = new Vector3[4];
            lineStartPoints[0] = new Vector3(-0.5f, 0.25f, 0.25f);
            lineStartPoints[1] = new Vector3(-0.5f, 0.25f, -0.25f);
            lineStartPoints[2] = new Vector3(-0.5f, -0.25f, 0.25f);
            lineStartPoints[3] = new Vector3(-0.5f, -0.25f, -0.25f);

            Vector3[] lineEndPoints = new Vector3[4];
            lineEndPoints[0] = new Vector3(0.5f, 0.25f, 0.25f);
            lineEndPoints[1] = new Vector3(0.5f, 0.25f, -0.25f);
            lineEndPoints[2] = new Vector3(0.5f, -0.25f, 0.25f);
            lineEndPoints[3] = new Vector3(0.5f, -0.25f, -0.25f);

            for (int i = 0; i < 4; i++)
                Gizmos.DrawLine(lineStartPoints[i], lineEndPoints[i]);
        }

        /// <summary>
        /// Method called by the runtime when joints are meant to be drawn.
        /// </summary>
        /// <param name="joint">Joint to draw gizmos for.</param>
        [DrawGizmo(DrawGizmoFlags.Selected | DrawGizmoFlags.ParentSelected)]
        private static void DrawD6Joint(D6Joint joint)
        {
            Vector3 anchorA = GetAnchor(joint, JointBody.A);
            Vector3 anchorB = GetAnchor(joint, JointBody.B);

            Gizmos.Color = Color.White;
            Gizmos.DrawSphere(anchorA, 0.05f);
            Gizmos.DrawSphere(anchorB, 0.05f);
        }
    }
}