using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using BansheeEngine;

namespace BansheeEditor
{
    public class InspectableResourceRef : InspectableObjectBase
    {
        private Resource propertyValue;
        private GUIResourceField guiField;
        private bool isInitialized;

        public InspectableResourceRef(string title, InspectableFieldLayout layout, SerializableProperty property)
            : base(title, layout, property)
        {

        }

        private void Initialize(int layoutIndex)
        {
            if (property.Type == SerializableProperty.FieldType.ResourceRef)
            {
                guiField = new GUIResourceField(property.InternalType, new GUIContent(title));
                guiField.OnChanged += OnFieldValueChanged;

                layout.AddElement(layoutIndex, guiField);
            }

            isInitialized = true;
        }

        protected override bool IsModified()
        {
            if (!isInitialized)
                return true;

            Resource newPropertyValue = property.GetValue<Resource>();
            if (propertyValue != newPropertyValue)
                return true;

            return base.IsModified();
        }

        protected override void Update(int layoutIndex)
        {
            base.Update(layoutIndex);

            if (!isInitialized)
                Initialize(layoutIndex);

            propertyValue = property.GetValue<Resource>();
            if (guiField != null)
                guiField.Value = propertyValue;
        }

        private void OnFieldValueChanged(Resource newValue)
        {
            property.SetValue(newValue);
        }
    }
}
