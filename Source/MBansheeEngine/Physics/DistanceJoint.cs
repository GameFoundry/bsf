//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
using System;

namespace BansheeEngine
{
    /// <summary>
    /// A joint that maintains an upper or lower (or both) bound on the distance between two bodies.
    /// </summary>
    public sealed class DistanceJoint : Joint
    {
        /// <summary>
        /// Flags to control distance joint behaviour.
        /// </summary>
        [Flags]
        internal enum Flag // Note: Must match C++ enum DistanceJoint::Flag
        {
            MinDistanceLimit = 0x01,
            MaxDistanceLimit = 0x02,
            Spring = 0x04
        }

        [SerializeField]
        private SerializableData data = new SerializableData();

        /// <summary>
        /// Returns the current distance between the two joint bodies.
        /// </summary>
        public float Distance
        {
            get
            {
                if(Native != null)
                    return Native.Distance;

                return 0.0f;
            }
        }

        /// <summary>
        /// Determines the minimum distance the bodies are allowed to be at, they will get no closer. You must enable
        /// <see cref="EnableMinDistanceLimit"/> in order for this to be enforced.
        /// </summary>
        public float MinDistance
        {
            get { return data.@internal.minDistance; }
            set
            {
                if (data.@internal.minDistance == value)
                    return;

                data.@internal.minDistance = value;

                if (Native != null)
                    Native.MinDistance = value;
            }
        }

        /// <summary>
        /// Determines the maximum distance the bodies are allowed to be at, they will get no further. You must enable
        /// <see cref="EnableMaxDistanceLimit"/> in order for this to be enforced.
        /// </summary>
        public float MaxDistance
        {
            get { return data.@internal.maxDistance; }
            set
            {
                if (data.@internal.maxDistance == value)
                    return;

                data.@internal.maxDistance = value;

                if (Native != null)
                    Native.MaxDistance = value;
            }
        }

        /// <summary>
        /// Determines the error tolerance of the joint at which the joint becomes active. This value slightly extends the
        /// lower and upper limit for precision reasons.
        /// </summary>
        public float Tolerance
        {
            get { return data.@internal.tolerance; }
            set
            {
                if (data.@internal.tolerance == value)
                    return;

                data.@internal.tolerance = value;

                if (Native != null)
                    Native.Tolerance = value;
            }
        }

        /// <summary>
        /// Returns a spring that controls how the joint responds when a limit is reached. You must enable 
        /// <see cref="EnableSpring"/> in order for the spring to be applied.
        /// </summary>
        public Spring Spring
        {
            get { return data.@internal.spring; }
            set
            {
                if (data.@internal.spring == value)
                    return;

                data.@internal.spring = value;

                if (Native != null)
                    Native.Spring = value;
            }
        }

        /// <summary>
        /// Enables or disables the limit that causes joint objects to maintain a minimum distance between themselves.
        /// </summary>
        public bool EnableMinDistanceLimit
        {
            get { return (data.@internal.flags & Flag.MinDistanceLimit) != 0; }
            set
            {
                if (!SetFlag(Flag.MinDistanceLimit, value))
                    return;

                if (Native != null)
                    Native.EnableMinDistanceLimit = value;
            }
        }

        /// <summary>
        /// Enables or disables the limit that causes joint objects to maintain a maximum distance between themselves.
        /// </summary>
        public bool EnableMaxDistanceLimit
        {
            get { return (data.@internal.flags & Flag.MaxDistanceLimit) != 0; }
            set
            {
                if (!SetFlag(Flag.MaxDistanceLimit, value))
                    return;

                if (Native != null)
                    Native.EnableMinDistanceLimit = value;
            }
        }

        /// <summary>
        /// Enables or disables the spring that controls how the joint reacts when the limit is reached.
        /// </summary>
        public bool EnableSpring
        {
            get { return (data.@internal.flags & Flag.Spring) != 0; }
            set
            {
                if (!SetFlag(Flag.Spring, value))
                    return;

                if (Native != null)
                    Native.EnableSpring = value;
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
            Flag newFlags = data.@internal.flags;

            if (enabled)
                newFlags |= flag;
            else
                newFlags &= ~flag;

            if (newFlags == data.@internal.flags)
                return false;

            data.@internal.flags = newFlags;
            return true;
        }

        /// <summary>
        /// Returns the native joint wrapped by this component.
        /// </summary>
        private NativeDistanceJoint Native
        {
            get { return (NativeDistanceJoint)native; }
        }

        /// <inheritdoc/>
        internal override NativeJoint CreateNative()
        {
            NativeDistanceJoint joint = new NativeDistanceJoint(commonData.@internal, data.@internal);

            return joint;
        }

        /// <summary>
        /// Holds all data the joint component needs to persist through serialization.
        /// </summary>
        [SerializeObject]
        internal new class SerializableData
        {
            public ScriptDistanceJointData @internal;

            public SerializableData()
            {
                @internal.minDistance = 0.0f;
                @internal.maxDistance = 0.0f;
                @internal.tolerance = 0.25f;
                @internal.flags = 0;
            }
        }
    }
}
