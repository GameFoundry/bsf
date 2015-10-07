using System.Collections;
using System.Collections.Generic;
using BansheeEngine;

namespace BansheeEditor
{
    /// <summary>
    /// Renders an inspector for the <see cref="Renderable"/> component.
    /// </summary>
    [CustomInspector(typeof (Renderable))]
    internal class RenderableInspector : Inspector
    {
        private GUIResourceField meshField;
        private GUIListBoxField layersField;
        private GUIArrayField<Material> materialsField = new GUIArrayField<Material>();
        private List<MaterialParamGUI[]> materialParams = new List<MaterialParamGUI[]>();

        private ulong layersValue = 0;
        private Material[] materials;

        /// <inheritdoc/>
        protected internal override void Initialize()
        {
            BuildGUI();
        }

        /// <inheritdoc/>
        protected internal override void Refresh()
        {
            Renderable renderable = InspectedObject as Renderable;
            if (renderable == null)
                return;

            bool rebuildGUI = false;

            Material[] newMaterials = renderable.Materials;
            if (newMaterials == null)
                rebuildGUI = materials != null;
            else
            {
                if (materials == null)
                    rebuildGUI = true;
                else
                {
                    if (materials.Length != newMaterials.Length)
                        rebuildGUI = true;
                    else
                    {
                        for (int i = 0; i < materials.Length; i++)
                        {
                            if (materials[i] != newMaterials[i])
                            {
                                rebuildGUI = true;
                                break;
                            }
                        }
                    }
                }
            }

            if (rebuildGUI)
                BuildGUI();

            meshField.Value = renderable.Mesh;

            if (layersValue != renderable.Layers)
            {
                bool[] states = new bool[64];
                for (int i = 0; i < states.Length; i++)
                    states[i] = (renderable.Layers & Layers.Values[i]) == Layers.Values[i];

                layersField.States = states;
                layersValue = renderable.Layers;
            }

            if (materials != null)
            {
                for (int i = 0; i < materialParams.Count; i++)
                {
                    if (materialParams[i] != null)
                    {
                        foreach (var param in materialParams[i])
                            param.Refresh(materials[i]);
                    }
                }
            }
        }

        /// <summary>
        /// Recreates all the GUI elements used by this inspector.
        /// </summary>
        private void BuildGUI()
        {
            Layout.Clear();

            Renderable renderable = InspectedObject as Renderable;
            if (renderable == null)
                return;

            meshField = new GUIResourceField(typeof(Mesh), new LocEdString("Mesh"));
            layersField = new GUIListBoxField(Layers.Names, true, new LocEdString("Layers"));

            Layout.AddElement(meshField);
            Layout.AddElement(layersField);

            layersValue = 0;
            materials = renderable.Materials;
            materialsField.BuildGUI<MaterialArrayRow>(new LocEdString("Materials"), materials, Layout);

            materialsField.OnChanged += x =>
            {
                renderable.Materials = (Material[])x;
                BuildGUI();
                Refresh();
            };
            meshField.OnChanged += x => renderable.Mesh = x as Mesh;
            layersField.OnSelectionChanged += x =>
            {
                ulong layers = 0;
                bool[] states = layersField.States;
                for (int i = 0; i < states.Length; i++)
                    layers |= states[i] ? Layers.Values[i] : 0;

                layersValue = layers;
                renderable.Layers = layers;
            };

            materialParams.Clear();
            if (materials != null)
            {
                for (int i = 0; i < materials.Length; i++)
                {
                    if (materials[i] == null)
                    {
                        materialParams.Add(new MaterialParamGUI[0]);
                        continue;
                    }

                    Layout.AddSpace(10);

                    MaterialParamGUI[] matParams = MaterialInspector.CreateMaterialGUI(materials[i], Layout);
                    materialParams.Add(matParams);
                }
            }
        }

        /// <summary>
        /// Row element used for displaying GUI for material array elements.
        /// </summary>
        public class MaterialArrayRow : GUIListFieldRow
        {
            private GUIResourceField materialField;

            /// <inheritdoc/>
            protected override GUILayoutX CreateGUI(GUILayoutY layout)
            {
                GUILayoutX titleLayout = layout.AddLayoutX();
                materialField = new GUIResourceField(typeof(Material), new LocEdString(seqIndex + ". "));
                titleLayout.AddElement(materialField);

                materialField.OnChanged += SetValue;

                return titleLayout;
            }

            /// <inheritdoc/>
            internal protected override bool Refresh()
            {
                materialField.Value = GetValue<Material>();
                return false;
            }
        }
    }
}