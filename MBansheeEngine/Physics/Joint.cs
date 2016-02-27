//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//

using System;
using System.Runtime.InteropServices;

namespace BansheeEngine
{
    /// <summary>
    /// Base class for all Joint types. Joints constrain how two rigidbodies move relative to one another (e.g. a door 
    /// hinge). One of the bodies in the joint must always be movable (i.e. non-kinematic).
    /// </summary>
    public abstract class Joint : Component
    {
        internal NativeJoint native;

        [SerializeField]
        internal SerializableData serializableData = new SerializableData();

        /// <summary>
        /// Triggered when the joint's break force or torque is exceeded.
        /// </summary>
        public event Action OnJointBreak;

        /// <summary>
        /// Maximum force the joint can apply before breaking. Broken joints no longer participate in physics simulation.
        /// </summary>
        public float BreakForce
        {
            get { return serializableData.breakForce; }
            set
            {
                if (serializableData.breakForce == value)
                    return;

                serializableData.breakForce = value;

                if (native != null)
                    native.BreakForce = value;
            }
        }

        /// <summary>
        /// Sets the maximum force the joint can apply before breaking. Broken joints no longer participate in physics
        /// simulation.
        /// </summary>
        public float BreakTorque
        {
            get { return serializableData.breakTorque; }
            set
            {
                if (serializableData.breakTorque == value)
                    return;

                serializableData.breakTorque = value;

                if (native != null)
                    native.BreakTorque = value;
            }
        }

        /// <summary>
        /// Determines whether collisions between the two bodies managed by the joint are enabled.
        /// </summary>
        public bool EnableCollision
        {
            get { return serializableData.enableCollision; }
            set
            {
                if (serializableData.enableCollision == value)
                    return;

                serializableData.enableCollision = value;

                if (native != null)
                    native.EnableCollision = value;
            }
        }

        /// <summary>
        /// Returns one of the bodies managed by the joint.
        /// </summary>
        /// <param name="body">Which of the rigidbodies to return.</param>
        /// <returns>Rigidbody managed by the joint, or null if none.</returns>
        public Rigidbody GetRigidbody(JointBody body)
        {
            return serializableData.bodies[(int) body];
        }

        /// <summary>
        /// Sets a body managed by the joint. One of the bodies must be movable (i.e. non-kinematic).
        /// </summary>
        /// <param name="body">Which of the rigidbodies to set.</param>
        /// <param name="rigidbody">Rigidbody to managed by the joint, or null. If one of the bodies is null the other
        ///                         one will be anchored globally to the position/rotation set by <see cref="SetPosition"/>
        ///                         and <see cref="SetRotation"/>.</param>
        public void SetRigidbody(JointBody body, Rigidbody rigidbody)
        {
            if (serializableData.bodies[(int)body] == rigidbody)
                return;

            if (serializableData.bodies[(int)body] != null)
                serializableData.bodies[(int)body].SetJoint(null);

            serializableData.bodies[(int)body] = rigidbody;

            if (rigidbody != null)
                serializableData.bodies[(int)body].SetJoint(this);

            if (native != null)
            {
                native.SetRigidbody(body, rigidbody);
                UpdateTransform(body);
            }
        }

        /// <summary>
        /// Returns the position at which the body is anchored to the joint.
        /// </summary>
        /// <param name="body">Which body to retrieve position for.</param>
        /// <returns>Position relative to the body.</returns>
        public Vector3 GetPosition(JointBody body)
        {
            return serializableData.positions[(int)body];
        }

        /// <summary>
        /// Sets the position at which the body is anchored to the joint.
        /// </summary>
        /// <param name="body">Which body set the position for.</param>
        /// <param name="position">Position relative to the body.</param>
        public void SetPosition(JointBody body, Vector3 position)
        {
            if (serializableData.positions[(int)body] == position)
                return;

            serializableData.positions[(int) body] = position;

            if (native != null)
                UpdateTransform(body);
        }

        /// <summary>
        /// Returns the rotation at which the body is anchored to the joint.
        /// </summary>
        /// <param name="body">Which body to retrieve rotation for.</param>
        /// <returns>Rotation relative to the body.</returns>
        public Quaternion GetRotation(JointBody body)
        {
            return serializableData.rotations[(int)body];
        }

        /// <summary>
        /// Sets the rotation at which the body is anchored to the joint.
        /// </summary>
        /// <param name="body">Which body set the rotation for.</param>
        /// <param name="rotation">Rotation relative to the body.</param>
        public void SetRotation(JointBody body, Quaternion rotation)
        {
            if (serializableData.rotations[(int)body] == rotation)
                return;

            serializableData.rotations[(int)body] = rotation;

            if (native != null)
                UpdateTransform(body);
        }

        /// <summary>
        /// Triggered when the joint breaks.
        /// </summary>
        internal void DoOnJointBreak()
        {
            if (OnJointBreak != null)
                OnJointBreak();
        }

        /// <summary>
        /// Notifies the joint that one of the attached rigidbodies moved and that its transform needs updating.
        /// </summary>
        /// <param name="body">Rigidbody that moved.</param>
	    internal void NotifyRigidbodyMoved(Rigidbody body)
	    {
		    // If physics update is in progress do nothing, as its the joint itself that's probably moving the body
		    if (Physics.IsUpdateInProgress)
			    return;

		    if (serializableData.bodies[0] == body)
			    UpdateTransform(JointBody.A);
		    else if (serializableData.bodies[1] == body)
			    UpdateTransform(JointBody.B);
	    }

        /// <summary>
        /// Creates the internal representation of the Joint for use by the component.
        /// </summary>
        /// <returns>New native joint object.</returns>
        internal abstract NativeJoint CreateNative();

        private void OnInitialize()
        {
            NotifyFlags = TransformChangedFlags.Transform | TransformChangedFlags.Parent;
        }

        private void OnReset()
        {
            RestoreNative();
        }

        private void OnEnable()
        {
            if (native == null)
                RestoreNative();
        }

        private void OnDisable()
        {
            DestroyNative();
        }

        private void OnDestroy()
        {
            if (serializableData.bodies[0] != null)
                serializableData.bodies[0].SetJoint(null);

            if (serializableData.bodies[1] != null)
                serializableData.bodies[1].SetJoint(null);

            DestroyNative();
        }

        private void OnTransformChanged(TransformChangedFlags flags)
        {
            if (!SceneObject.Active)
                return;

            // We're ignoring this during physics update because it would cause problems if the joint itself was moved by physics
            // Note: This isn't particularily correct because if the joint is being moved by physics but the rigidbodies
            // themselves are not parented to the joint, the transform will need updating. However I'm leaving it up to the
            // user to ensure rigidbodies are always parented to the joint in such a case (It's an unlikely situation that
            // I can't think of an use for - joint transform will almost always be set as an initialization step and not a 
            // physics response).
            if (Physics.IsUpdateInProgress)
                return;

            UpdateTransform(JointBody.A);
            UpdateTransform(JointBody.B);
        }

        /// <summary>
        /// Creates the internal representation of the Joint and restores the values saved by the Component.
        /// </summary>
        private void RestoreNative()
	    {
            native = CreateNative();

            // Note: Merge into one call to avoid many virtual function calls
            Rigidbody[] bodies = new Rigidbody[2];

		    if (serializableData.bodies[0] != null)
			    bodies[0] = serializableData.bodies[0];
		    else
			    bodies[0] = null;

		    if (serializableData.bodies[1] != null)
			    bodies[1] = serializableData.bodies[1];
		    else
			    bodies[1] = null;

		    native.SetRigidbody(JointBody.A, bodies[0]);
		    native.SetRigidbody(JointBody.B, bodies[1]);
		    native.BreakForce = serializableData.breakForce;
            native.BreakTorque = serializableData.breakTorque;
            native.EnableCollision = serializableData.enableCollision;
		    native.BreakTorque = serializableData.breakTorque;
            native.EnableCollision = serializableData.enableCollision;

		    UpdateTransform(JointBody.A);
		    UpdateTransform(JointBody.B);
	    }

        /// <summary>
        /// Destroys the internal joint representation.
        /// </summary>
        private void DestroyNative()
	    {
	        if (native != null)
	        {
	            native.Destroy();
                native = null;
	        }
	    }

        /// <summary>
        /// Updates the local transform for the specified body attached to the joint.
        /// </summary>
        /// <param name="body">Body to update.</param>
	    private void UpdateTransform(JointBody body)
	    {
		    Vector3 localPos;
		    Quaternion localRot;

		    localPos = serializableData.positions[(int)body];
		    localRot = serializableData.rotations[(int)body];

		    // Transform to world space of the related body
		    Rigidbody rigidbody = serializableData.bodies[(int)body];
		    if (rigidbody != null)
		    {
			    localRot = rigidbody.SceneObject.Rotation * localRot;
			    localPos = localRot.Rotate(localPos) + rigidbody.SceneObject.Position;
		    }

		    // Transform to space local to the joint
		    Quaternion invRotation = SceneObject.Rotation.Inverse;

		    localPos = invRotation.Rotate(localPos - SceneObject.Position);
		    localRot = invRotation * localRot;

		    native.SetPosition(body, localPos);
            native.SetRotation(body, localRot);
	    }

        /// <summary>
        /// Holds all data the joint component needs to persist through serialization.
        /// </summary>
        [SerializeObject]
        internal class SerializableData
        {
            public Rigidbody[] bodies = new Rigidbody[2];
            public Vector3[] positions = new Vector3[2];
            public Quaternion[] rotations = new Quaternion[2];
            public float breakForce = float.MaxValue;
            public float breakTorque = float.MaxValue;
            public bool enableCollision = false;
        }
    }

    /// <summary>
    /// Controls spring parameters for a physics joint limits. If a limit is soft (body bounces back due to restition when 
    /// the limit is reached) the spring will pull the body back towards the limit using the specified parameters.
    /// </summary>
    [StructLayout(LayoutKind.Sequential), SerializeObject]
    public struct Spring // Note: Must match C++ struct Spring
    {
        /// <summary>
        /// Constructs a spring.
        /// </summary>
        /// <param name="stiffness">Spring strength.Force proportional to the position error.</param>
        /// <param name="damping">Damping strength. Force propertional to the velocity error.</param>
        public Spring(float stiffness, float damping)
        {
            this.stiffness = stiffness;
            this.damping = damping;
        }

        /// <summary>
        /// Spring strength. Force proportional to the position error.
        /// </summary>
        public float stiffness;

        /// <summary>
        /// Damping strength. Force propertional to the velocity error.
        /// </summary>
        public float damping;
	}

    /// <summary>
    /// Specifies first or second body referenced by a Joint.
    /// </summary>
    public enum JointBody
    {
        A, B
    };

    /// <summary>
    /// Specifies axes that the D6 joint can constrain motion on.
    /// </summary>
    public enum D6JointAxis
    {
        /// <summary>
        /// Movement on the X axis. 
        /// </summary>
        X,
        /// <summary>
        /// Movement on the Y axis.
        /// </summary>
		Y,
        /// <summary>
        /// Movement on the Z axis.
        /// </summary>
		Z,
        /// <summary>
        /// Rotation around the X axis.
        /// </summary>
		Twist,
        /// <summary>
        /// Rotation around the Y axis.
        /// </summary>
        SwingY,
        /// <summary>
        /// Rotation around the Z axis.
        /// </summary>
        SwingZ,
		Count
    }

    /// <summary>
    /// Specifies type of constraint placed on a specific axis of a D6 joint.
    /// </summary>
    public enum D6JointMotion
    {
        /// <summary>
        /// Axis is immovable.
        /// </summary>
        Locked,
        /// <summary>
        /// Axis will be constrained by the specified limits.
        /// </summary>
        Limited,
        /// <summary>
        /// Axis will not be constrained.
        /// </summary>
        Free,
		Count
    }

    /// <summary>
    /// Type of drives that can be used for moving or rotating bodies attached to the D6 joint.
    /// </summary>
    public enum DriveType
    {
        /// <summary>
        /// Linear movement on the X axis using the linear drive model.
        /// </summary>
        X,
        /// <summary>
        /// Linear movement on the Y axis using the linear drive model.
        /// </summary>
        Y,
        /// <summary>
        /// Linear movement on the Z axis using the linear drive model.
        /// </summary>
        Z,
        /// <summary>
        /// Rotation around the Y axis using the twist/swing angular drive model. Should not be used together with 
        /// SLERP mode. 
        /// </summary>
		Swing, 
        /// <summary>
        /// Rotation around the Z axis using the twist/swing angular drive model. Should not be used together with 
        /// SLERP mode.
        /// </summary>
		Twist,
        /// <summary>
        /// Rotation using spherical linear interpolation. Uses the SLERP angular drive mode which performs rotation
        /// by interpolating the quaternion values directly over the shortest path (applies to all three axes, which
        /// they all must be unlocked).
        /// </summary>
		SLERP, 
		Count
    }

    /// <summary>
    /// Specifies parameters for a drive that will attempt to move the D6 joint bodies to the specified drive position and
    /// velocity.
    /// </summary>
    public class D6JointDrive
    {
        /// <summary>
        /// Spring strength. Force proportional to the position error.
        /// </summary>
        public float stiffness = 0.0f;

        /// <summary>
        /// Damping strength. Force propertional to the velocity error.
        /// </summary>
        public float damping = 0.0f;

        /// <summary>
        /// Maximum force the drive can apply.
        /// </summary>
        public float forceLimit = float.MaxValue;

        /// <summary>
        /// If true the drive will generate acceleration instead of forces. Acceleration drives are easier to tune as
        /// they account for the masses of the actors to which the joint is attached.
        /// </summary>
        public bool acceleration = false;

        /// <summary>
        /// Used for accessing drive data from native code.
        /// </summary>
        /// <param name="output">Native readable drive structure.</param>
        private void Internal_GetNative(ref ScriptD6JointDrive output)
        {
            output.stiffness = stiffness;
            output.damping = damping;
            output.forceLimit = forceLimit;
            output.acceleration = acceleration;
        }
    }

    /// <summary>
    /// Properties of a drive that drives the hinge joint's angular velocity towards a paricular value.
    /// </summary>
    public class HingeJointDrive
    {
        /// <summary>
        /// Target speed of the joint.
        /// </summary>
        public float speed = 0.0f;

        /// <summary>
        /// Maximum torque the drive is allowed to apply.
        /// </summary>
        public float forceLimit = float.MaxValue;

        /// <summary>
        /// Scales the velocity of the first body, and its response to drive torque is scaled down.
        /// </summary>
        public float gearRatio = 1.0f;

        /// <summary>
        /// If the joint is moving faster than the drive's target speed, the drive will try to break. If you don't want
        /// the breaking to happen set this to true.
        /// </summary>
        public bool freeSpin = false;

        /// <summary>
        /// Used for accessing drive data from native code.
        /// </summary>
        /// <param name="output">Native readable drive structure.</param>
        private void Internal_GetNative(ref ScriptHingeJointDrive output)
        {
            output.speed = speed;
            output.forceLimit = forceLimit;
            output.gearRatio = gearRatio;
            output.freeSpin = freeSpin;
        }
    };

    /// <summary>
    /// Contains common values used by all Joint limit types.
    /// </summary>
    [SerializeObject]
    public class LimitCommon
	{
		public LimitCommon(float contactDist = -1.0f)
        {
            this.contactDist = contactDist;
            this.restitution = 0.0f;
            this.spring = new Spring();
        }

        public LimitCommon(Spring spring, float restitution = 0.0f)
        {
            this.contactDist = -1.0f;
            this.restitution = restitution;
            this.spring = spring;
        }

        /// <summary>
        /// Distance from the limit at which it becomes active. Allows the solver to activate earlier than the limit is
        /// reached to avoid breaking the limit.
        /// </summary>
        public float contactDist;

        /// <summary>
        /// Controls how do objects react when the limit is reached, values closer to zero specify non-ellastic collision,
        /// while those closer to one specify more ellastic(i.e bouncy) collision.Must be in [0, 1] range.
        /// </summary>
		public float restitution;

        /// <summary>
        /// Spring that controls how are the bodies pulled back towards the limit when they breach it.
        /// </summary>
        public Spring spring;
    }

    /// <summary>
    /// Represents a joint limit between two distance values. Lower value must be less than the upper value.
    /// </summary>
    [SerializeObject]
    public class LimitLinearRange : LimitCommon
	{
        /// <summary>
        /// Constructs an empty limit.
        /// </summary>
        public LimitLinearRange()
        { }

        /// <summary>
        /// Constructs a hard limit. Once the limit is reached the movement of the attached bodies will come to a stop.
        /// </summary>
        /// <param name="lower">Lower distance of the limit.Must be less than <paramref name="upper"/>.</param>
        /// <param name="upper">Upper distance of the limit.Must be more than <paramref name="lower"/>.</param>
        /// <param name="contactDist">Distance from the limit at which it becomes active.Allows the solver to activate 
        ///                           earlier than the limit is reached to avoid breaking the limit.Specify -1 for the 
        ///                           default.</param>
        public LimitLinearRange(float lower, float upper, float contactDist = -1.0f)
            :base(contactDist)
        {
            this.lower = lower;
            this.upper = upper;

        }

        /// <summary>
        /// Constructs a soft limit. Once the limit is reached the bodies will bounce back according to the resitution
        /// parameter and will be pulled back towards the limit by the provided spring.
        /// </summary>
        /// <param name="lower">Lower distance of the limit. Must be less than <paramref name="upper"/>.</param>
        /// <param name="upper">Upper distance of the limit. Must be more than <paramref name="lower"/>.</param>
        /// <param name="spring">Spring that controls how are the bodies pulled back towards the limit when they breach it.
        ///                      </param>
        /// <param name="restitution">Controls how do objects react when the limit is reached, values closer to zero specify
        ///                           non-ellastic collision, while those closer to one specify more ellastic(i.e bouncy)
        ///                           collision.Must be in [0, 1] range.</param>
        public LimitLinearRange(float lower, float upper, Spring spring, float restitution = 0.0f)
            :base(spring, restitution)
        {
            this.lower = lower;
            this.upper = upper;
        }

		/// <summary>
        /// Lower distance of the limit. Must be less than #upper.
        /// </summary>
		public float lower;

        /// <summary>
        /// Upper distance of the limit. Must be more than #lower.
        /// </summary>
        public float upper;

        /// <summary>
        /// Used for accessing limit data from native code.
        /// </summary>
        /// <param name="output">Native readable limit structure.</param>
        private void Internal_GetNative(ref ScriptLimitLinearRange output)
        {
            output.contactDist = contactDist;
            output.resitution = restitution;
            output.spring = spring;
            output.lower = lower;
            output.upper = upper;
        }
    }

    /// <summary>
    /// Represents a joint limit between zero a single distance value.
    /// </summary>
    [SerializeObject]
    public class LimitLinear : LimitCommon
	{
        /// <summary>
        /// Constructs an empty limit.
        /// </summary>
        public LimitLinear()
		{ }

        /// <summary>
        /// Constructs a hard limit.Once the limit is reached the movement of the attached bodies will come to a stop.
        /// </summary>
        /// <param name="extent">Distance at which the limit becomes active.</param>
        /// <param name="contactDist">Distance from the limit at which it becomes active. Allows the solver to activate 
        ///                           earlier than the limit is reached to avoid breaking the limit.Specify -1 for the 
        ///                           default.</param>
        public LimitLinear(float extent, float contactDist = -1.0f)
			:base(contactDist)
        {
            this.extent = extent;
        }

        /// <summary>
        /// Constructs a soft limit.Once the limit is reached the bodies will bounce back according to the resitution
        /// parameter and will be pulled back towards the limit by the provided spring.
        /// </summary>
        /// <param name="extent">Distance at which the limit becomes active. </param>
        /// <param name="spring">Spring that controls how are the bodies pulled back towards the limit when they breach it.
        ///                      </param>
        /// <param name="restitution">Controls how do objects react when the limit is reached, values closer to zero specify
        ///                           non-ellastic collision, while those closer to one specify more ellastic(i.e bouncy)
        ///                           collision.Must be in [0, 1] range.</param>
		public LimitLinear(float extent, Spring spring, float restitution = 0.0f)
			:base(spring, restitution)
        {
            this.extent = extent;
        }

        /// <summary>
        /// Distance at which the limit becomes active.
        /// </summary>
        public float extent = 0.0f;

        /// <summary>
        /// Used for accessing limit data from native code.
        /// </summary>
        /// <param name="output">Native readable limit structure.</param>
        private void Internal_GetNative(ref ScriptLimitLinear output)
        {
            output.contactDist = contactDist;
            output.resitution = restitution;
            output.spring = spring;
            output.extent = extent;
        }
    }

    /// <summary>
    /// Represents a joint limit between two angles.
    /// </summary>
    [SerializeObject]
    public class LimitAngularRange : LimitCommon
	{
        /// <summary>
        /// Constructs an empty limit.
        /// </summary>
        public LimitAngularRange()
		{ }

        /// <summary>
        /// Constructs a hard limit. Once the limit is reached the movement of the attached bodies will come to a stop.
        /// </summary>
        /// <param name="lower">Lower angle of the limit. Must be less than <paramref name="upper"/>.</param>
        /// <param name="upper">Upper angle of the limit. Must be more than <paramref name="lower"/>.</param>
        /// <param name="contactDist">Distance from the limit at which it becomes active. Allows the solver to activate 
        ///                           earlier than the limit is reached to avoid breaking the limit.Specify -1 for the 
        ///                           default.</param>
        public LimitAngularRange(Radian lower, Radian upper, float contactDist = -1.0f)
            : base(contactDist)
        {
            this.lower = lower;
            this.upper = upper;
        }

        /// <summary>
        /// Constructs a soft limit. Once the limit is reached the bodies will bounce back according to the resitution
        /// parameter and will be pulled back towards the limit by the provided spring.
        /// </summary>
        /// <param name="lower">Lower angle of the limit. Must be less than <paramref name="upper"/>.</param>
        /// <param name="upper">Upper angle of the limit. Must be more than <paramref name="lower"/>.</param>
        /// <param name="spring">Spring that controls how are the bodies pulled back towards the limit when they breach it.
        ///                      </param>
        /// <param name="restitution">Controls how do objects react when the limit is reached, values closer to zero specify
        ///                           non-ellastic collision, while those closer to one specify more ellastic(i.e bouncy)
        ///                           collision.Must be in [0, 1] range.</param>
        public LimitAngularRange(Radian lower, Radian upper, Spring spring, float restitution = 0.0f)
            : base(spring, restitution)
        {
            this.lower = lower;
            this.upper = upper;
        }

        /// <summary>
        /// Lower angle of the limit. Must be less than #upper.
        /// </summary>
        public Radian lower = new Radian(0.0f);

        /// <summary>
        /// Upper angle of the limit. Must be less than #lower.
        /// </summary>
        public Radian upper = new Radian(0.0f);

        /// <summary>
        /// Used for accessing limit data from native code.
        /// </summary>
        /// <param name="output">Native readable limit structure.</param>
        private void Internal_GetNative(ref ScriptLimitAngularRange output)
        {
            output.contactDist = contactDist;
            output.resitution = restitution;
            output.spring = spring;
            output.lower = lower;
            output.upper = upper;
        }
    }

    /// <summary>
    /// Represents a joint limit that contraints movement to within an elliptical cone.
    /// </summary>
    [SerializeObject]
    public class LimitConeRange : LimitCommon
	{
        /// <summary>
        /// Constructs a limit with a 45 degree cone.
        /// </summary>
        public LimitConeRange()
		{ }

        /// <summary>
        /// Constructs a hard limit. Once the limit is reached the movement of the attached bodies will come to a stop.
        /// </summary>
        /// <param name="yLimitAngle">Y angle of the cone. Movement is constrainted between 0 and this angle on the Y axis.
        ///                           </param>
        /// <param name="zLimitAngle">Z angle of the cone. Movement is constrainted between 0 and this angle on the Z axis.
        ///                           </param>
        /// <param name="contactDist">Distance from the limit at which it becomes active. Allows the solver to activate 
        ///                           earlier than the limit is reached to avoid breaking the limit.Specify -1 for the
        ///                           default.</param>
        public LimitConeRange(Radian yLimitAngle, Radian zLimitAngle, float contactDist = -1.0f)
            : base(contactDist)
        {
            this.yLimitAngle = yLimitAngle;
            this.zLimitAngle = zLimitAngle;
        }

        /// <summary>
        /// Constructs a soft limit. Once the limit is reached the bodies will bounce back according to the resitution
        /// parameter and will be pulled back towards the limit by the provided spring.
        /// </summary>
        /// <param name="yLimitAngle">Y angle of the cone. Movement is constrainted between 0 and this angle on the Y axis.
        ///                           </param>
        /// <param name="zLimitAngle">Z angle of the cone. Movement is constrainted between 0 and this angle on the Z axis.
        ///                           </param>
        /// <param name="spring">Spring that controls how are the bodies pulled back towards the limit when they breach it.
        ///                      </param>
        /// <param name="restitution">Controls how do objects react when the limit is reached, values closer to zero specify
        ///                           non-ellastic collision, while those closer to one specify more ellastic(i.e bouncy)
        ///                           collision.Must be in [0, 1] range.</param>
        public LimitConeRange(Radian yLimitAngle, Radian zLimitAngle, Spring spring, float restitution = 0.0f)
            : base(spring, restitution)
        {
            this.yLimitAngle = yLimitAngle;
            this.zLimitAngle = zLimitAngle;
        }

        /// <summary>
        /// Y angle of the cone. Movement is constrainted between 0 and this angle on the Y axis.
        /// </summary>
        public Radian yLimitAngle = new Radian(MathEx.Pi * 0.5f);

        /// <summary>
        /// Z angle of the cone. Movement is constrainted between 0 and this angle on the Z axis.
        /// </summary>
        public Radian zLimitAngle = new Radian(MathEx.Pi * 0.5f);

        /// <summary>
        /// Used for accessing limit data from native code.
        /// </summary>
        /// <param name="output">Native readable limit structure.</param>
        private void Internal_GetNative(ref ScriptLimitConeRange output)
        {
            output.contactDist = contactDist;
            output.resitution = restitution;
            output.spring = spring;
            output.yLimitAngle = yLimitAngle;
            output.zLimitAngle = zLimitAngle;
        }
	}

    /// <summary>
    /// Used for passing HingeJointDrive data between native and managed code.
    /// </summary>
    [StructLayout(LayoutKind.Sequential)]
    internal struct ScriptHingeJointDrive // Note: Must match C++ struct HingeJoint::Drive
    {
        public float speed;
        public float forceLimit;
        public float gearRatio;
        public bool freeSpin;
    }

    /// <summary>
    /// Used for passing D6JointDrive data between native and managed code.
    /// </summary>
    [StructLayout(LayoutKind.Sequential)]
    internal struct ScriptD6JointDrive // Note: Must match C++ struct D6Joint::Drive
    {
        public float stiffness;
        public float damping;
        public float forceLimit;
        public bool acceleration;
    }

    /// <summary>
    /// Used for passing LimitLinearRange data between native and managed code.
    /// </summary>
    [StructLayout(LayoutKind.Sequential)]
    internal struct ScriptLimitLinearRange // Note: Must match C++ struct LimitLinearRange
    {
        public float contactDist;
        public float resitution;
        public Spring spring;
        public float lower;
        public float upper;
    }

    /// <summary>
    /// Used for passing LimitLinear data between native and managed code.
    /// </summary>
    [StructLayout(LayoutKind.Sequential)]
    internal struct ScriptLimitLinear // Note: Must match C++ struct LimitLinear
    {
        public float contactDist;
        public float resitution;
        public Spring spring;
        public float extent;
    }

    /// <summary>
    /// Used for passing LimitAngularRange data between native and managed code.
    /// </summary>
    [StructLayout(LayoutKind.Sequential)]
    internal struct ScriptLimitAngularRange // Note: Must match C++ struct LimitAngularRange
    {
        public float contactDist;
        public float resitution;
        public Spring spring;
        public Radian lower;
        public Radian upper;
    }

    /// <summary>
    /// Used for passing LimitConeRange data between native and managed code.
    /// </summary>
    [StructLayout(LayoutKind.Sequential)]
    internal struct ScriptLimitConeRange // Note: Must match C++ struct LimitConeRange
    {
        public float contactDist;
        public float resitution;
        public Spring spring;
        public Radian yLimitAngle;
        public Radian zLimitAngle;
    }
}