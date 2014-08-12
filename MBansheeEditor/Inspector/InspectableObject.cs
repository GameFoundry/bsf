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

        private object oldPropertyValue;
        private bool isInitialized;

        public InspectableObject(string title, InspectableFieldLayout layout, SerializableProperty property)
            : base(title, layout, property)
        {
            
        }

        protected override bool IsModified()
        {
            if (!isInitialized)
                return true;

            object newPropertyValue = property.GetValue<object>();
            if (oldPropertyValue != newPropertyValue)
            {
                oldPropertyValue = newPropertyValue;

                return true;
            }

            return base.IsModified();
        }

        protected override void Update(int index)
        {
            base.Update(index);
            isInitialized = true;

            if (property.Type != SerializableProperty.FieldType.Object)
                return;

            layout.DestroyElements();

            if (property.GetValue<object>() == null)
            {
                GUILayoutX guiChildLayout = layout.AddLayoutX(index);

                guiChildLayout.AddElement(new GUILabel(title));
                guiChildLayout.AddElement(new GUILabel("Empty"));

                if (!property.IsValueType)
                {
                    GUIButton createBtn = new GUIButton("Create");
                    createBtn.OnClick += OnCreateButtonClicked;
                    guiChildLayout.AddElement(createBtn);
                }
            }
            else
            {
                GUILayoutX guiTitleLayout = layout.AddLayoutX(index);

                GUILabel guiLabel = new GUILabel(title); // TODO - Use foldout
                guiTitleLayout.AddElement(guiLabel);

                GUIButton clearBtn = new GUIButton("Clear");
                clearBtn.OnClick += OnClearButtonClicked;
                guiTitleLayout.AddElement(clearBtn);

                GUILayoutX guiChildLayout = layout.AddLayoutX(index);
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
