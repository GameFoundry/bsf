//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
namespace BansheeEngine
{
    /** @addtogroup Rendering
     *  @{
     */

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
        /// Maximum range of the light. Light will not affect any geometry past that point. Range is automatically 
        /// calculated from intensity if <see cref="PhysicallyBasedAttenuation"/> is turned on.
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
        /// Determines is the light attenuation handled in a physically correct way, or should the user have more artistic
        /// control over it. If true the range and attenuation of the light are controlled by inverse square of distance. 
        /// If false then the user is allowed to set the range and attenuation is adjusted accordingly. 
        /// </summary>
        public bool PhysicallyBasedAttenuation
        {
            get { return _nativeLight.PhysicallyBasedAttenuation; }
            set { _nativeLight.PhysicallyBasedAttenuation = value; serializableData.physicallyBasedAttenuation = value; }
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
            _nativeLight.PhysicallyBasedAttenuation = serializableData.physicallyBasedAttenuation;
        }

        private void OnUpdate()
        {
            _nativeLight.UpdateTransform(SceneObject);
        }

        private void OnDestroy()
        {
            _nativeLight.OnDestroy();
        }

        /// <inheritdoc/>
        protected internal override bool CalculateBounds(out AABox box, out Sphere sphere)
        {
            sphere = Bounds;

            Vector3 extents = new Vector3(sphere.Radius, sphere.Radius, sphere.Radius);
            box = new AABox(sphere.Center - extents, sphere.Center + extents);

            return true;
        }

        /// <summary>
        /// Holds all data the light component needs to persist through serialization.
        /// </summary>
        [SerializeObject]
        private class SerializableData
        {
            public Color color = Color.White;
            public Degree spotAngle = new Degree(45);
            public Degree spotFalloffAngle = new Degree(40);
            public float range = 10.0f;
            public float intensity = 5.0f;
            public LightType type = LightType.Point;
            public bool castShadows = false;
            public bool physicallyBasedAttenuation = true;
        }
    }

    /** @} */
}
