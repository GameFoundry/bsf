//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
namespace BansheeEngine
{
    /** @addtogroup Physics
     *  @{
     */

    /// <summary>
    /// Collider with plane geometry.
    /// </summary>
    public sealed class PlaneCollider : Collider
    {
        [SerializeField]
        private float distance;

        [SerializeField]
        private Vector3 normal = new Vector3(1.0f, 0.0f, 0.0f);

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
        protected internal override bool IsValidParent(Rigidbody parent)
    	{
		    // Planes cannot be added to non-kinematic rigidbodies
		    return parent.Kinematic;
        }

        /// <summary>
        /// Returns the native plane collider wrapped by this component.
        /// </summary>
        private NativePlaneCollider Native
        {
            get { return (NativePlaneCollider)native; }
        }

        public PlaneCollider()
        {
            serializableData.localRotation = Quaternion.FromToRotation(Vector3.XAxis, normal);
        }

        /// <inheritdoc/>
        internal override NativeCollider CreateCollider()
        {
            return new NativePlaneCollider();
        }
    }

    /** @} */
}
