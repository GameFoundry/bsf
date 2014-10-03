using System.Runtime.InteropServices;

namespace BansheeEngine
{
    [StructLayout(LayoutKind.Sequential), SerializeObject]
	public class Rect3
	{
        public Rect3(Vector3 center, Vector3[] axes, float[] extents)
        {
            this.center = center;
            this.axes[0] = axes[0];
            this.axes[1] = axes[1];
            this.extents[0] = extents[0];
            this.extents[1] = extents[1];
        }

        public Vector3 Center
        {
            get { return center; }
            set { center = value; }
        }

        public Vector3 AxisHorz
        {
            get { return axes[0]; }
            set { axes[0] = value; }
        }

        public Vector3 AxisVert
        {
            get { return axes[1]; }
            set { axes[1] = value; }
        }

        public float ExtentHorz
        {
            get { return extents[0]; }
            set { extents[0] = value; }
        }

        public float ExtentVert
        {
            get { return extents[1]; }
            set { extents[1] = value; }
        }

		private Vector3 center;
        private Vector3[] axes = new Vector3[2];
        private float[] extents = new float[2];
	};
}
