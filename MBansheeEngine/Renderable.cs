using System;

namespace BansheeEngine
{
    public class Renderable : Component
    {
        private RenderableHandler handler;

        [SerializeField]
        private SerializableData serializableData = new SerializableData();

        internal RenderableHandler Handler
        {
            get { return handler; }
        }

        public Mesh Mesh
        {
            get { return handler.Mesh; }
            set 
            { 
                handler.Mesh = value; 
                serializableData.mesh = value;

                Material[] newMaterials = new Material[value.SubMeshCount];
                int numToCopy = MathEx.Min(newMaterials.Length, serializableData.materials.Length);
                Array.Copy(serializableData.materials, newMaterials, numToCopy);
                serializableData.materials = newMaterials;
            }
        }

        public Material Material
        {
            get { return handler.GetMaterial(0); }
            set 
            { handler.SetMaterial(value); serializableData.materials[0] = value; }
        }

        public Material GetMaterial(int index = 0)
        {
            return handler.GetMaterial(index);
        }

        public void SetMaterial(Material material, int index = 0)
        {
            handler.SetMaterial(material, index);
            serializableData.materials[index] = material;
        }

        public UInt64 Layers
        {
            get { return handler.Layers; }
            set { handler.Layers = value; serializableData.layers = value; }
        }

        public Bounds Bounds
        {
            get { return handler.GetBounds(sceneObject); }
        }

        private void OnInitialize()
        {
            serializableData.materials = new Material[0];
            serializableData.layers = 0xFFFFFFFFFFFFFFFF;
        }

        private void OnReset()
        {
            if (handler != null)
                handler.OnDestroy();

            handler = new RenderableHandler(sceneObject);

            // Restore saved values after reset
            handler.Mesh = serializableData.mesh;

            if (serializableData.materials != null)
            {
                for (int i = 0; i < serializableData.materials.Length; i++)
                    handler.SetMaterial(serializableData.materials[i], i);
            }

            handler.Layers = serializableData.layers;
        }

        private void Update()
        {
            handler.UpdateTransform(sceneObject);
        }

        private void OnDestroy()
        {
            handler.OnDestroy();
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
