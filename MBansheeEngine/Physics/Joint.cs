//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
using System.Runtime.InteropServices;

namespace BansheeEngine
{
    public class Joint : Component
    {
        // TODO
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