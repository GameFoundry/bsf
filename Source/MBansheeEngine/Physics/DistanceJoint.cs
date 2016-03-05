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
        private enum Flag
        {
            MinDistanceLimit = 0x01,
            MaxDistanceLimit = 0x02,
            Spring = 0x04
        }

        [SerializeField]
        private float minDistance = 0.0f;
        [SerializeField]
        private float maxDistance = 0.0f;
        [SerializeField]
        private float tolerance = 0.25f;
        [SerializeField]
        private Spring spring;
        [SerializeField]
        private Flag flags = 0;

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
            get { return minDistance; }
            set
            {
                if (minDistance == value)
                    return;

                minDistance = value;

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
            get { return maxDistance; }
            set
            {
                if (maxDistance == value)
                    return;

                maxDistance = value;

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
            get { return tolerance; }
            set
            {
                if (tolerance == value)
                    return;

                tolerance = value;

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
            get { return spring; }
            set
            {
                if (spring == value)
                    return;

                spring = value;

                if (Native != null)
                    Native.Spring = value;
            }
        }

        /// <summary>
        /// Enables or disables the limit that causes joint objects to maintain a minimum distance between themselves.
        /// </summary>
        public bool EnableMinDistanceLimit
        {
            get { return (flags & Flag.MinDistanceLimit) != 0; }
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
            get { return (flags & Flag.MaxDistanceLimit) != 0; }
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
            get { return (flags & Flag.Spring) != 0; }
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
        private NativeDistanceJoint Native
        {
            get { return (NativeDistanceJoint)native; }
        }

        /// <inheritdoc/>
        internal override NativeJoint CreateNative()
        {
            NativeDistanceJoint joint = new NativeDistanceJoint();

            // TODO - Apply this all at once to avoid all the individual interop function calls
            joint.MinDistance = minDistance;
            joint.MaxDistance = maxDistance;
            joint.Tolerance = tolerance;
            joint.Spring = spring;
            joint.EnableMinDistanceLimit = EnableMinDistanceLimit;
            joint.EnableMaxDistanceLimit = EnableMaxDistanceLimit;
            joint.EnableSpring = EnableSpring;

            return joint;
        }
    }
}
