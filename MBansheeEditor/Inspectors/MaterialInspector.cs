using System.Collections.Generic;
using BansheeEngine;

namespace BansheeEditor
{
    /// <summary>
    /// Renders an inspector for the <see cref="Material"/> resource.
    /// </summary>
    [CustomInspector(typeof (Material))]
    internal class MaterialInspector : Inspector
    {
        private MaterialParamGUI[] guiParams;
        private GUIResourceField shaderField;

        /// <inheritdoc/>
        protected internal override void Initialize()
        {
            Material material = referencedObject as Material;
            if (material == null)
                return;

            shaderField = new GUIResourceField(typeof(Shader), new LocEdString("Shader"));
            shaderField.OnChanged += (x) =>
            {
                material.Shader = x as Shader;
                RebuildParamGUI(material);
            };

            layout.AddElement(shaderField);

            RebuildParamGUI(material);
        }

        /// <inheritdoc/>
        protected internal override bool Refresh()
        {
            Material material = referencedObject as Material;
            if (material == null)
                return false;

            bool anythingModified = false;

            if (material.Shader != shaderField.Value)
            {
                shaderField.Value = material.Shader;
                RebuildParamGUI(material);
                anythingModified = true;
            }

            if (guiParams != null)
            {
                foreach (var param in guiParams)
                    anythingModified |= param.Refresh(material);
            }

            return anythingModified;
        }

        /// <summary>
        /// Recreates GUI elements for all material parameters.
        /// </summary>
        /// <param name="material">Material to create parameters for</param>
        private void RebuildParamGUI(Material material)
        {
            if (guiParams != null)
            {
                foreach (var param in guiParams)
                    param.Destroy();

                guiParams = null;
            }

            if (material != null && material.Shader != null)
                guiParams = CreateMaterialGUI(material, layout);
        }


        /// <summary>
        /// Creates a set of objects in which each object represents a GUI for a material parameter.
        /// </summary>
        /// <param name="mat">Material for whose parameters to create GUI for.</param>
        /// <param name="layout">Layout to add the parameter GUI elements to.</param>
        /// <returns>A material parameter GUI object for each supported material parameter.</returns>
        static internal MaterialParamGUI[] CreateMaterialGUI(Material mat, GUILayout layout)
        {
            Shader shader = mat.Shader;
            if (shader == null)
                return new MaterialParamGUI[0];

            List<MaterialParamGUI> guiParams = new List<MaterialParamGUI>();
            ShaderParameter[] shaderParams = shader.Parameters;

            foreach (var param in shaderParams)
            {
                switch (param.type)
                {
                    case ShaderParameterType.Float:
                        guiParams.Add(new MaterialParamFloatGUI(param, mat, layout));
                        break;
                    case ShaderParameterType.Vector2:
                        guiParams.Add(new MaterialParamVec2GUI(param, mat, layout));
                        break;
                    case ShaderParameterType.Vector3:
                        guiParams.Add(new MaterialParamVec3GUI(param, mat, layout));
                        break;
                    case ShaderParameterType.Vector4:
                        guiParams.Add(new MaterialParamVec4GUI(param, mat, layout));
                        break;
                    case ShaderParameterType.Matrix3:
                        guiParams.Add(new MaterialParamMat3GUI(param, mat, layout));
                        break;
                    case ShaderParameterType.Matrix4:
                        guiParams.Add(new MaterialParamMat4GUI(param, mat, layout));
                        break;
                    case ShaderParameterType.Color:
                        guiParams.Add(new MaterialParamColorGUI(param, mat, layout));
                        break;
                    case ShaderParameterType.Texture2D:
                    case ShaderParameterType.Texture3D:
                    case ShaderParameterType.TextureCube:
                        guiParams.Add(new MaterialParamTextureGUI(param, mat, layout));
                        break;
                }
            }

            return guiParams.ToArray();
        }
    }

    /// <summary>
    /// Contains GUI element(s) for a single parameter in a <see cref="Material"/>.
    /// </summary>
    internal abstract class MaterialParamGUI
    {
        protected ShaderParameter shaderParam;

        /// <summary>
        /// Creates a new material parameter GUI.
        /// </summary>
        /// <param name="shaderParam">Shader parameter to create the GUI for.</param>
        protected MaterialParamGUI(ShaderParameter shaderParam)
        {
            this.shaderParam = shaderParam;
        }

        /// <summary>
        /// Checks if the data stored in GUI and in the material matches, and updates the GUI if it doesn't.
        /// </summary>
        /// <param name="material">Material whose data to check.</param>
        /// <returns>True if anything was modified, false otherwise.</returns>
        internal abstract bool Refresh(Material material);

        /// <summary> 
        /// Destroys the internal GUI elements.
        /// </summary>
        internal abstract void Destroy();
    }

    /// <summary>
    /// Contains GUI element(s) for a single floating point parameter in a <see cref="Material"/>.
    /// </summary>
    internal class MaterialParamFloatGUI : MaterialParamGUI
    {
        private GUIFloatField guiElem;

        /// <summary>
        /// Creates a new material parameter GUI.
        /// </summary>
        /// <param name="shaderParam">Shader parameter to create the GUI for. Must be of floating point type.</param>
        /// <param name="material">Material the parameter is a part of.</param>
        /// <param name="layout">Layout to append the GUI elements to.</param>
        internal MaterialParamFloatGUI(ShaderParameter shaderParam, Material material, GUILayout layout)
            : base(shaderParam)
        {
            LocString title = new LocEdString(shaderParam.name);
            guiElem = new GUIFloatField(title);
            guiElem.OnChanged += (x) =>
            {
                material.SetFloat(shaderParam.name, x);
                EditorApplication.SetDirty(material);
            };

            layout.AddElement(guiElem);
        }

        /// <inheritdoc/>
        internal override bool Refresh(Material material)
        {
            float value = material.GetFloat(shaderParam.name);
            if (value != guiElem.Value)
            {
                guiElem.Value = value;
                return true;
            }

            return false;
        }

        /// <inheritdoc/>
        internal override void Destroy()
        {
            guiElem.Destroy();
        }
    }

    /// <summary>
    /// Contains GUI element(s) for a single 2D vector parameter in a <see cref="Material"/>.
    /// </summary>
    internal class MaterialParamVec2GUI : MaterialParamGUI
    {
        private GUIVector2Field guiElem;

        /// <summary>
        /// Creates a new material parameter GUI.
        /// </summary>
        /// <param name="shaderParam">Shader parameter to create the GUI for. Must be of 2D vector type.</param>
        /// <param name="material">Material the parameter is a part of.</param>
        /// <param name="layout">Layout to append the GUI elements to.</param>
        internal MaterialParamVec2GUI(ShaderParameter shaderParam, Material material, GUILayout layout)
            : base(shaderParam)
        {
            LocString title = new LocEdString(shaderParam.name);
            guiElem = new GUIVector2Field(title);
            guiElem.OnChanged += (x) =>
            {
                material.SetVector2(shaderParam.name, x);
                EditorApplication.SetDirty(material);
            };

            layout.AddElement(guiElem);
        }

        /// <inheritdoc/>
        internal override bool Refresh(Material material)
        {
            Vector2 value = material.GetVector2(shaderParam.name);
            if (value != guiElem.Value)
            {
                guiElem.Value = value;
                return true;
            }

            return false;
        }

        /// <inheritdoc/>
        internal override void Destroy()
        {
            guiElem.Destroy();
        }
    }

    /// <summary>
    /// Contains GUI element(s) for a single 3D vector parameter in a <see cref="Material"/>.
    /// </summary>
    internal class MaterialParamVec3GUI : MaterialParamGUI
    {
        private GUIVector3Field guiElem;

        /// <summary>
        /// Creates a new material parameter GUI.
        /// </summary>
        /// <param name="shaderParam">Shader parameter to create the GUI for. Must be of 3D vector type.</param>
        /// <param name="material">Material the parameter is a part of.</param>
        /// <param name="layout">Layout to append the GUI elements to.</param>
        internal MaterialParamVec3GUI(ShaderParameter shaderParam, Material material, GUILayout layout)
            : base(shaderParam)
        {
            LocString title = new LocEdString(shaderParam.name);
            guiElem = new GUIVector3Field(title);
            guiElem.OnChanged += (x) =>
            {
                material.SetVector3(shaderParam.name, x);
                EditorApplication.SetDirty(material);
            };

            layout.AddElement(guiElem);
        }

        /// <inheritdoc/>
        internal override bool Refresh(Material material)
        {
            Vector3 value = material.GetVector3(shaderParam.name);
            if (value != guiElem.Value)
            {
                guiElem.Value = value;
                return true;
            }

            return false;
        }

        /// <inheritdoc/>
        internal override void Destroy()
        {
            guiElem.Destroy();
        }
    }

    /// <summary>
    /// Contains GUI element(s) for a single 4D vector parameter in a <see cref="Material"/>.
    /// </summary>
    internal class MaterialParamVec4GUI : MaterialParamGUI
    {
        private GUIVector4Field guiElem;

        /// <summary>
        /// Creates a new material parameter GUI.
        /// </summary>
        /// <param name="shaderParam">Shader parameter to create the GUI for. Must be of 4D vector type.</param>
        /// <param name="material">Material the parameter is a part of.</param>
        /// <param name="layout">Layout to append the GUI elements to.</param>
        internal MaterialParamVec4GUI(ShaderParameter shaderParam, Material material, GUILayout layout)
            : base(shaderParam)
        {
            LocString title = new LocEdString(shaderParam.name);
            guiElem = new GUIVector4Field(title);
            guiElem.OnChanged += (x) =>
            {
                material.SetVector4(shaderParam.name, x);
                EditorApplication.SetDirty(material);
            };

            layout.AddElement(guiElem);
        }

        /// <inheritdoc/>
        internal override bool Refresh(Material material)
        {
            Vector4 value = material.GetVector4(shaderParam.name);
            if (value != guiElem.Value)
            {
                guiElem.Value = value;
                return true;
            }

            return false;
        }

        /// <inheritdoc/>
        internal override void Destroy()
        {
            guiElem.Destroy();
        }
    }

    /// <summary>
    /// Contains GUI element(s) for a single 3x3 matrix parameter in a <see cref="Material"/>.
    /// </summary>
    internal class MaterialParamMat3GUI : MaterialParamGUI
    {
        private const int MAT_SIZE = 3;

        private GUIFloatField[] guiMatFields = new GUIFloatField[MAT_SIZE * MAT_SIZE];

        /// <summary>
        /// Creates a new material parameter GUI.
        /// </summary>
        /// <param name="shaderParam">Shader parameter to create the GUI for. Must be of 3x3 matrix type.</param>
        /// <param name="material">Material the parameter is a part of.</param>
        /// <param name="layout">Layout to append the GUI elements to.</param>
        internal MaterialParamMat3GUI(ShaderParameter shaderParam, Material material, GUILayout layout)
            : base(shaderParam)
        {
            LocString title = new LocEdString(shaderParam.name);
            GUILabel guiTitle = new GUILabel(title, GUIOption.FixedWidth(100));

            GUILayout mainLayout = layout.AddLayoutX();
            mainLayout.AddElement(guiTitle);

            GUILayoutY contentLayout = mainLayout.AddLayoutY();

            GUILayoutX[] rows = new GUILayoutX[MAT_SIZE];
            for (int i = 0; i < MAT_SIZE; i++)
                rows[i] = contentLayout.AddLayoutX();

            for (int row = 0; row < MAT_SIZE; row++)
            {
                for (int col = 0; col < MAT_SIZE; col++)
                {
                    int index = row * MAT_SIZE + col;
                    guiMatFields[index] = new GUIFloatField(row + ", " + col, 30);

                    GUIFloatField field = guiMatFields[index];
                    rows[row].AddElement(field);

                    int hoistedRow = row;
                    int hoistedCol = col;
                    field.OnChanged += (x) =>
                    {
                        Matrix3 value = material.GetMatrix3(shaderParam.name);
                        value[hoistedRow, hoistedCol] = x;
                        material.SetMatrix3(shaderParam.name, value);
                        EditorApplication.SetDirty(material);
                    };
                }
            }
        }

        /// <inheritdoc/>
        internal override bool Refresh(Material material)
        {
            Matrix3 value = material.GetMatrix3(shaderParam.name);

            bool wasModified = false;
            for (int row = 0; row < MAT_SIZE; row++)
            {
                for (int col = 0; col < MAT_SIZE; col++)
                {
                    int index = row * MAT_SIZE + col;
                    if (value[row, col] != guiMatFields[index].Value)
                    {
                        guiMatFields[index].Value = value[row, col];
                        wasModified = true;
                    }
                }
            }

            return wasModified;
        }

        /// <inheritdoc/>
        internal override void Destroy()
        {
            for (int row = 0; row < MAT_SIZE; row++)
            {
                for (int col = 0; col < MAT_SIZE; col++)
                {
                    int index = row*MAT_SIZE + col;
                    guiMatFields[index].Destroy();
                }
            }
        }
    }

    /// <summary>
    /// Contains GUI element(s) for a single 4x4 matrix parameter in a <see cref="Material"/>.
    /// </summary>
    internal class MaterialParamMat4GUI : MaterialParamGUI
    {
        private const int MAT_SIZE = 4;

        private GUIFloatField[] guiMatFields = new GUIFloatField[MAT_SIZE * MAT_SIZE];

        /// <summary>
        /// Creates a new material parameter GUI.
        /// </summary>
        /// <param name="shaderParam">Shader parameter to create the GUI for. Must be of 4x4 matrix type.</param>
        /// <param name="material">Material the parameter is a part of.</param>
        /// <param name="layout">Layout to append the GUI elements to.</param>
        internal MaterialParamMat4GUI(ShaderParameter shaderParam, Material material, GUILayout layout)
            : base(shaderParam)
        {
            LocString title = new LocEdString(shaderParam.name);
            GUILabel guiTitle = new GUILabel(title, GUIOption.FixedWidth(100));

            GUILayout mainLayout = layout.AddLayoutX();
            mainLayout.AddElement(guiTitle);

            GUILayoutY contentLayout = mainLayout.AddLayoutY();

            GUILayoutX[] rows = new GUILayoutX[MAT_SIZE];
            for (int i = 0; i < MAT_SIZE; i++)
                rows[i] = contentLayout.AddLayoutX();

            for (int row = 0; row < MAT_SIZE; row++)
            {
                for (int col = 0; col < MAT_SIZE; col++)
                {
                    int index = row * MAT_SIZE + col;
                    guiMatFields[index] = new GUIFloatField(row + ", " + col, 30);

                    GUIFloatField field = guiMatFields[index];
                    rows[row].AddElement(field);

                    int hoistedRow = row;
                    int hoistedCol = col;
                    field.OnChanged += (x) =>
                    {
                        Matrix4 value = material.GetMatrix4(shaderParam.name);
                        value[hoistedRow, hoistedCol] = x;
                        material.SetMatrix4(shaderParam.name, value);
                        EditorApplication.SetDirty(material);
                    };
                }
            }
        }

        /// <inheritdoc/>
        internal override bool Refresh(Material material)
        {
            Matrix4 value = material.GetMatrix4(shaderParam.name);

            bool wasModified = false;
            for (int row = 0; row < MAT_SIZE; row++)
            {
                for (int col = 0; col < MAT_SIZE; col++)
                {
                    int index = row * MAT_SIZE + col;
                    if (value[row, col] != guiMatFields[index].Value)
                    {
                        guiMatFields[index].Value = value[row, col];
                        wasModified = true;
                    }
                }
            }

            return wasModified;
        }

        /// <inheritdoc/>
        internal override void Destroy()
        {
            for (int row = 0; row < MAT_SIZE; row++)
            {
                for (int col = 0; col < MAT_SIZE; col++)
                {
                    int index = row * MAT_SIZE + col;
                    guiMatFields[index].Destroy();
                }
            }
        }
    }

    /// <summary>
    /// Contains GUI element(s) for a single color parameter in a <see cref="Material"/>.
    /// </summary>
    internal class MaterialParamColorGUI : MaterialParamGUI
    {
        private GUIColorField guiElem;

        /// <summary>
        /// Creates a new material parameter GUI.
        /// </summary>
        /// <param name="shaderParam">Shader parameter to create the GUI for. Must be of color type.</param>
        /// <param name="material">Material the parameter is a part of.</param>
        /// <param name="layout">Layout to append the GUI elements to.</param>
        internal MaterialParamColorGUI(ShaderParameter shaderParam, Material material, GUILayout layout)
            : base(shaderParam)
        {
            LocString title = new LocEdString(shaderParam.name);
            guiElem = new GUIColorField(title);
            guiElem.OnChanged += (x) =>
            {
                material.SetColor(shaderParam.name, x);
                EditorApplication.SetDirty(material);
            };

            layout.AddElement(guiElem);
        }

        /// <inheritdoc/>
        internal override bool Refresh(Material material)
        {
            Color value = material.GetColor(shaderParam.name);
            if (value != guiElem.Value)
            {
                guiElem.Value = value;
                return true;
            }

            return false;
        }

        /// <inheritdoc/>
        internal override void Destroy()
        {
            guiElem.Destroy();
        }
    }

    /// <summary>
    /// Contains GUI element(s) for a single texture parameter in a <see cref="Material"/>.
    /// </summary>
    internal class MaterialParamTextureGUI : MaterialParamGUI
    {
        private GUITextureField guiElem;

        /// <summary>
        /// Creates a new material parameter GUI.
        /// </summary>
        /// <param name="shaderParam">Shader parameter to create the GUI for. Must be of texture type.</param>
        /// <param name="material">Material the parameter is a part of.</param>
        /// <param name="layout">Layout to append the GUI elements to.</param>
        internal MaterialParamTextureGUI(ShaderParameter shaderParam, Material material, GUILayout layout)
            : base(shaderParam)
        {
            LocString title = new LocEdString(shaderParam.name);
            guiElem = new GUITextureField(title);

            switch (shaderParam.type)
            {
                case ShaderParameterType.Texture2D:
                    guiElem.OnChanged += (x) =>
                    {
                        material.SetTexture2D(shaderParam.name, x as Texture2D);
                        EditorApplication.SetDirty(material);
                    };
                    break;
                case ShaderParameterType.Texture3D:
                    guiElem.OnChanged += (x) =>
                    {
                        material.SetTexture3D(shaderParam.name, x as Texture3D);
                        EditorApplication.SetDirty(material);
                    };
                    break;
                case ShaderParameterType.TextureCube:
                    guiElem.OnChanged += (x) =>
                    {
                        material.SetTextureCube(shaderParam.name, x as TextureCube);
                        EditorApplication.SetDirty(material);
                    };
                    break;
            }

            layout.AddElement(guiElem);
        }

        /// <inheritdoc/>
        internal override bool Refresh(Material material)
        {
            Texture value = null;
            switch (shaderParam.type)
            {
                case ShaderParameterType.Texture2D:
                    value = material.GetTexture2D(shaderParam.name);
                    break;
                case ShaderParameterType.Texture3D:
                    value = material.GetTexture3D(shaderParam.name);
                    break;
                case ShaderParameterType.TextureCube:
                    value = material.GetTextureCube(shaderParam.name);
                    break;
            }

            if (value != guiElem.Value)
            {
                guiElem.Value = value;
                return true;
            }

            return false;
        }

        /// <inheritdoc/>
        internal override void Destroy()
        {
            guiElem.Destroy();
        }
    }
}
