using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Text;

namespace BansheeEngine
{
    /// <summary>
    /// Component that illuminates a portion of the scene covered by the light.
    /// </summary>
    [RunInEditor]
    public sealed class Light : Component
    {
        private NativeLight _nativeLight;

        [SerializeField]
        private SerializableData serializableData = new SerializableData();

        /// <summary>
        /// Returns the non-component version of Light that is wrapped by this component. 
        /// </summary>
        internal NativeLight Native
        {
            get { return _nativeLight; }
        }

        /// <summary>
        /// Light type that determines how are elements near it illuminated.
        /// </summary>
        public LightType Type
        {
            get { return _nativeLight.Type; }
            set { _nativeLight.Type = value; serializableData.type = value; }
        }

        /// <summary>
        /// Color emitted from the light.
        /// </summary>
        public Color Color
        {
            get { return _nativeLight.Color; }
            set { _nativeLight.Color = value; serializableData.color = value; }
        }

        /// <summary>
        /// Maximum range of the light. Light will not affect any geometry past that point.
        /// </summary>
        public float Range
        {
            get { return _nativeLight.Range; }
            set { _nativeLight.Range = value; serializableData.range = value; }
        }

        /// <summary>
        /// Power of the light source. This is luminous flux for point & spot lights, and radiance for directional lights.
        /// </summary>
        public float Intensity
        {
            get { return _nativeLight.Intensity; }
            set { _nativeLight.Intensity = value; serializableData.intensity = value; }
        }

        /// <summary>
        /// Total angle covered by a spot light. Ignored by other light types.
        /// </summary>
        public Degree SpotAngle
        {
            get { return _nativeLight.SpotAngle; }
            set { _nativeLight.SpotAngle = value; serializableData.spotAngle = value; }
        }

        /// <summary>
        /// Falloff angle covered by a spot light. Falloff angle determines at what point does light intensity starts 
        /// linearly falling off as the angle approaches the total spot angle. Ignored by other light types.
        /// </summary>
        public Degree SpotFalloffAngle
        {
            get { return _nativeLight.SpotFalloffAngle; }
            set { _nativeLight.SpotFalloffAngle = value; serializableData.spotFalloffAngle = value; }
        }

        /// <summary>
        /// Determines does this light cast a shadow when rendered.
        /// </summary>
        public bool CastsShadow
        {
            get { return _nativeLight.CastsShadow; }
            set { _nativeLight.CastsShadow = value; serializableData.castShadows = value; }
        }

        /// <summary>
        /// Returns world space bounds that completely encompass the light's area of influence.
        /// </summary>
        public Sphere Bounds
        {
            get { Native.UpdateTransform(SceneObject); return Native.Bounds; }
        }

        private void OnInitialize()
        {
            serializableData.color = Color.White;
            serializableData.spotAngle = new Degree(45);
            serializableData.spotFalloffAngle = new Degree(40);
            serializableData.range = 10.0f;
            serializableData.intensity = 100.0f;
            serializableData.type = LightType.Point;
            serializableData.castShadows = false;
        }

        private void OnReset()
        {
            if (_nativeLight != null)
                _nativeLight.OnDestroy();

            _nativeLight = new NativeLight(SceneObject);

            // Restore saved values after reset
            _nativeLight.Color = serializableData.color;
            _nativeLight.SpotAngle = serializableData.spotAngle;
            _nativeLight.SpotFalloffAngle = serializableData.spotFalloffAngle;
            _nativeLight.Range = serializableData.range;
            _nativeLight.Intensity = serializableData.intensity;
            _nativeLight.Type = serializableData.type;
            _nativeLight.CastsShadow = serializableData.castShadows;
        }

        private void Update()
        {
            
        }

        private void OnDestroy()
        {
            _nativeLight.OnDestroy();
        }

        /// <summary>
        /// Holds all data the light component needs to persist through serialization.
        /// </summary>
        [SerializeObject]
        private struct SerializableData
        {
            public Color color;
            public Degree spotAngle;
            public Degree spotFalloffAngle;
            public float range;
            public float intensity;
            public LightType type;
            public bool castShadows;
        }
    }
}
