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

        internal Vector3 Position
        {
            get { return internalLight.Position; }
            set { internalLight.Position = value; serializableData.position = value; }
        }

        internal Quaternion Rotation
        {
            get { return internalLight.Rotation; }
            set { internalLight.Rotation = value; serializableData.rotation = value; }
        }

        internal LightType Type
        {
            get { return internalLight.Type; }
            set { internalLight.Type = value; serializableData.type = value; }
        }

        internal Color Color
        {
            get { return internalLight.Color; }
            set { internalLight.Color = value; serializableData.color = value; }
        }

        internal float Range
        {
            get { return internalLight.Range; }
            set { internalLight.Range = value; serializableData.range = value; }
        }

        internal float LuminousFlux
        {
            get { return internalLight.LuminousFlux; }
            set { internalLight.LuminousFlux = value; serializableData.luminousFlux = value; }
        }

        internal Degree SpotAngle
        {
            get { return internalLight.SpotAngle; }
            set { internalLight.SpotAngle = value; serializableData.spotAngle = value; }
        }

        internal bool CastsShadow
        {
            get { return internalLight.CastsShadow; }
            set { internalLight.CastsShadow = value; serializableData.castShadows = value; }
        }

        private void OnInitialize()
        {
            serializableData.position = Vector3.zero;
            serializableData.rotation = Quaternion.identity;
            serializableData.color = Color.White;
            serializableData.spotAngle = new Degree(45);
            serializableData.range = 10.0f;
            serializableData.luminousFlux = 100.0f;
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
            internalLight.Range = serializableData.range;
            internalLight.LuminousFlux = serializableData.luminousFlux;
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
            public float range;
            public float luminousFlux;
            public LightType type;
            public bool castShadows;
        }
    }
}
