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

        protected internal abstract void PreInput();
        protected internal abstract void PostInput();
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
