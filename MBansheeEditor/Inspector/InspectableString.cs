using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using BansheeEngine;

namespace BansheeEditor
{
    public class InspectableString : InspectableObjectBase
    {
        private string propertyValue;
        private GUITextField guiField;
        private bool isInitialized;

        public InspectableString(string title, InspectableFieldLayout layout, SerializableProperty property)
            : base(title, layout, property)
        {

        }

        private void Initialize(int layoutIndex)
        {
            if (property.Type == SerializableProperty.FieldType.String)
            {
                guiField = new GUITextField(new GUIContent(title));
                guiField.OnChanged += OnFieldValueChanged;

                layout.AddElement(layoutIndex, guiField);
            }

            isInitialized = true;
        }

        protected override bool IsModified()
        {
            if (!isInitialized)
                return true;

            string newPropertyValue = property.GetValue<string>();
            if (propertyValue != newPropertyValue)
                return true;

            return base.IsModified();
        }

        protected override void Update(int layoutIndex)
        {
            base.Update(layoutIndex);

            if (!isInitialized)
                Initialize(layoutIndex);

            // TODO - Skip update if it currently has input focus so user can modify the value in peace

            propertyValue = property.GetValue<string>();
            if (guiField != null)
                guiField.Value = propertyValue;
        }

        private void OnFieldValueChanged(string newValue)
        {
            property.SetValue(newValue);
        }
    }
}
