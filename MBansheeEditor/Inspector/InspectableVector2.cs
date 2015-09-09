using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using BansheeEngine;

namespace BansheeEditor
{
    public class InspectableVector2 : InspectableField
    {
        private Vector2 propertyValue;
        private GUIVector2Field guiField;
        private bool isInitialized;

        public InspectableVector2(string title, InspectableFieldLayout layout, SerializableProperty property)
            : base(title, layout, property)
        {

        }

        private void Initialize(int layoutIndex)
        {
            if (property.Type == SerializableProperty.FieldType.Vector2)
            {
                guiField = new GUIVector2Field(new GUIContent(title));
                guiField.OnChanged += OnFieldValueChanged;

                layout.AddElement(layoutIndex, guiField);
            }

            isInitialized = true;
        }

        protected override bool IsModified()
        {
            if (!isInitialized)
                return true;

            Vector2 newPropertyValue = property.GetValue<Vector2>();
            if (propertyValue != newPropertyValue)
                return true;

            return base.IsModified();
        }

        protected override void Update(int layoutIndex)
        {
            base.Update(layoutIndex);

            if (!isInitialized)
                Initialize(layoutIndex);

            propertyValue = property.GetValue<Vector2>();
            if (guiField != null)
            {
                if (guiField.HasInputFocus())
                    return;

                guiField.Value = propertyValue;
            }
        }

        private void OnFieldValueChanged(Vector2 newValue)
        {
            property.SetValue(newValue);
        }
    }
}
