//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
namespace BansheeEngine
{
    /// <summary>
    /// Bounds represented by an axis aligned box and a sphere.
    /// </summary>
    public struct Bounds
    {
        /// <summary>
        /// Creates new bounds.
        /// </summary>
        /// <param name="box">Axis aligned box representation of the bounds.</param>
        /// <param name="sphere">Sphere representation of the bounds.</param>
        public Bounds(AABox box, Sphere sphere)
        {
            Box = box;
            Sphere = sphere;
        }

        /// <summary>
        /// Axis aligned box representation of the bounds.
        /// </summary>
        public AABox Box;

        /// <summary>
        /// Sphere representation of the bounds.
        /// </summary>
        public Sphere Sphere;
    }
}
