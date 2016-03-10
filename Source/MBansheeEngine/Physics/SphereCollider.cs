//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
namespace BansheeEngine
{
    /// <summary>
    /// Collider with sphere geometry.
    /// </summary>
    public sealed class SphereCollider : Collider
    {
        [SerializeField]
        private float radius = 1.0f;

        /// <summary>
        /// Radius of the sphere.
        /// </summary>
        public float Radius
        {
            get { return radius; }
            set
            {
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
        /// Position of the sphere shape, relative to the component's scene object.
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
        /// Returns the native sphere collider wrapped by this component.
        /// </summary>
        private NativeSphereCollider Native
        {
            get { return (NativeSphereCollider)native; }
        }

        /// <inheritdoc/>
        internal override NativeCollider CreateCollider()
        {
            NativeSphereCollider sphereCollider = new NativeSphereCollider();
            sphereCollider.Radius = radius;

            return sphereCollider;
        }
    }
}
