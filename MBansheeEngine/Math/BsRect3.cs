using System.Runtime.InteropServices;

namespace BansheeEngine
{
    [StructLayout(LayoutKind.Sequential), SerializeObject]
	public struct Rect3
	{
        public Rect3(Vector3 center, Vector3[] axes, float[] extents)
        {
            this._center = center;
            _axisHorz = axes[0];
            _axisVert = axes[1];
            _extentHorz = extents[0];
            _extentVert = extents[1];
        }

        public Vector3 center
        {
            get { return _center; }
            set { _center = value; }
        }

        public Vector3 axisHorz
        {
            get { return _axisHorz; }
            set { _axisHorz = value; }
        }

        public Vector3 axisVert
        {
            get { return _axisVert; }
            set { _axisVert = value; }
        }

        public float extentHorz
        {
            get { return _extentHorz; }
            set { _extentHorz = value; }
        }

        public float extentVert
        {
            get { return _extentVert; }
            set { _extentVert = value; }
        }

		private Vector3 _center;
        private Vector3 _axisHorz;
        private Vector3 _axisVert;
        private float _extentHorz;
        private float _extentVert;
	};
}
