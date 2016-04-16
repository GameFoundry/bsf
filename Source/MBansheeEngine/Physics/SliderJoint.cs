//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
namespace BansheeEngine
{
    /** @addtogroup Physics
     *  @{
     */

    /// <summary>
    /// Joint that removes all but a single translational degree of freedom. Bodies are allowed to move along a single axis.
    /// </summary>
    public sealed class SliderJoint : Joint
    {
        [SerializeField]
        private SerializableData data = new SerializableData();

        /// <summary>
        /// Determines the limit that constrains the movement of the joint to a specific minimum and maximum distance. You
        /// must enable <see cref="EnableLimit"/> for this to be enforced.
        /// </summary>
        public LimitLinearRange Limit
        {
            get { return data.@internal.limit; }
            set
            {
                if (data.@internal.limit == value)
                    return;

                data.@internal.limit = value;

                if (Native != null)
                    Native.Limit = value;
            }
        }

        /// <summary>
        /// Enables or disables the limit that clamps the movement of the joint.
        /// </summary>
        public bool EnableLimit
        {
            get { return data.@internal.enableLimit; }
            set
            {
                if (data.@internal.enableLimit == value)
                    return;

                data.@internal.enableLimit = value;

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
            NativeSliderJoint joint = new NativeSliderJoint(commonData.@internal, data.@internal);

            return joint;
        }

        /// <summary>
        /// Holds all data the joint component needs to persist through serialization.
        /// </summary>
        [SerializeObject]
        internal new class SerializableData
        {
            public ScriptSliderJointData @internal;

            public SerializableData()
            {
                @internal.limit = new LimitLinearRange();
                @internal.enableLimit = false;
            }
        }
    }

    /** @} */
}
