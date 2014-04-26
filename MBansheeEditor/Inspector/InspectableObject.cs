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
        private GUILayout guiContentLayout;
        private bool isInitialized;

        public InspectableObject(string title, SerializableProperty property)
            : base(title, property)
        {
            
        }

        protected void Initialize(GUILayout layout)
        {
            if (property.Type != SerializableProperty.FieldType.Object)
                return;

            guiLabel = new GUILabel(title);
            layout.AddElement(guiLabel); // TODO - Add foldout and hook up its callbacks

            guiChildLayout = layout.AddLayoutX();
            guiChildLayout.AddSpace(IndentAmount);

            guiContentLayout = guiChildLayout.AddLayoutY();

            SerializableObject serializableObject = property.GetObject();

            foreach (var field in serializableObject.fields)
            {
                if (!field.Inspectable)
                    continue;

                if (field.HasCustomInspector)
                    AddChild(CreateCustomInspectable(field.CustomInspectorType, field.Name, field.GetProperty()));
                else
                    AddChild(CreateDefaultInspectable(field.Name, field.GetProperty()));
            }

            isInitialized = true;
        }

        protected override bool IsModified()
        {
            object newPropertyValue = property.GetValue<object>();
            if (oldPropertyValue != newPropertyValue)
            {
                oldPropertyValue = newPropertyValue;

                return true;
            }

            return base.IsModified();
        }

        protected override void Update(GUILayout layout)
        {
            base.Update(layout);

            if (!isInitialized)
                Initialize(layout);

            // TODO - Update GUI element(s) with value from property
        }

        public override void Destroy()
        {
            if (guiLabel != null)
                guiLabel.Destroy();

            if (guiContentLayout != null)
                guiContentLayout.Destroy();

            if (guiChildLayout != null)
                guiChildLayout.Destroy();

            base.Destroy();
        }
    }
}
