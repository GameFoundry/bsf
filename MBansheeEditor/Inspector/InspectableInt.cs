using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using BansheeEngine;

namespace BansheeEditor
{
    public class InspectableInt : InspectableObjectBase
    {
        private int propertyValue;
        private GUIIntField guiIntField;
        private bool isInitialized;

        public InspectableInt(string title, InspectableFieldLayout layout, SerializableProperty property)
            : base(title, layout, property)
        {

        }

        private void Initialize(int layoutIndex)
        {
            if (property.Type == SerializableProperty.FieldType.Int)
            {
                guiIntField = new GUIIntField(new GUIContent(title));
                guiIntField.OnChanged += OnFieldValueChanged;

                layout.AddElement(layoutIndex, guiIntField);
            }

            isInitialized = true;
        }

        protected override bool IsModified()
        {
            if (!isInitialized)
                return true;

            int newPropertyValue = property.GetValue<int>();
            if (propertyValue != newPropertyValue)
                return true;

            return base.IsModified();
        }

        protected override void Update(int layoutIndex)
        {
            base.Update(layoutIndex);

            if (!isInitialized)
                Initialize(layoutIndex);

            propertyValue = property.GetValue<int>();
            if (guiIntField != null)
            {
                if (guiIntField.HasInputFocus())
                    return;

                guiIntField.Value = propertyValue;
            }
        }

        private void OnFieldValueChanged(int newValue)
        {
            property.SetValue(newValue);
        }
    }
}
