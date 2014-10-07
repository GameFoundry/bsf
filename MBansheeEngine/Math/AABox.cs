using System.Runtime.InteropServices;

namespace BansheeEngine
{
    [StructLayout(LayoutKind.Sequential), SerializeObject]
	public struct AABox
	{
        private Vector3 minimum;
		private Vector3 maximum;

        public Vector3 Minimum
        {
            get { return minimum; }
            set { minimum = value; }
        }

        public Vector3 Maximum
        {
            get { return maximum; }
            set { maximum = value; }
        }

        public Vector3 Center
        {
            get 
            { 		
                return new Vector3((maximum.x + minimum.x) * 0.5f,
			            (maximum.y + minimum.y) * 0.5f,
			            (maximum.z + minimum.z) * 0.5f);
            }
        }

	    public Vector3 Size
	    {
	        get
	        {
	            return maximum - minimum;
	        }
	    }

        public AABox(Vector3 min, Vector3 max)
        {
            minimum = min;
            maximum = max;
        }
	};
}
