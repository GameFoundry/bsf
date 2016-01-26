//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
using System;

namespace BansheeEngine
{
    /// <summary>
    /// Makes an integer or a floating point field be displayed as a slider with a specified range in the inspector.
    /// </summary>
    [AttributeUsage(AttributeTargets.Field)]
    public sealed class Range : Attribute
    {
        /// <summary>
        /// Creates a new range attribute.
        /// </summary>
        /// <param name="min">Minimum boundary of the range to clamp the field value to.</param>
        /// <param name="max">Maximum boundary of the range to clamp the field value to.</param>
        public Range(float min, float max)
        {
            this.min = min;
            this.max = max;
        }

        private float min;
        private float max;
    }
}
