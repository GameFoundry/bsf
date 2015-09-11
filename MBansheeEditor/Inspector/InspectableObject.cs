using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using BansheeEngine;

namespace BansheeEditor
{
    public class InspectableObject : InspectableField
    {
        private const int IndentAmount = 5;

        private object propertyValue;

        private GUILayoutX guiChildLayout;
        private GUILayoutX guiTitleLayout;
        private bool isExpanded;
        private bool forceUpdate = true;

        public InspectableObject(string title, int depth, InspectableFieldLayout layout, SerializableProperty property)
            : base(title, depth, layout, property)
        {
            
        }

        public override GUILayoutX GetTitleLayout()
        {
            return guiTitleLayout;
        }

        protected override bool IsModified()
        {
            if (forceUpdate)
                return true;

            object newPropertyValue = property.GetValue<object>();
            if (propertyValue == null)
                return newPropertyValue != null;
            
            if (newPropertyValue == null)
                return propertyValue != null;
            
            return base.IsModified();
        }

        protected override void Update(int index)
        {
            base.Update(index);
            forceUpdate = false;
            guiTitleLayout = null;

            if (property.Type != SerializableProperty.FieldType.Object)
                return;

            layout.DestroyElements();

            propertyValue = property.GetValue<object>();
            if (propertyValue == null)
            {
                guiChildLayout = null;
                guiTitleLayout = layout.AddLayoutX(index);

                guiTitleLayout.AddElement(new GUILabel(title));
                guiTitleLayout.AddElement(new GUILabel("Empty", GUIOption.FixedWidth(100)));

                if (!property.IsValueType)
                {
                    GUIButton createBtn = new GUIButton("Cr", GUIOption.FixedWidth(20));
                    createBtn.OnClick += OnCreateButtonClicked;
                    guiTitleLayout.AddElement(createBtn);
                }
            }
            else
            {
                guiTitleLayout = layout.AddLayoutX(index);

                GUIToggle guiFoldout = new GUIToggle(title, EditorStyles.Foldout);
                guiFoldout.Value = isExpanded;
                guiFoldout.OnToggled += OnFoldoutToggled;
                guiTitleLayout.AddElement(guiFoldout);

                GUIButton clearBtn = new GUIButton("Cl", GUIOption.FixedWidth(20));
                clearBtn.OnClick += OnClearButtonClicked;
                guiTitleLayout.AddElement(clearBtn);

                if (isExpanded)
                {
                    guiChildLayout = layout.AddLayoutX(index);
                    guiChildLayout.AddSpace(IndentAmount);

                    GUIPanel guiContentPanel = guiChildLayout.AddPanel();
                    GUILayoutX guiIndentLayoutX = guiContentPanel.AddLayoutX();
                    guiIndentLayoutX.AddSpace(IndentAmount);
                    GUILayoutY guiIndentLayoutY = guiIndentLayoutX.AddLayoutY();
                    guiIndentLayoutY.AddSpace(IndentAmount);
                    GUILayoutY guiContentLayout = guiIndentLayoutY.AddLayoutY();
                    guiIndentLayoutY.AddSpace(IndentAmount);
                    guiIndentLayoutX.AddSpace(IndentAmount);
                    guiChildLayout.AddSpace(IndentAmount);

                    short backgroundDepth = (short) (Inspector.START_BACKGROUND_DEPTH - depth - 1);
                    string bgPanelStyle = depth % 2 == 0 ? EditorStyles.InspectorContentBgAlternate : EditorStyles.InspectorContentBg;
                    GUIPanel backgroundPanel = guiContentPanel.AddPanel(backgroundDepth);
                    GUITexture inspectorContentBg = new GUITexture(null, bgPanelStyle);
                    backgroundPanel.AddElement(inspectorContentBg);

                    SerializableObject serializableObject = property.GetObject();
                    foreach (var field in serializableObject.Fields)
                    {
                        if (!field.Inspectable)
                            continue;

                        AddChild(CreateInspectable(field.Name, depth + 1, new InspectableFieldLayout(guiContentLayout), field.GetProperty()));
                    }
                }
                else
                    guiChildLayout = null;
            }
        }

        private void OnFoldoutToggled(bool expanded)
        {
            isExpanded = expanded;
            forceUpdate = true;
        }

        private void OnCreateButtonClicked()
        {
            property.SetValue(property.CreateObjectInstance<object>());
        }

        private void OnClearButtonClicked()
        {
            property.SetValue<object>(null);
        }
    }
}
