//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//

using System.Runtime.InteropServices;

namespace BansheeEngine
{
    /// <summary>
    /// Represents a line segment in three dimensional space defined by a start and an end point.
    /// </summary>
    [StructLayout(LayoutKind.Sequential), SerializeObject]
    public struct LineSegment
    {
        [SerializeField]
        private Vector3 start;

        [SerializeField]
        private Vector3 end;

        /// <summary>
        /// Start position of the line segment.
        /// </summary>
        public Vector3 Start
        {
            get { return start; }
            set { start = value; }
        }

        /// <summary>
        /// End position of the line segment.
        /// </summary>
        public Vector3 End
        {
            get { return end; }
            set { end = value; }
        }

        /// <summary>
        /// Creates a new line segment.
        /// </summary>
        /// <param name="start">Line segment start position.</param>
        /// <param name="end">Line segment end position.</param>
        public LineSegment(Vector3 start, Vector3 end)
        {
            this.start = start;
            this.end = end;
        }
    }
}
