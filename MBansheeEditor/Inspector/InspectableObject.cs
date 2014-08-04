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

        private GUILabel guiLabel;
        private GUILayout guiChildLayout;
        private GUILayoutY guiContentLayout;
        private bool isInitialized;

        public InspectableObject(string title, InspectableFieldLayout layout, SerializableProperty property)
            : base(title, layout, property)
        {
            
        }

        protected void Initialize(int layoutIndex)
        {
            if (property.Type != SerializableProperty.FieldType.Object)
                return;

            guiLabel = new GUILabel(title);
            layout.AddElement(layoutIndex, guiLabel);

            guiChildLayout = layout.AddLayoutX(layoutIndex);
            guiChildLayout.AddSpace(IndentAmount);

            guiContentLayout = guiChildLayout.AddLayoutY();

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

            isInitialized = true;
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

            if (!isInitialized)
                Initialize(index);

            // TODO - Update GUI element(s) with value from property
        }
    }
}
