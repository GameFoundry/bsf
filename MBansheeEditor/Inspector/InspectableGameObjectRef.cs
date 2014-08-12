using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using BansheeEngine;

namespace BansheeEditor
{
    public class InspectableGameObjectRef : InspectableObjectBase
    {
        private GameObject oldPropertyValue;
        private GUIGameObjectField guiField;
        private bool isInitialized;

        public InspectableGameObjectRef(string title, InspectableFieldLayout layout, SerializableProperty property)
            : base(title, layout, property)
        {

        }

        private void Initialize(int layoutIndex)
        {
            if (property.Type == SerializableProperty.FieldType.GameObjectRef)
            {
                guiField = new GUIGameObjectField(property.InternalType, new GUIContent(title));
                guiField.OnChanged += OnFieldValueChanged;

                layout.AddElement(layoutIndex, guiField);
            }

            isInitialized = true;
        }

        protected override bool IsModified()
        {
            if (!isInitialized)
                return true;

            GameObject newPropertyValue = property.GetValue<GameObject>();
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

            if (guiField != null)
                guiField.Value = property.GetValue<GameObject>();
        }

        private void OnFieldValueChanged(GameObject newValue)
        {
            property.SetValue(newValue);
        }
    }
}
