//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
namespace BansheeEngine
{
    /// <summary>
    /// Represents the most customizable type of joint. This joint type can be used to create all other built-in joint 
    /// types, and to design your own custom ones, but is less intuitive to use.Allows a specification of a linear
    /// constraint (e.g. for slider), twist constraint(rotating around X) and swing constraint(rotating around Y and Z).
    /// It also allows you to constrain limits to only specific axes or completely lock specific axes.
    /// </summary>
    public sealed class D6Joint : Joint
    {
        [SerializeField]
        private LimitLinear linearLimit = new LimitLinear();
        [SerializeField]
        private LimitAngularRange twistLimit = new LimitAngularRange();
        [SerializeField]
        private LimitConeRange swingLimit = new LimitConeRange();
        [SerializeField]
        private D6JointMotion[] motion = new D6JointMotion[(int)D6JointAxis.Count];
        [SerializeField]
        private D6JointDrive[] drives = new D6JointDrive[(int)D6JointDriveType.Count];
        [SerializeField]
        private Vector3 drivePosition = Vector3.Zero;
        [SerializeField]
        private Quaternion driveRotation = Quaternion.Identity;
        [SerializeField]
        private Vector3 driveLinearVelocity = Vector3.Zero;
        [SerializeField]
        private Vector3 driveAngularVelocity = Vector3.Zero;

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
            get { return linearLimit; }
            set
            {
                if (linearLimit == value)
                    return;

                linearLimit = value;

                if (Native != null)
                    Native.LimitLinear = value;
            }
        }

        /// <summary>
        /// Angular limit used for constraining the twist (rotation around X) degree of freedom.
        /// </summary>
        public LimitAngularRange LimitTwist
        {
            get { return twistLimit; }
            set
            {
                if (twistLimit == value)
                    return;

                twistLimit = value;

                if (Native != null)
                    Native.LimitTwist = value;
            }
        }

        /// <summary>
        /// Cone limit used for constraining the swing (rotation around Y and Z) degree of freedom.
        /// </summary>
        public LimitConeRange LimitSwing
        {
            get { return swingLimit; }
            set
            {
                if (swingLimit == value)
                    return;

                swingLimit = value;

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
            get { return drivePosition; }
            set
            {
                if (drivePosition == value)
                    return;

                drivePosition = value;

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
            get { return driveRotation; }
            set
            {
                if (driveRotation == value)
                    return;

                driveRotation = value;

                if (Native != null)
                    Native.DriveRotation = value;
            }
        }

        /// <summary>
        /// Determines the drive's target linear velocity. This is the velocity the drive will attempt to reach if enabled.
        /// </summary>
        public Vector3 DriveLinearVelocity
        {
            get { return driveLinearVelocity; }
            set
            {
                if (driveLinearVelocity == value)
                    return;

                driveLinearVelocity = value;

                if (Native != null)
                    Native.DriveLinearVelocity = value;
            }
        }

        /// <summary>
        /// Determines the drive's target angular velocity. This is the velocity the drive will attempt to reach if enabled.
        /// </summary>
        public Vector3 DriveAngularVelocity
        {
            get { return driveAngularVelocity; }
            set
            {
                if (driveAngularVelocity == value)
                    return;

                driveAngularVelocity = value;

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
            return motion[(int) axis];
        }

        /// <summary>
        /// Allows you to constrain motion of the specified axis. Be aware that when setting drives for a specific axis
        ///  you must also take care not to constrain its motion in a conflicting way(e.g.you cannot add a drive that
        /// moves the joint on X axis, and then lock the X axis).
        /// 
        /// Unlocking translations degrees of freedom allows the bodies to move along the subset of the unlocked axes.
        /// (e.g.unlocking just one translational axis is the equivalent of a slider joint.)
        /// 
        /// Angular degrees of freedom are partitioned as twist(around X axis) and swing(around Y and Z axes). Different
        /// effects can be achieves by unlocking their various combinations: 
        ///  - If a single degree of angular freedom is unlocked it should be the twist degree as it has extra options for
        ///    that case (e.g. for a hinge joint).
        ///  - If both swing degrees are unlocked but twist is locked the result is a zero-twist joint.
        ///  - If one swing and one twist degree of freedom are unlocked the result is a zero-swing joint (e.g.an arm
        ///    attached at the elbow)
        ///  - If all angular degrees of freedom are unlocked the result is the same as the spherical joint.
        /// </summary>
        /// <param name="axis">Axis to change the motion type for.</param>
        /// <param name="motion">Type of motion for the axis.</param>
        public void SetMotion(D6JointAxis axis, D6JointMotion motion)
        {
            if (this.motion[(int)axis] == motion)
                return;

            this.motion[(int)axis] = motion;

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
            return drives[(int) type];
        }

        /// <summary>
        /// Sets a drive that will attempt to move the specified degree(s) of freedom to the wanted position and velocity. 
        /// </summary>
        /// <param name="type">Type of the drive.</param>
        /// <param name="drive">Drive properties.</param>
        public void SetDrive(D6JointDriveType type, D6JointDrive drive)
        {
            if (this.drives[(int)type] == drive)
                return;

            this.drives[(int)type] = drive;

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
            NativeD6Joint joint = new NativeD6Joint();

            // TODO - Apply this all at once to avoid all the individual interop function calls
            joint.LimitLinear = linearLimit;
            joint.LimitTwist = twistLimit;
            joint.LimitSwing = swingLimit;

            for (int i = 0; i < (int) D6JointAxis.Count; i++)
                joint.SetMotion((D6JointAxis) i, motion[i]);

            for (int i = 0; i < (int)D6JointDriveType.Count; i++)
                joint.SetDrive((D6JointDriveType)i, drives[i]);

            joint.DrivePosition = drivePosition;
            joint.DriveRotation = driveRotation;
            joint.DriveLinearVelocity = driveLinearVelocity;
            joint.DriveAngularVelocity = driveAngularVelocity;

            return joint;
        }
    }
}
