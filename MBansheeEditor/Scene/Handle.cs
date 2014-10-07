using System.Collections.Generic;
using BansheeEngine;

namespace BansheeEditor
{
    public abstract class Handle
    {
        private List<HandleSlider> sliders = new List<HandleSlider>();

        protected abstract void PreInput();
        protected abstract void PostInput();
        protected abstract void Draw();

        internal void RegisterSlider(HandleSlider slider)
        {
            sliders.Add(slider);
        }

        public void Destroy()
        {
            foreach (var slider in sliders)
                slider.Destroy();
        }
    }
}
