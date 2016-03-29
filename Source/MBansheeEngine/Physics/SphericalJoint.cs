//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
using System;

namespace BansheeEngine
{
    /// <summary>
    /// A spherical joint removes all translational degrees of freedom but allows all rotational degrees of freedom. 
    /// Essentially this ensures that the anchor points of the two bodies are always coincident. Bodies are allowed to
    /// rotate around the anchor points, and their rotatation can be limited by an elliptical cone.
    /// </summary>
    public sealed class SphericalJoint : Joint
    {
        [SerializeField]
        private SerializableData data = new SerializableData();

        /// <summary>
        /// Determines the limit that clamps the rotation of the joint inside an eliptical angular cone. You must enable 
        /// <see cref="EnableLimit"/> for this to be enforced.
        /// </summary>
        public LimitConeRange Limit
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
        /// Enables or disables the limit that clamps the rotation of the joint.
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
        private NativeSphericalJoint Native
        {
            get { return (NativeSphericalJoint)native; }
        }

        /// <inheritdoc/>
        internal override NativeJoint CreateNative()
        {
            NativeSphericalJoint joint = new NativeSphericalJoint(commonData.@internal, data.@internal);

            return joint;
        }

        /// <summary>
        /// Holds all data the joint component needs to persist through serialization.
        /// </summary>
        [SerializeObject]
        internal new class SerializableData
        {
            public ScriptSphericalJointData @internal;

            public SerializableData()
            {
                @internal.limit = new LimitConeRange();
                @internal.enableLimit = false;
            }
        }
    }
}
