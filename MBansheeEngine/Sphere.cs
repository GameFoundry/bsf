using System.Runtime.InteropServices;

namespace BansheeEngine
{
    [StructLayout(LayoutKind.Sequential), SerializeObject]
    public struct Sphere
    {
        private float _radius;
        private Vector3 _center;
        
        public Vector3 Center
        {
            get { return _center; }
            set { _center = value; }
        }

        public float Radius
        {
            get { return _radius; }
            set { _radius = value; }
        }

        public Sphere(Vector3 center, float radius)
        {
            _center = center;
            _radius = radius;
        }
    };
}
