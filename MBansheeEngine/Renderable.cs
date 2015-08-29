using System;

namespace BansheeEngine
{
    public class Renderable : Component
    {
        private NativeRenderable _native;

        [SerializeField]
        private SerializableData serializableData = new SerializableData();

        internal NativeRenderable Native
        {
            get { return _native; }
        }

        public Mesh Mesh
        {
            get { return _native.Mesh; }
            set 
            { 
                _native.Mesh = value; 
                serializableData.mesh = value;

                int subMeshCount = 0;
                if (value != null)
                    subMeshCount = value.SubMeshCount;

                Material[] newMaterials = new Material[subMeshCount];
                int numToCopy = MathEx.Min(newMaterials.Length, serializableData.materials.Length);
                Array.Copy(serializableData.materials, newMaterials, numToCopy);
                serializableData.materials = newMaterials;
            }
        }

        public Material Material
        {
            get { return _native.GetMaterial(0); }
            set 
            { _native.SetMaterial(value); serializableData.materials[0] = value; }
        }

        public Material GetMaterial(int index = 0)
        {
            return _native.GetMaterial(index);
        }

        public void SetMaterial(Material material, int index = 0)
        {
            _native.SetMaterial(material, index);
            serializableData.materials[index] = material;
        }

        public UInt64 Layers
        {
            get { return _native.Layers; }
            set { _native.Layers = value; serializableData.layers = value; }
        }

        public Bounds Bounds
        {
            get { return _native.GetBounds(SceneObject); }
        }

        private void OnInitialize()
        {
            serializableData.materials = new Material[0];
            serializableData.layers = 1;
        }

        private void OnReset()
        {
            if (_native != null)
                _native.OnDestroy();

            _native = new NativeRenderable(SceneObject);

            // Restore saved values after reset
            _native.Mesh = serializableData.mesh;

            if (serializableData.materials != null)
            {
                for (int i = 0; i < serializableData.materials.Length; i++)
                    _native.SetMaterial(serializableData.materials[i], i);
            }

            _native.Layers = serializableData.layers;
        }

        private void Update()
        {
            _native.UpdateTransform(SceneObject);
        }

        private void OnDestroy()
        {
            _native.OnDestroy();
        }

        [SerializeObject]
        private struct SerializableData
        {
            public Mesh mesh;
            public Material[] materials;
            public UInt64 layers;
        }
    }
}
