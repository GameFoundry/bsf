using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using BansheeEngine;

namespace BansheeEditor
{
    public class InspectableVector2 : InspectableObjectBase
    {
        private Vector2 oldPropertyValue;
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
            if (oldPropertyValue != newPropertyValue)
            {
                oldPropertyValue = newPropertyValue;

                return true;
            }

            return base.IsModified();
        }

        protected override void Update(int layoutIndex)
        {
            base.Update(layoutIndex);

            if (!isInitialized)
                Initialize(layoutIndex);

            // TODO - Skip update if it currently has input focus so user can modify the value in peace

            if (guiField != null)
                guiField.Value = property.GetValue<Vector2>();
        }

        private void OnFieldValueChanged(Vector2 newValue)
        {
            property.SetValue(newValue);
        }
    }
}
