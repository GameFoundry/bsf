namespace BansheeEngine
{
    /// <summary>
    /// Collider with plane geometry.
    /// </summary>
    public sealed class PlaneCollider : Collider
    {
        [SerializeField]
        private float distance;

        [SerializeField]
        private Vector3 normal;

        /// <summary>
        /// Distance of the plane from the local origin, along its normal vector.
        /// </summary>
        public float Distance
        {
            get { return distance; }
            set
            {
                if (distance == value)
                    return;

                distance = value;
                serializableData.localPosition = normal * distance;

                if (Native != null)
                    UpdateTransform();
            }
        }

        /// <summary>
        /// Normal vector of the plane. It determines how is the plane oriented.
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

        /// <inheritdoc/>
        protected override bool IsValidParent(Rigidbody parent)
    	{
		    // Planes cannot be added to non-kinematic rigidbodies
		    return parent.IsKinematic;
        }

        /// <summary>
        /// Returns the native plane collider wrapped by this component.
        /// </summary>
        private NativePlaneCollider Native
        {
            get { return (NativePlaneCollider)native; }
        }

        /// <inheritdoc/>
        internal override NativeCollider CreateCollider()
        {
            return new NativePlaneCollider();
        }
    }
}
