//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
using System;

namespace BansheeEngine
{
    /// <summary>
    /// Hinge joint removes all but a single rotation degree of freedom from its two attached bodies (for example a door
    /// hinge).
    /// </summary>
    public sealed class HingeJoint : Joint
    {
        /// <summary>
        /// Flags to control hinge joint behaviour.
        /// </summary>
        [Flags]
        private enum Flag
        {
            Limit = 0x01,
            Drive = 0x02,
        }

        [SerializeField]
        private LimitAngularRange limit = new LimitAngularRange();
        [SerializeField]
        private HingeJointDrive drive = new HingeJointDrive();
        [SerializeField]
        private Flag flags = 0;

        /// <summary>
        /// Returns the current angle between the two attached bodes.
        /// </summary>
        public Radian Angle
        {
            get
            {
                if (Native != null)
                    return Native.Angle;

                return new Radian(0.0f);
            }
        }

        /// <summary>
        /// Returns the current angular speed of the joint.
        /// </summary>
        public float Speed
        {
            get
            {
                if (Native != null)
                    return Native.Speed;

                return 0.0f;
            }
        }

        /// <summary>
        /// Determines the limit of the joint. Limit constrains the motion to the specified angle range. You must enable
        /// <see cref="EnableLimit"/> for this to be enforced.
        /// </summary>
        public LimitAngularRange Limit
        {
            get { return limit; }
            set
            {
                if (limit == value)
                    return;

                limit = value;

                if (Native != null)
                    Native.Limit = value;
            }
        }

        /// <summary>
        /// Determines the drive properties of the joint. It drives the joint's angular velocity towards a particular value.
        /// You must enable <see cref="EnableDrive"/> for this to be applied.
        /// </summary>
        public HingeJointDrive Drive
        {
            get { return drive; }
            set
            {
                if (drive == value)
                    return;

                drive = value;

                if (Native != null)
                    Native.Drive = value;
            }
        }

        /// <summary>
        /// Enables or disables a limit that contrains the joint's motion to a specified angle range.
        /// </summary>
        public bool EnableLimit
        {
            get { return (flags & Flag.Limit) != 0; }
            set
            {
                if (!SetFlag(Flag.Limit, value))
                    return;

                if (Native != null)
                    Native.EnableLimit = value;
            }
        }

        /// <summary>
        /// Enables or disables a drive that drives the joint's angular velocity towards a particular value.
        /// </summary>
        public bool EnableDrive
        {
            get { return (flags & Flag.Drive) != 0; }
            set
            {
                if (!SetFlag(Flag.Drive, value))
                    return;

                if (Native != null)
                    Native.EnableDrive = value;
            }
        }

        /// <summary>
        /// Toggles a specific distance joint flag on or off.
        /// </summary>
        /// <param name="flag">Flag to toggle.</param>
        /// <param name="enabled">Should the flag be turned on or off.</param>
        /// <returns>True if the new newly set flag state was different from the previous one.</returns>
        private bool SetFlag(Flag flag, bool enabled)
        {
            Flag newFlags = flags;

            if (enabled)
                newFlags |= flag;
            else
                newFlags &= ~flag;

            if (newFlags == flags)
                return false;

            flags = newFlags;
            return true;
        }

        /// <summary>
        /// Returns the native joint wrapped by this component.
        /// </summary>
        private NativeHingeJoint Native
        {
            get { return (NativeHingeJoint)native; }
        }

        /// <inheritdoc/>
        internal override NativeJoint CreateNative()
        {
            NativeHingeJoint joint = new NativeHingeJoint();

            // TODO - Apply this all at once to avoid all the individual interop function calls
            joint.Limit = limit;
            joint.Drive = drive;
            joint.EnableLimit = EnableLimit;
            joint.EnableDrive = EnableDrive;

            return joint;
        }
    }
}
