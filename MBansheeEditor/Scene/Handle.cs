//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
using System.Collections.Generic;
using BansheeEngine;

namespace BansheeEditor
{
    /// <summary>
    /// Base class for handle implementations. Handles provide tools that the user can interact with in the scene view.
    /// This includes traditional handles like move, rotate or scale, but also custom user defined handles. Custom handles
    /// need to implement this class and also specify a <see cref="CustomHandle"/> attribute.
    /// </summary>
    public abstract class Handle
    {
        private List<HandleSlider> sliders = new List<HandleSlider>();

        /// <summary>
        /// Called every frame before handle input is processed. Allows handle transforms to be updated before input.
        /// </summary>
        protected internal abstract void PreInput();

        /// <summary>
        /// Called every frame after handle input is processed. Active handle sliders should contain values signals if and
        /// how much they were dragged.
        /// </summary>
        protected internal abstract void PostInput();

        /// <summary>
        /// Called every frame after <see cref="PostInput"/>. Allows handles graphics to be drawn. This is the only
        /// method that draw methods <see cref="Handles"/> may be called.
        /// </summary>
        protected internal abstract void Draw();

        /// <summary>
        /// Registers a new handle slider used by this handle.
        /// </summary>
        /// <param name="slider">Slider used by the handle.</param>
        internal void RegisterSlider(HandleSlider slider)
        {
            sliders.Add(slider);
        }

        /// <summary>
        /// Destroys the handle, removing it from the scene.
        /// </summary>
        public void Destroy()
        {
            foreach (var slider in sliders)
                slider.Destroy();
        }
    }
}
