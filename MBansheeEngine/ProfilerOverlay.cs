using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace BansheeEngine
{
    // Note: Must be the same as C++ enum ProfilerOverlayType
	public enum ProfilerOverlayType
	{
		CPUSamples,
		GPUSamples
	};

    public class ProfilerOverlay : Component
    {
        private ProfilerOverlayInternal impl;

        public void SetType(ProfilerOverlayType type)
        {
            impl.SetType(type);   
        }

        private void OnReset()
        {
            if (impl != null)
                impl.Destroy();

            Camera cam = SceneObject.GetComponent<Camera>();
            impl = new ProfilerOverlayInternal(cam);
        }

        private void Update()
        {
            impl.Update();
        }

        private void OnDestroy()
        {
            impl.Destroy();
        }
    }
}
