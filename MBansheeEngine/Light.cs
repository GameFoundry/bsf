using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Text;

namespace BansheeEngine
{
    public class Light : Component
    {
        private LightInternal internalLight;

        [SerializeField]
        private SerializableData serializableData = new SerializableData();

        internal LightInternal Internal
        {
            get { return internalLight; }
        }

        public Vector3 Position
        {
            get { return internalLight.Position; }
            set { internalLight.Position = value; serializableData.position = value; }
        }

        public Quaternion Rotation
        {
            get { return internalLight.Rotation; }
            set { internalLight.Rotation = value; serializableData.rotation = value; }
        }

        public LightType Type
        {
            get { return internalLight.Type; }
            set { internalLight.Type = value; serializableData.type = value; }
        }

        public Color Color
        {
            get { return internalLight.Color; }
            set { internalLight.Color = value; serializableData.color = value; }
        }

        public float Range
        {
            get { return internalLight.Range; }
            set { internalLight.Range = value; serializableData.range = value; }
        }

        public float Intensity
        {
            get { return internalLight.Intensity; }
            set { internalLight.Intensity = value; serializableData.intensity = value; }
        }

        public Degree SpotAngle
        {
            get { return internalLight.SpotAngle; }
            set { internalLight.SpotAngle = value; serializableData.spotAngle = value; }
        }

        public Degree SpotFalloffAngle
        {
            get { return internalLight.SpotFalloffAngle; }
            set { internalLight.SpotFalloffAngle = value; serializableData.spotFalloffAngle = value; }
        }

        public bool CastsShadow
        {
            get { return internalLight.CastsShadow; }
            set { internalLight.CastsShadow = value; serializableData.castShadows = value; }
        }

        public Sphere Bounds
        {
            get { Internal.UpdateTransform(SceneObject); return Internal.Bounds; }
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
            if (internalLight != null)
                internalLight.OnDestroy();

            internalLight = new LightInternal(SceneObject);

            // Restore saved values after reset
            internalLight.Position = serializableData.position;
            internalLight.Rotation = serializableData.rotation;
            internalLight.Color = serializableData.color;
            internalLight.SpotAngle = serializableData.spotAngle;
            internalLight.SpotFalloffAngle = serializableData.spotFalloffAngle;
            internalLight.Range = serializableData.range;
            internalLight.Intensity = serializableData.intensity;
            internalLight.Type = serializableData.type;
            internalLight.CastsShadow = serializableData.castShadows;
        }

        private void Update()
        {
            
        }

        private void OnDestroy()
        {
            internalLight.OnDestroy();
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
