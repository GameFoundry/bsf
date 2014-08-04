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
        private int oldPropertyValue;
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

            if(guiIntField != null)
                guiIntField.Value = property.GetValue<int>();
        }

        private void OnFieldValueChanged(int newValue)
        {
            property.SetValue<int>(newValue);
        }
    }
}
