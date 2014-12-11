using System.Runtime.InteropServices;

namespace BansheeEngine
{
    [StructLayout(LayoutKind.Sequential), SerializeObject]
	public struct AABox
	{
        private Vector3 _minimum;
		private Vector3 _maximum;

        public Vector3 minimum
        {
            get { return _minimum; }
            set { _minimum = value; }
        }

        public Vector3 maximum
        {
            get { return _maximum; }
            set { _maximum = value; }
        }

        public Vector3 center
        {
            get 
            { 		
                return new Vector3((_maximum.x + _minimum.x) * 0.5f,
			            (_maximum.y + _minimum.y) * 0.5f,
			            (_maximum.z + _minimum.z) * 0.5f);
            }
        }

	    public Vector3 size
	    {
	        get
	        {
	            return _maximum - _minimum;
	        }
	    }

        public AABox(Vector3 min, Vector3 max)
        {
            _minimum = min;
            _maximum = max;
        }
	};
}
