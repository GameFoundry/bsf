//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
using BansheeEngine;

namespace BansheeEditor
{
    /// <summary>
    /// Base class for all the built-in handle types.
    /// </summary>
    public abstract class DefaultHandle : Handle
    {
        protected Vector3 position;
        protected Quaternion rotation;

        /// <summary>
        /// World position of the handle.
        /// </summary>
        public Vector3 Position
        {
            get { return position; }
            set { position = value; }
        }

        /// <summary>
        /// World rotation of the handle.
        /// </summary>
        public Quaternion Rotation
        {
            get { return rotation; }
            set { rotation = value; }
        }

        /// <summary>
        /// Checks is the handle currently being interacted with.
        /// </summary>
        /// <returns>True if the handle is being interacted with (i.e. dragged or clicked).</returns>
        internal abstract bool IsDragged();
    }
}
