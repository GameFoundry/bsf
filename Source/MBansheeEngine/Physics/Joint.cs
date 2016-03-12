//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//

using System;
using System.Runtime.InteropServices;

namespace BansheeEngine
{
    /// <summary>
    /// Base class for all Joint types. Joints constrain how two rigidbodies move relative to one another (for example a
    /// door hinge). One of the bodies in the joint must always be movable (that is non-kinematic).
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

        private void OnEnable()
        {
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
    /// Controls spring parameters for a physics joint limits. If a limit is soft (body bounces back due to restitution when 
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

        /// <inheritdoc/>
        public override bool Equals(object rhs)
        {
            if (rhs is Spring)
            {
                Spring other = (Spring)rhs;
                return stiffness == other.stiffness && damping == other.damping;
            }

            return false;
        }

        /// <inheritdoc/>
        public override int GetHashCode()
        {
            return base.GetHashCode();
        }

        public static bool operator ==(Spring a, Spring b)
        {
            return a.Equals(b);
        }

        public static bool operator !=(Spring a, Spring b)
        {
            return !(a == b);
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
    public enum D6JointDriveType
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
    [SerializeObject]
    public class D6JointDrive
    {
        [SerializeField]
        private D6JointDriveData data;

        /// <summary>
        /// Spring strength. Force proportional to the position error.
        /// </summary>
        public float Stiffness { get { return data.stiffness; } }

        /// <summary>
        /// Damping strength. Force propertional to the velocity error.
        /// </summary>
        public float Damping { get { return data.damping; } }

        /// <summary>
        /// Maximum force the drive can apply.
        /// </summary>
        public float ForceLimit { get { return data.forceLimit; } }

        /// <summary>
        /// If true the drive will generate acceleration instead of forces. Acceleration drives are easier to tune as
        /// they account for the masses of the actors to which the joint is attached.
        /// </summary>
        public bool Acceleration { get { return data.acceleration; } }

        /// <summary>
        /// Gets drive properties.
        /// </summary>
        public D6JointDriveData Data
        {
            get { return data; }
        }

        /// <summary>
        /// Constructs a new D6 joint drive.
        /// </summary>
        /// <param name="stiffness"><see cref="Stiffness"/></param>
        /// <param name="damping"><see cref="Damping"/></param>
        /// <param name="forceLimit"><see cref="ForceLimit"/></param>
        /// <param name="acceleration"><see cref="Acceleration"/></param>
        public D6JointDrive(float stiffness = 0.0f, float damping = 0.0f, float forceLimit = float.MaxValue,
            bool acceleration = false)
        {
            data.stiffness = stiffness;
            data.damping = damping;
            data.forceLimit = forceLimit;
            data.acceleration = acceleration;
        }

        /// <summary>
        /// Constructs a new D6 joint drive.
        /// </summary>
        /// <param name="data">Properties to initialize the drive with.</param>
        public D6JointDrive(D6JointDriveData data)
        {
            this.data = data;
        }

        /// <inheritdoc/>
        public override bool Equals(object rhs)
        {
            if (rhs is D6JointDrive)
            {
                D6JointDrive other = (D6JointDrive)rhs;
                return Stiffness == other.Stiffness && Damping == other.Damping && ForceLimit == other.ForceLimit 
                    && Acceleration == other.Acceleration;
            }

            return false;
        }

        /// <inheritdoc/>
        public override int GetHashCode()
        {
            return base.GetHashCode();
        }

        public static bool operator ==(D6JointDrive a, D6JointDrive b)
        {
            return a.Equals(b);
        }

        public static bool operator !=(D6JointDrive a, D6JointDrive b)
        {
            return !(a == b);
        }

        /// <summary>
        /// Used for accessing drive data from native code.
        /// </summary>
        /// <param name="output">Native readable drive structure.</param>
        private void Internal_GetNative(out D6JointDriveData output)
        {
            output = data;
        }
    }

    /// <summary>
    /// Properties of a drive that drives the hinge joint's angular velocity towards a paricular value.
    /// </summary>
    [SerializeObject]
    public class HingeJointDrive
    {
        [SerializeField]
        private HingeJointDriveData data;

        /// <summary>
        /// Target speed of the joint.
        /// </summary>
        public float Speed { get { return data.speed; } }

        /// <summary>
        /// Maximum torque the drive is allowed to apply.
        /// </summary>
        public float ForceLimit { get { return data.forceLimit; } }

        /// <summary>
        /// Scales the velocity of the first body, and its response to drive torque is scaled down.
        /// </summary>
        public float GearRatio { get { return data.gearRatio; } }

        /// <summary>
        /// If the joint is moving faster than the drive's target speed, the drive will try to break. If you don't want
        /// the breaking to happen set this to true.
        /// </summary>
        public bool FreeSpin { get { return data.freeSpin; } }

        /// <summary>
        /// Gets drive properties.
        /// </summary>
        public HingeJointDriveData Data
        {
            get { return data; }
        }

        /// <summary>
        /// Constructs a new hinge joint drive.
        /// </summary>
        /// <param name="speed"><see cref="Speed"/></param>
        /// <param name="forceLimit"><see cref="ForceLimit"/></param>
        /// <param name="gearRatio"><see cref="GearRatio"/></param>
        /// <param name="freeSpin"><see cref="FreeSpin"/></param>
        public HingeJointDrive(float speed = 0.0f, float forceLimit = float.MaxValue, 
            float gearRatio = 1.0f, bool freeSpin = false)
        {
            data.speed = speed;
            data.forceLimit = forceLimit;
            data.gearRatio = gearRatio;
            data.freeSpin = freeSpin;
        }

        /// <summary>
        /// Constructs a new hinge joint drive.
        /// </summary>
        /// <param name="data">Properties to initialize the drive with.</param>
        public HingeJointDrive(HingeJointDriveData data)
        {
            this.data = data;
        }

        /// <inheritdoc/>
        public override bool Equals(object rhs)
        {
            if (rhs is HingeJointDrive)
            {
                HingeJointDrive other = (HingeJointDrive)rhs;
                return data.speed == other.data.speed && data.gearRatio == other.data.gearRatio && 
                    data.forceLimit == other.data.forceLimit && data.freeSpin == other.data.freeSpin;
            }

            return false;
        }

        /// <inheritdoc/>
        public override int GetHashCode()
        {
            return base.GetHashCode();
        }

        public static bool operator ==(HingeJointDrive a, HingeJointDrive b)
        {
            return a.Equals(b);
        }

        public static bool operator !=(HingeJointDrive a, HingeJointDrive b)
        {
            return !(a == b);
        }

        /// <summary>
        /// Used for accessing drive data from native code.
        /// </summary>
        /// <param name="output">Native readable drive structure.</param>
        private void Internal_GetNative(out HingeJointDriveData output)
        {
            output = data;
        }
    };

    /// <summary>
    /// Contains common values used by all Joint limit types.
    /// </summary>
    [SerializeObject]
    public class LimitCommon
    {
        private LimitCommonData data;

        /// <summary>
        /// Distance from the limit at which it becomes active. Allows the solver to activate earlier than the limit is
        /// reached to avoid breaking the limit.
        /// </summary>
        public float ContactDist { get { return data.contactDist; } }

        /// <summary>
        /// Controls how do objects react when the limit is reached, values closer to zero specify non-ellastic collision,
        /// while those closer to one specify more ellastic(i.e bouncy) collision.Must be in [0, 1] range.
        /// </summary>
		public float Restitution { get { return data.restitution; } }

        /// <summary>
        /// Spring that controls how are the bodies pulled back towards the limit when they breach it.
        /// </summary>
        public Spring Spring { get { return data.spring; } }

        /// <summary>
        /// Gets properties common to all limit types.
        /// </summary>
        public LimitCommonData CommonData
        {
            get { return data; }
        }

        protected LimitCommon(float contactDist = -1.0f)
        {
            data.contactDist = contactDist;
            data.restitution = 0.0f;
            data.spring = new Spring();
        }

        protected LimitCommon(Spring spring, float restitution = 0.0f)
        {
            data.contactDist = -1.0f;
            data.restitution = restitution;
            data.spring = spring;
        }

        protected LimitCommon(LimitCommonData data)
        {
            this.data = data;
        }

        /// <inheritdoc/>
        public override bool Equals(object rhs)
        {
            if (rhs is LimitCommon)
            {
                LimitCommon other = (LimitCommon)rhs;
                return ContactDist == other.ContactDist && Restitution == other.Restitution && Spring == other.Spring;
            }

            return false;
        }

        /// <inheritdoc/>
        public override int GetHashCode()
        {
            return base.GetHashCode();
        }

        public static bool operator ==(LimitCommon a, LimitCommon b)
        {
            return a.Equals(b);
        }

        public static bool operator !=(LimitCommon a, LimitCommon b)
        {
            return !(a == b);
        }
    }

    /// <summary>
    /// Represents a joint limit between two distance values. Lower value must be less than the upper value.
    /// </summary>
    [SerializeObject]
    public class LimitLinearRange : LimitCommon
    {
        private LimitLinearRangeData data;

        /// <summary>
        /// Lower distance of the limit. Must be less than <see cref="Upper"/>.
        /// </summary>
        public float Lower { get { return data.lower; } }

        /// <summary>
        /// Upper distance of the limit. Must be greater than <see cref="Lower"/>.
        /// </summary>
        public float Upper { get { return data.upper; } }

        /// <summary>
        /// Gets properties of the linear limit range.
        /// </summary>
        public LimitLinearRangeData Data
        {
            get { return data; }
        }

        /// <summary>
        /// Constructs an empty limit.
        /// </summary>
        public LimitLinearRange()
        { }

        /// <summary>
        /// Constructs a hard limit. Once the limit is reached the movement of the attached bodies will come to a stop.
        /// </summary>
        /// <param name="lower"><see cref="Lower"/></param>
        /// <param name="upper"><see cref="Upper"/></param>
        /// <param name="contactDist"><see cref="LimitCommon.ContactDist"/></param>
        public LimitLinearRange(float lower, float upper, float contactDist = -1.0f)
            :base(contactDist)
        {
            data.lower = lower;
            data.upper = upper;
        }

        /// <summary>
        /// Constructs a soft limit. Once the limit is reached the bodies will bounce back according to the resitution
        /// parameter and will be pulled back towards the limit by the provided spring.
        /// </summary>
        /// <param name="lower"><see cref="Lower"/></param>
        /// <param name="upper"><see cref="Upper"/></param>
        /// <param name="spring"><see cref="LimitCommon.Spring"/></param>
        /// <param name="restitution"><see cref="LimitCommon.Restitution"/></param>
        public LimitLinearRange(float lower, float upper, Spring spring, float restitution = 0.0f)
            :base(spring, restitution)
        {
            data.lower = lower;
            data.upper = upper;
        }

        /// <summary>
        /// Constructs a new limit from the provided properties.
        /// </summary>
        /// <param name="limitData">Linear range specific properties.</param>
        /// <param name="commonData">Properties common to all limit types.</param>
        public LimitLinearRange(LimitLinearRangeData limitData, LimitCommonData commonData)
            :base(commonData)
        {
            this.data = limitData;
        }

        /// <inheritdoc/>
        public override bool Equals(object rhs)
        {
            if (rhs is LimitLinearRange)
            {
                LimitLinearRange other = (LimitLinearRange)rhs;
                return base.Equals(rhs) && Lower == other.Lower && Upper == other.Upper;
            }

            return false;
        }

        /// <inheritdoc/>
        public override int GetHashCode()
        {
            return base.GetHashCode();
        }

        public static bool operator ==(LimitLinearRange a, LimitLinearRange b)
        {
            return a.Equals(b);
        }

        public static bool operator !=(LimitLinearRange a, LimitLinearRange b)
        {
            return !(a == b);
        }

        /// <summary>
        /// Used for accessing limit data from native code.
        /// </summary>
        /// <param name="output">Native readable limit structure.</param>
        private void Internal_GetNative(ref ScriptLimitLinearRange output)
        {
            output.contactDist = ContactDist;
            output.restitution = Restitution;
            output.spring = Spring;
            output.lower = Lower;
            output.upper = Upper;
        }
    }

    /// <summary>
    /// Represents a joint limit between zero a single distance value.
    /// </summary>
    [SerializeObject]
    public class LimitLinear : LimitCommon
    {
        private LimitLinearData data;

        /// <summary>
        /// Distance at which the limit becomes active.
        /// </summary>
        public float Extent { get { return data.extent; } }

        /// <summary>
        /// Gets properties of the linear limit.
        /// </summary>
        public LimitLinearData Data
        {
            get { return data; }
        }

        /// <summary>
        /// Constructs an empty limit.
        /// </summary>
        public LimitLinear()
		{ }

        /// <summary>
        /// Constructs a hard limit.Once the limit is reached the movement of the attached bodies will come to a stop.
        /// </summary>
        /// <param name="extent"><see cref="Extent"/></param>
        /// <param name="contactDist"><see cref="LimitCommon.ContactDist"/></param>
        public LimitLinear(float extent, float contactDist = -1.0f)
			:base(contactDist)
        {
            data.extent = extent;
        }

        /// <summary>
        /// Constructs a soft limit.Once the limit is reached the bodies will bounce back according to the resitution
        /// parameter and will be pulled back towards the limit by the provided spring.
        /// </summary>
        /// <param name="extent"><see cref="Extent"/></param>
        /// <param name="spring"><see cref="LimitCommon.Spring"/></param>
        /// <param name="restitution"><see cref="LimitCommon.Restitution"/></param>
		public LimitLinear(float extent, Spring spring, float restitution = 0.0f)
			:base(spring, restitution)
        {
            data.extent = extent;
        }

        /// <summary>
        /// Constructs a new limit from the provided properties.
        /// </summary>
        /// <param name="limitData">Linear limit specific properties.</param>
        /// <param name="commonData">Properties common to all limit types.</param>
        public LimitLinear(LimitLinearData limitData, LimitCommonData commonData)
            :base(commonData)
        {
            this.data = limitData;
        }

        /// <inheritdoc/>
        public override bool Equals(object rhs)
        {
            if (rhs is LimitLinear)
            {
                LimitLinear other = (LimitLinear)rhs;
                return base.Equals(rhs) && Extent == other.Extent;
            }

            return false;
        }

        /// <inheritdoc/>
        public override int GetHashCode()
        {
            return base.GetHashCode();
        }

        public static bool operator ==(LimitLinear a, LimitLinear b)
        {
            return a.Equals(b);
        }

        public static bool operator !=(LimitLinear a, LimitLinear b)
        {
            return !(a == b);
        }

        /// <summary>
        /// Used for accessing limit data from native code.
        /// </summary>
        /// <param name="output">Native readable limit structure.</param>
        private void Internal_GetNative(ref ScriptLimitLinear output)
        {
            output.contactDist = ContactDist;
            output.restitution = Restitution;
            output.spring = Spring;
            output.extent = Extent;
        }
    }

    /// <summary>
    /// Represents a joint limit between two angles.
    /// </summary>
    [SerializeObject]
    public class LimitAngularRange : LimitCommon
	{
        private LimitAngularRangeData data;

        /// <summary>
        /// Lower angle of the limit. Must be less than <see cref="Upper"/>.
        /// </summary>
        public Radian Lower { get { return data.lower; } }

        /// <summary>
        /// Upper angle of the limit. Must be greater than <see cref="Lower"/>.
        /// </summary>
        public Radian Upper { get { return data.upper; } }

        /// <summary>
        /// Gets properties of the angular limit range.
        /// </summary>
        public LimitAngularRangeData Data
        {
            get { return data; }
        }

        /// <summary>
        /// Constructs an empty limit.
        /// </summary>
        public LimitAngularRange()
		{ }

        /// <summary>
        /// Constructs a hard limit. Once the limit is reached the movement of the attached bodies will come to a stop.
        /// </summary>
        /// <param name="lower"><see cref="Lower"/></param>
        /// <param name="upper"><see cref="Upper"/></param>
        /// <param name="contactDist"><see cref="LimitCommon.ContactDist"/></param>
        public LimitAngularRange(Radian lower, Radian upper, float contactDist = -1.0f)
            : base(contactDist)
        {
            data.lower = lower;
            data.upper = upper;
        }

        /// <summary>
        /// Constructs a soft limit. Once the limit is reached the bodies will bounce back according to the resitution
        /// parameter and will be pulled back towards the limit by the provided spring.
        /// </summary>
        /// <param name="lower"><see cref="Lower"/></param>
        /// <param name="upper"><see cref="Upper"/></param>
        /// <param name="spring"><see cref="LimitCommon.Spring"/></param>
        /// <param name="restitution"><see cref="LimitCommon.Restitution"/></param>
        public LimitAngularRange(Radian lower, Radian upper, Spring spring, float restitution = 0.0f)
            : base(spring, restitution)
        {
            data.lower = lower;
            data.upper = upper;
        }

        /// <summary>
        /// Constructs a new limit from the provided properties.
        /// </summary>
        /// <param name="limitData">Angular limit range specific properties.</param>
        /// <param name="commonData">Properties common to all limit types.</param>
        public LimitAngularRange(LimitAngularRangeData limitData, LimitCommonData commonData)
            :base(commonData)
        {
            this.data = limitData;
        }

        /// <inheritdoc/>
        public override bool Equals(object rhs)
        {
            if (rhs is LimitAngularRange)
            {
                LimitAngularRange other = (LimitAngularRange)rhs;
                return base.Equals(rhs) && Lower == other.Lower && Upper == other.Upper;
            }

            return false;
        }

        /// <inheritdoc/>
        public override int GetHashCode()
        {
            return base.GetHashCode();
        }

        public static bool operator ==(LimitAngularRange a, LimitAngularRange b)
        {
            return a.Equals(b);
        }

        public static bool operator !=(LimitAngularRange a, LimitAngularRange b)
        {
            return !(a == b);
        }

        /// <summary>
        /// Used for accessing limit data from native code.
        /// </summary>
        /// <param name="output">Native readable limit structure.</param>
        private void Internal_GetNative(ref ScriptLimitAngularRange output)
        {
            output.contactDist = ContactDist;
            output.restitution = Restitution;
            output.spring = Spring;
            output.lower = Lower;
            output.upper = Upper;
        }
    }

    /// <summary>
    /// Represents a joint limit that contraints movement to within an elliptical cone.
    /// </summary>
    [SerializeObject]
    public class LimitConeRange : LimitCommon
    {
        private LimitConeRangeData data;

        /// <summary>
        /// Y angle of the cone. Movement is constrainted between 0 and this angle on the Y axis.
        /// </summary>
        public Radian YLimitAngle { get { return data.yLimitAngle; } }

        /// <summary>
        /// Z angle of the cone. Movement is constrainted between 0 and this angle on the Z axis.
        /// </summary>
        public Radian ZLimitAngle { get { return data.zLimitAngle; } }

        /// <summary>
        /// Gets properties of the cone limit range.
        /// </summary>
        public LimitConeRangeData Data
        {
            get { return data; }
        }

        /// <summary>
        /// Constructs a limit with a 45 degree cone.
        /// </summary>
        public LimitConeRange()
        {
            data.yLimitAngle = new Radian(MathEx.Pi * 0.5f);
            data.zLimitAngle = new Radian(MathEx.Pi * 0.5f);
        }

        /// <summary>
        /// Constructs a hard limit. Once the limit is reached the movement of the attached bodies will come to a stop.
        /// </summary>
        /// <param name="yLimitAngle"><see cref="YLimitAngle"/></param>
        /// <param name="zLimitAngle"><see cref="ZLimitAngle"/></param>
        /// <param name="contactDist"><see cref="LimitCommon.ContactDist"/></param>
        public LimitConeRange(Radian yLimitAngle, Radian zLimitAngle, float contactDist = -1.0f)
            : base(contactDist)
        {
            data.yLimitAngle = yLimitAngle;
            data.zLimitAngle = zLimitAngle;
        }

        /// <summary>
        /// Constructs a soft limit. Once the limit is reached the bodies will bounce back according to the resitution
        /// parameter and will be pulled back towards the limit by the provided spring.
        /// </summary>
        /// <param name="yLimitAngle"><see cref="YLimitAngle"/></param>
        /// <param name="zLimitAngle"><see cref="ZLimitAngle"/></param>
        /// <param name="spring"><see cref="LimitCommon.Spring"/></param>
        /// <param name="restitution"><see cref="LimitCommon.Restitution"/></param>
        public LimitConeRange(Radian yLimitAngle, Radian zLimitAngle, Spring spring, float restitution = 0.0f)
            : base(spring, restitution)
        {
            data.yLimitAngle = yLimitAngle;
            data.zLimitAngle = zLimitAngle;
        }

        /// <summary>
        /// Constructs a new limit from the provided properties.
        /// </summary>
        /// <param name="limitData">Cone limit range specific properties.</param>
        /// <param name="commonData">Properties common to all limit types.</param>
        public LimitConeRange(LimitConeRangeData limitData, LimitCommonData commonData)
            :base(commonData)
        {
            this.data = limitData;
        }

        /// <inheritdoc/>
        public override bool Equals(object rhs)
        {
            if (rhs is LimitConeRange)
            {
                LimitConeRange other = (LimitConeRange)rhs;
                return base.Equals(rhs) && YLimitAngle == other.YLimitAngle && ZLimitAngle == other.ZLimitAngle;
            }

            return false;
        }

        /// <inheritdoc/>
        public override int GetHashCode()
        {
            return base.GetHashCode();
        }

        public static bool operator ==(LimitConeRange a, LimitConeRange b)
        {
            return a.Equals(b);
        }

        public static bool operator !=(LimitConeRange a, LimitConeRange b)
        {
            return !(a == b);
        }

        /// <summary>
        /// Used for accessing limit data from native code.
        /// </summary>
        /// <param name="output">Native readable limit structure.</param>
        private void Internal_GetNative(ref ScriptLimitConeRange output)
        {
            output.contactDist = ContactDist;
            output.restitution = Restitution;
            output.spring = Spring;
            output.yLimitAngle = YLimitAngle;
            output.zLimitAngle = ZLimitAngle;
        }
	}

    /// <summary>
    /// Contains data used by HingeJointDrive.
    /// </summary>
    [StructLayout(LayoutKind.Sequential)]
    public struct HingeJointDriveData // Note: Must match C++ struct HingeJoint::Drive
    {
        /// <summary>
        /// <see cref="HingeJointDrive.Speed"/>
        /// </summary>
        public float speed;

        /// <summary>
        /// <see cref="HingeJointDrive.ForceLimit"/>
        /// </summary>
        public float forceLimit;

        /// <summary>
        /// <see cref="HingeJointDrive.GearRatio"/>
        /// </summary>
        public float gearRatio;

        /// <summary>
        /// <see cref="HingeJointDrive.FreeSpin"/>
        /// </summary>
        public bool freeSpin;
    }

    /// <summary>
    /// Contains data used by D6JointDrive.
    /// </summary>
    [StructLayout(LayoutKind.Sequential)]
    public struct D6JointDriveData // Note: Must match C++ struct D6Joint::Drive
    {
        /// <summary>
        /// <see cref="D6JointDrive.Stiffness"/>
        /// </summary>
        public float stiffness;

        /// <summary>
        /// <see cref="D6JointDrive.Damping"/>
        /// </summary>
        public float damping;

        /// <summary>
        /// <see cref="D6JointDrive.ForceLimit"/>
        /// </summary>
        public float forceLimit;

        /// <summary>
        /// <see cref="D6JointDrive.Acceleration"/>
        /// </summary>
        public bool acceleration;
    }

    /// <summary>
    /// Contains data used by LimitCommon.
    /// </summary>
    public struct LimitCommonData
    {
        /// <summary>
        /// <see cref="LimitCommon.ContactDist"/>
        /// </summary>
        public float contactDist;

        /// <summary>
        /// <see cref="LimitCommon.Restitution"/>
        /// </summary>
        public float restitution;

        /// <summary>
        /// <see cref="LimitCommon.Spring"/>
        /// </summary>
        public Spring spring;
    }

    /// <summary>
    /// Contains data used by LimitLinearRange.
    /// </summary>
    public struct LimitLinearRangeData
    {
        /// <summary>
        /// <see cref="LimitLinearRange.Lower"/>
        /// </summary>
        public float lower;
        /// <summary>
        /// <see cref="LimitLinearRange.Upper"/>
        /// </summary>
        public float upper;
    }

    /// <summary>
    /// Contains data used by LimitLinear.
    /// </summary>
    public struct LimitLinearData
    {
        /// <summary>
        /// <see cref="LimitLinearRange.Extent"/>
        /// </summary>
        public float extent;
    }

    /// <summary>
    /// Contains data used by LimitAngularRange.
    /// </summary>
    public struct LimitAngularRangeData
    {
        /// <summary>
        /// <see cref="LimitAngularRange.Lower"/>
        /// </summary>
        public Radian lower;
        /// <summary>
        /// <see cref="LimitAngularRange.Upper"/>
        /// </summary>
        public Radian upper;
    }

    /// <summary>
    /// Contains data used by LimitConeRange.
    /// </summary>
    public struct LimitConeRangeData
    {
        /// <summary>
        /// <see cref="LimitConeRange.YLimitAngle"/>
        /// </summary>
        public Radian yLimitAngle;
        /// <summary>
        /// <see cref="LimitConeRange.ZLimitAngle"/>
        /// </summary>
        public Radian zLimitAngle;
    }

    /// <summary>
    /// Used for passing LimitLinearRange data between native and managed code.
    /// </summary>
    [StructLayout(LayoutKind.Sequential)]
    internal struct ScriptLimitLinearRange // Note: Must match C++ struct LimitLinearRange
    {
        public float contactDist;
        public float restitution;
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
        public float restitution;
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
        public float restitution;
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
        public float restitution;
        public Spring spring;
        public Radian yLimitAngle;
        public Radian zLimitAngle;
    }
}