//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
namespace BansheeEngine
{
    /// <summary>
    /// Collider with capsule geometry.
    /// </summary>
    public sealed class CapsuleCollider : Collider
    {
        [SerializeField]
        private float radius = 0.2f;

        [SerializeField]
        private float halfHeight = 1.0f;

        [SerializeField]
        private Vector3 normal = Vector3.YAxis;

        /// <summary>
        /// Radius of the capsule.
        /// </summary>
        public float Radius
        {
            get { return radius; }
            set
            {
                value = MathEx.Max(value, 0.01f);
                if (radius == value)
                    return;

                radius = value;

                if (Native != null)
                {
                    Native.Radius = value;

                    if (parent != null)
                        parent.UpdateMassDistribution();
                }
            }
        }

        /// <summary>
        /// Half height of the capsule, from the origin to one of the hemispherical centers, along the normal vector.
        /// </summary>
        public float HalfHeight
        {
            get { return halfHeight; }
            set
            {
                value = MathEx.Max(value, 0.01f);
                if (halfHeight == value)
                    return;

                halfHeight = value;

                if (Native != null)
                {
                    Native.HalfHeight = value;

                    if (parent != null)
                        parent.UpdateMassDistribution();
                }
            }
        }
        
        /// <summary>
        /// Position of the capsule shape, relative to the component's scene object.
        /// </summary>
        public Vector3 Center
        {
            get { return serializableData.localPosition; }
            set
            {
                if (serializableData.localPosition == value)
                    return;

                serializableData.localPosition = value;

                if (Native != null)
                    UpdateTransform();
            }
        }

        /// <summary>
        /// Normal vector of the capsule. It determines how is the capsule oriented.
        /// </summary>
        public Vector3 Normal
        {
            get { return normal; }
            set
            {
                if (normal == value)
                    return;

                normal = value.Normalized;
                serializableData.localRotation = Quaternion.FromToRotation(Vector3.XAxis, normal);

                if (Native != null)
                    UpdateTransform();
            }
        }

        public CapsuleCollider()
        {
            serializableData.localRotation = Quaternion.FromToRotation(Vector3.XAxis, normal);
        }

        /// <summary>
        /// Returns the native capsule collider wrapped by this component.
        /// </summary>
        private NativeCapsuleCollider Native
        {
            get { return (NativeCapsuleCollider)native; }
        }

        /// <inheritdoc/>
        internal override NativeCollider CreateCollider()
        {
            NativeCapsuleCollider capsuleCollider = new NativeCapsuleCollider();
            capsuleCollider.Radius = radius;
            capsuleCollider.HalfHeight = halfHeight;

            return capsuleCollider;
        }
    }
}
