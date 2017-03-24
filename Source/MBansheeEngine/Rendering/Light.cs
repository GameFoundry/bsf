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
    public sealed class Light : ManagedComponent
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
        /// Range at which the light contribution fades out to zero. Use <see cref="UseAutoAttenuation"/> to provide
        /// a radius automatically dependant on light intensity. The radius will cut-off light contribution and therefore
        /// manually set very small radius can end up being very physically incorrect.
        /// </summary>
        public float AttenuationRadius
        {
            get { return _nativeLight.AttenuationRadius; }
            set { _nativeLight.AttenuationRadius = value; serializableData.attenuationRadius = value; }
        }

        /// <summary>
        /// Radius of the light source. If non-zero then this light represents an area light, otherwise it is a punctual
        /// light. Area lights have different attenuation then punctual lights, and their appearance in specular reflections
        /// is realistic. Shape of the area light depends on light type:
        ///  - For directional light the shape is a disc projected on the hemisphere on the horizon. This parameter
        ///    represents angular radius (in degrees) of the disk and should be very small (think of how much space the Sun
        ///    takes on the sky - roughly 0.5 degrees).
        ///  - For radial light the shape is a sphere and the radius is the radius of the sphere.
        ///  - For spot lights the shape is a disc oriented in the direction of the spot light and the radius is the radius
        ///    of the disc.
        /// </summary>
        public float SourceRadius
        {
            get { return _nativeLight.SourceRadius; }
            set { _nativeLight.SourceRadius = value; serializableData.sourceRadius = value; }
        }

        /// <summary>
        /// Determines the power of the light source.This will be luminous flux for radial & spot lights, luminance for
        /// directional lights with no area, and illuminance for directional lights with area(non-zero source radius).
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
        /// If enabled the <see cref="AttenuationRadius"/> property will automatically be controlled in order to provide
        /// reasonable light radius, depending on its intensity.
        /// </summary>
        public bool UseAutoAttenuation
        {
            get { return _nativeLight.UseAutoAttenuation; }
            set { _nativeLight.UseAutoAttenuation = value; serializableData.useAutoAttenuation = value; }
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
            _nativeLight.AttenuationRadius = serializableData.attenuationRadius;
            _nativeLight.SourceRadius = serializableData.sourceRadius;
            _nativeLight.Intensity = serializableData.intensity;
            _nativeLight.Type = serializableData.type;
            _nativeLight.CastsShadow = serializableData.castShadows;
            _nativeLight.UseAutoAttenuation = serializableData.useAutoAttenuation;
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
            public float attenuationRadius = 10.0f;
            public float sourceRadius = 0.0f;
            public float intensity = 5.0f;
            public LightType type = LightType.Radial;
            public bool castShadows = false;
            public bool useAutoAttenuation = true;
        }
    }

    /** @} */
}
