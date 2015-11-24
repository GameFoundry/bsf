using System.Runtime.InteropServices;

namespace BansheeEngine
{
    /// <summary>
    /// Axis aligned box represented by minimum and maximum point.
    /// </summary>
    [StructLayout(LayoutKind.Sequential), SerializeObject]
	public struct AABox // Note: Must match C++ class AABox
	{
        private Vector3 _minimum;
		private Vector3 _maximum;

        /// <summary>
        /// Corner of the box with minimum values (opposite to maximum corner).
        /// </summary>
        public Vector3 Minimum
        {
            get { return _minimum; }
            set { _minimum = value; }
        }

        /// <summary>
        /// Corner of the box with maximum values (opposite to minimum corner).
        /// </summary>
        public Vector3 Maximum
        {
            get { return _maximum; }
            set { _maximum = value; }
        }

        /// <summary>
        /// Returns the center of the box.
        /// </summary>
        public Vector3 Center
        {
            get 
            { 		
                return new Vector3((_maximum.x + _minimum.x) * 0.5f,
			            (_maximum.y + _minimum.y) * 0.5f,
			            (_maximum.z + _minimum.z) * 0.5f);
            }
        }

        /// <summary>
        /// Returns the width, height and depth of the box.
        /// </summary>
	    public Vector3 Size
	    {
	        get
	        {
	            return _maximum - _minimum;
	        }
	    }

        /// <summary>
        /// Creates a new axis aligned box.
        /// </summary>
        /// <param name="min">Corner of the box with minimum values.</param>
        /// <param name="max">Corner of the box with maximum values.</param>
        public AABox(Vector3 min, Vector3 max)
        {
            _minimum = min;
            _maximum = max;
        }

        /// <inheritdoc/>
        public override string ToString()
        {
            return "Min: " + _minimum + ". Max: " + _maximum;
        }
	};
}
