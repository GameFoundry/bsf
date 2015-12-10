using System.Runtime.InteropServices;

namespace BansheeEngine
{
    /// <summary>
    /// A sphere represented by a center point and a radius.
    /// </summary>
    [StructLayout(LayoutKind.Sequential), SerializeObject]
    public struct Sphere // Note: Must match C++ enum Sphere
    {
        [SerializeField]
        private float _radius;

        [SerializeField]
        private Vector3 _center;
        
        /// <summary>
        /// Center point of the sphere.
        /// </summary>
        public Vector3 Center
        {
            get { return _center; }
            set { _center = value; }
        }

        /// <summary>
        /// Radius of the sphere.
        /// </summary>
        public float Radius
        {
            get { return _radius; }
            set { _radius = value; }
        }

        /// <summary>
        /// Creates a new sphere object.
        /// </summary>
        /// <param name="center">Center point of the sphere.</param>
        /// <param name="radius">Radius of the sphere.</param>
        public Sphere(Vector3 center, float radius)
        {
            _center = center;
            _radius = radius;
        }
    };
}
