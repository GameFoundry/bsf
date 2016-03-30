//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
namespace BansheeEngine
{
    /// <summary>
    /// Represents the most customizable type of joint. This joint type can be used to create all other built-in joint 
    /// types, and to design your own custom ones, but is less intuitive to use.Allows a specification of a linear
    /// constraint (for example for slider), twist constraint(rotating around X) and swing constraint(rotating around Y and
    /// Z). It also allows you to constrain limits to only specific axes or completely lock specific axes.
    /// </summary>
    public sealed class D6Joint : Joint
    {
        [SerializeField]
        private SerializableData data = new SerializableData();

        /// <summary>
        /// Returns the current rotation of the joint around the X axis.
        /// </summary>
        public Radian Twist
        {
            get
            {
                if (Native != null)
                    return Native.Twist;

                return new Radian(0.0f);
            }
        }

        /// <summary>
        /// Returns the current rotation of the joint around the Y axis.
        /// </summary>
        public Radian SwingY
        {
            get
            {
                if (Native != null)
                    return Native.SwingY;

                return new Radian(0.0f);
            }
        }

        /// <summary>
        /// Returns the current rotation of the joint around the Z axis.
        /// </summary>
        public Radian SwingZ
        {
            get
            {
                if (Native != null)
                    return Native.SwingZ;

                return new Radian(0.0f);
            }
        }

        /// <summary>
        /// Linear limit used for constraining translation degrees of freedom.
        /// </summary>
        public LimitLinear LimitLinear
        {
            get { return data.@internal.linearLimit; }
            set
            {
                if (data.@internal.linearLimit == value)
                    return;

                data.@internal.linearLimit = value;

                if (Native != null)
                    Native.LimitLinear = value;
            }
        }

        /// <summary>
        /// Angular limit used for constraining the twist (rotation around X) degree of freedom.
        /// </summary>
        public LimitAngularRange LimitTwist
        {
            get { return data.@internal.twistLimit; }
            set
            {
                if (data.@internal.twistLimit == value)
                    return;

                data.@internal.twistLimit = value;

                if (Native != null)
                    Native.LimitTwist = value;
            }
        }

        /// <summary>
        /// Cone limit used for constraining the swing (rotation around Y and Z) degree of freedom.
        /// </summary>
        public LimitConeRange LimitSwing
        {
            get { return data.@internal.swingLimit; }
            set
            {
                if (data.@internal.swingLimit == value)
                    return;

                data.@internal.swingLimit = value;

                if (Native != null)
                    Native.LimitSwing = value;
            }
        }

        /// <summary>
        /// Determines the drive's target position relative to the joint's first body. This is the position the drive will
        /// attempt to reach if enabled.
        /// </summary>
        public Vector3 DrivePosition
        {
            get { return data.@internal.drivePosition; }
            set
            {
                if (data.@internal.drivePosition == value)
                    return;

                data.@internal.drivePosition = value;

                if (Native != null)
                    Native.DrivePosition = value;
            }
        }

        /// <summary>
        /// Determines the drive's target orientation relative to the joint's first body. This is the orientation the drive
        /// will attempt to reach if enabled.
        /// </summary>
        public Quaternion DriveRotation
        {
            get { return data.@internal.driveRotation; }
            set
            {
                if (data.@internal.driveRotation == value)
                    return;

                data.@internal.driveRotation = value;

                if (Native != null)
                    Native.DriveRotation = value;
            }
        }

        /// <summary>
        /// Determines the drive's target linear velocity. This is the velocity the drive will attempt to reach if enabled.
        /// </summary>
        public Vector3 DriveLinearVelocity
        {
            get { return data.@internal.driveLinearVelocity; }
            set
            {
                if (data.@internal.driveLinearVelocity == value)
                    return;

                data.@internal.driveLinearVelocity = value;

                if (Native != null)
                    Native.DriveLinearVelocity = value;
            }
        }

        /// <summary>
        /// Determines the drive's target angular velocity. This is the velocity the drive will attempt to reach if enabled.
        /// </summary>
        public Vector3 DriveAngularVelocity
        {
            get { return data.@internal.driveAngularVelocity; }
            set
            {
                if (data.@internal.driveAngularVelocity == value)
                    return;

                data.@internal.driveAngularVelocity = value;

                if (Native != null)
                    Native.DriveAngularVelocity = value;
            }
        }

        /// <summary>
        /// Returns the type of motion constrain for the specified axis.
        /// </summary>
        /// <param name="axis">Axis to retrieve the motion constrain for.</param>
        /// <returns>Motion constrain type for the axis.</returns>
        public D6JointMotion GetMotion(D6JointAxis axis)
        {
            return data.@internal.motion[(int) axis];
        }

        /// <summary>
        /// Allows you to constrain motion of the specified axis. Be aware that when setting drives for a specific axis
        /// you must also take care not to constrain its motion in a conflicting way(for example you cannot add a drive
        /// that moves the joint on X axis, and then lock the X axis).
        /// 
        /// Unlocking translations degrees of freedom allows the bodies to move along the subset of the unlocked axes.
        /// (for example unlocking just one translational axis is the equivalent of a slider joint.)
        /// 
        /// Angular degrees of freedom are partitioned as twist(around X axis) and swing(around Y and Z axes). Different
        /// effects can be achieves by unlocking their various combinations: 
        ///  - If a single degree of angular freedom is unlocked it should be the twist degree as it has extra options for
        ///    that case (for example for a hinge joint).
        ///  - If both swing degrees are unlocked but twist is locked the result is a zero-twist joint.
        ///  - If one swing and one twist degree of freedom are unlocked the result is a zero-swing joint (for example an 
        ///    arm attached at the elbow)
        ///  - If all angular degrees of freedom are unlocked the result is the same as the spherical joint.
        /// </summary>
        /// <param name="axis">Axis to change the motion type for.</param>
        /// <param name="motion">Type of motion for the axis.</param>
        public void SetMotion(D6JointAxis axis, D6JointMotion motion)
        {
            if (data.@internal.motion[(int)axis] == motion)
                return;

            data.@internal.motion[(int)axis] = motion;

            if (Native != null)
                Native.SetMotion(axis, motion);
        }

        /// <summary>
        /// Returns properties for the specified drive type.
        /// </summary>
        /// <param name="type">Type of drive to retrieve properties for.</param>
        /// <returns>Properties for the requested drive type.</returns>
        public D6JointDrive GetDrive(D6JointDriveType type)
        {
            return data.@internal.drives[(int) type];
        }

        /// <summary>
        /// Sets a drive that will attempt to move the specified degree(s) of freedom to the wanted position and velocity. 
        /// </summary>
        /// <param name="type">Type of the drive.</param>
        /// <param name="drive">Drive properties.</param>
        public void SetDrive(D6JointDriveType type, D6JointDrive drive)
        {
            if (data.@internal.drives[(int)type] == drive)
                return;

            data.@internal.drives[(int)type] = drive;

            if (Native != null)
                Native.SetDrive(type, drive);
        }

        /// <summary>
        /// Returns the native joint wrapped by this component.
        /// </summary>
        private NativeD6Joint Native
        {
            get { return (NativeD6Joint)native; }
        }

        /// <inheritdoc/>
        internal override NativeJoint CreateNative()
        {
            NativeD6Joint joint = new NativeD6Joint(commonData.@internal, data.@internal);

            return joint;
        }

        /// <summary>
        /// Holds all data the joint component needs to persist through serialization.
        /// </summary>
        [SerializeObject]
        internal new class SerializableData
        {
            public ScriptD6JointData @internal;

            public SerializableData()
            {
                @internal.linearLimit = new LimitLinear();
                @internal.twistLimit = new LimitAngularRange();
                @internal.swingLimit = new LimitConeRange();
                @internal.motion = new D6JointMotion[(int)D6JointAxis.Count];
                @internal.drives = new D6JointDrive[(int)D6JointDriveType.Count];
                @internal.drivePosition = Vector3.Zero;
                @internal.driveRotation = Quaternion.Identity;
                @internal.driveLinearVelocity = Vector3.Zero;
                @internal.driveAngularVelocity = Vector3.Zero;
            }
        }
    }
}
