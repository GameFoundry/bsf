using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using BansheeEngine;

namespace BansheeEditor
{
    public class InspectableObject : InspectableObjectBase
    {
        private const int IndentAmount = 15;

        private object propertyValue;

        private GUILayoutX guiChildLayout;
        private bool isExpanded;
        private bool forceUpdate = true;

        public InspectableObject(string title, InspectableFieldLayout layout, SerializableProperty property)
            : base(title, layout, property)
        {
            
        }

        protected override bool IsModified()
        {
            if (forceUpdate)
                return true;

            object newPropertyValue = property.GetValue<object>();
            if (!propertyValue.Equals(newPropertyValue))
                return true;

            return base.IsModified();
        }

        protected override void Update(int index)
        {
            base.Update(index);
            forceUpdate = false;

            if (property.Type != SerializableProperty.FieldType.Object)
                return;

            layout.DestroyElements();

            propertyValue = property.GetValue<object>();
            if (propertyValue == null)
            {
                guiChildLayout = null;
                GUILayoutX guiTitleLayout = layout.AddLayoutX(index);

                guiTitleLayout.AddElement(new GUILabel(title));
                guiTitleLayout.AddElement(new GUILabel("Empty"));

                if (!property.IsValueType)
                {
                    GUIButton createBtn = new GUIButton("Create");
                    createBtn.OnClick += OnCreateButtonClicked;
                    guiTitleLayout.AddElement(createBtn);
                }
            }
            else
            {
                GUILayoutX guiTitleLayout = layout.AddLayoutX(index);

                GUIFoldout guiFoldout = new GUIFoldout(title);
                guiFoldout.SetExpanded(isExpanded);
                guiFoldout.OnToggled += OnFoldoutToggled;
                guiTitleLayout.AddElement(guiFoldout);

                GUIButton clearBtn = new GUIButton("Clear");
                clearBtn.OnClick += OnClearButtonClicked;
                guiTitleLayout.AddElement(clearBtn);

                if (isExpanded)
                {
                    guiChildLayout = layout.AddLayoutX(index);
                    guiChildLayout.AddSpace(IndentAmount);

                    GUILayoutY guiContentLayout = guiChildLayout.AddLayoutY();

                    SerializableObject serializableObject = property.GetObject();
                    foreach (var field in serializableObject.fields)
                    {
                        if (!field.Inspectable)
                            continue;

                        if (field.HasCustomInspector)
                            AddChild(CreateCustomInspectable(field.CustomInspectorType, field.Name, new InspectableFieldLayout(guiContentLayout), field.GetProperty()));
                        else
                            AddChild(CreateDefaultInspectable(field.Name, new InspectableFieldLayout(guiContentLayout), field.GetProperty()));
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
