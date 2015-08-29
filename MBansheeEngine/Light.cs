using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Text;

namespace BansheeEngine
{
    public class Light : Component
    {
        private NativeLight _nativeLight;

        [SerializeField]
        private SerializableData serializableData = new SerializableData();

        internal NativeLight Native
        {
            get { return _nativeLight; }
        }

        public Vector3 Position
        {
            get { return _nativeLight.Position; }
            set { _nativeLight.Position = value; serializableData.position = value; }
        }

        public Quaternion Rotation
        {
            get { return _nativeLight.Rotation; }
            set { _nativeLight.Rotation = value; serializableData.rotation = value; }
        }

        public LightType Type
        {
            get { return _nativeLight.Type; }
            set { _nativeLight.Type = value; serializableData.type = value; }
        }

        public Color Color
        {
            get { return _nativeLight.Color; }
            set { _nativeLight.Color = value; serializableData.color = value; }
        }

        public float Range
        {
            get { return _nativeLight.Range; }
            set { _nativeLight.Range = value; serializableData.range = value; }
        }

        public float Intensity
        {
            get { return _nativeLight.Intensity; }
            set { _nativeLight.Intensity = value; serializableData.intensity = value; }
        }

        public Degree SpotAngle
        {
            get { return _nativeLight.SpotAngle; }
            set { _nativeLight.SpotAngle = value; serializableData.spotAngle = value; }
        }

        public Degree SpotFalloffAngle
        {
            get { return _nativeLight.SpotFalloffAngle; }
            set { _nativeLight.SpotFalloffAngle = value; serializableData.spotFalloffAngle = value; }
        }

        public bool CastsShadow
        {
            get { return _nativeLight.CastsShadow; }
            set { _nativeLight.CastsShadow = value; serializableData.castShadows = value; }
        }

        public Sphere Bounds
        {
            get { Native.UpdateTransform(SceneObject); return Native.Bounds; }
        }

        private void OnInitialize()
        {
            serializableData.position = Vector3.Zero;
            serializableData.rotation = Quaternion.Identity;
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
            _nativeLight.Position = serializableData.position;
            _nativeLight.Rotation = serializableData.rotation;
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

        [SerializeObject]
        private struct SerializableData
        {
            public Vector3 position;
            public Quaternion rotation;
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
