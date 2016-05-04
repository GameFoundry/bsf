//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
using System.Runtime.InteropServices;

namespace BansheeEngine
{
    /** @addtogroup Math
     *  @{
     */

    /// <summary>
    /// Represents a capsule with a line segment and a radius.
    /// </summary>
    [StructLayout(LayoutKind.Sequential), SerializeObject]
    public struct Capsule // Note: Must match C++ struct Capsule
    {
        [SerializeField]
        private LineSegment segment;

        [SerializeField]
        private float radius;

        /// <summary>
        /// Radius that defines the distance of the capsule from its line segment.
        /// </summary>
        public float Radius
        {
            get { return radius; }
            set { radius = value; }
        }

        /// <summary>
        /// Line segment along which the capsule lies. All capsule points are at equal distance from this segment.
        /// </summary>
        public LineSegment Segment
        {
            get { return segment; }
            set { segment = value; }
        }

        /// <summary>
        /// Creates a new capsule object.
        /// </summary>
        /// <param name="segment">Line segment along which the capsule lies.</param>
        /// <param name="radius">Radius that defines the distance of the capsule from its line segment.</param>
        public Capsule(LineSegment segment, float radius)
        {
            this.segment = segment;
            this.radius = radius;
        }
    };

    /** @} */
}
