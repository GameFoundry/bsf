//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
namespace BansheeEngine
{
    /// <summary>
    /// Joint that removes all but a single translational degree of freedom. Bodies are allowed to move along a single axis.
    /// </summary>
    public sealed class SliderJoint : Joint
    {
        [SerializeField]
        private LimitLinearRange limit = new LimitLinearRange();
        [SerializeField]
        private bool enableLimit;

        /// <summary>
        /// Determines the limit that constrains the movement of the joint to a specific minimum and maximum distance. You
        /// must enable <see cref="EnableLimit"/> for this to be enforced.
        /// </summary>
        public LimitLinearRange Limit
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
        /// Enables or disables the limit that clamps the movement of the joint.
        /// </summary>
        public bool EnableLimit
        {
            get { return enableLimit; }
            set
            {
                if (enableLimit == value)
                    return;

                enableLimit = value;

                if (Native != null)
                    Native.EnableLimit = value;
            }
        }

        /// <summary>
        /// Returns the native joint wrapped by this component.
        /// </summary>
        private NativeSliderJoint Native
        {
            get { return (NativeSliderJoint)native; }
        }

        /// <inheritdoc/>
        internal override NativeJoint CreateNative()
        {
            NativeSliderJoint joint = new NativeSliderJoint();

            // TODO - Apply this all at once to avoid all the individual interop function calls
            joint.Limit = limit;
            joint.EnableLimit = enableLimit;

            return joint;
        }
    }
}
