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
        private bool isInitialized = false;

        public InspectableInt(string title, SerializableProperty property)
            :base(title, property)
        {

        }

        protected void Initialize(GUILayout layout)
        {
            if(property.Type == SerializableProperty.FieldType.Int)
            {
                guiIntField = new GUIIntField(new GUIContent(title));
                guiIntField.OnChanged += OnFieldValueChanged;
                layout.AddElement(guiIntField); 
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

        protected override void Update(GUILayout layout)
        {
            base.Update(layout);

            if (!isInitialized)
                Initialize(layout);

            // TODO - Skip update if it currently has input focus so user can modify the value in peace

            guiIntField.Value = property.GetValue<int>();
        }

        private void OnFieldValueChanged(int newValue)
        {
            property.SetValue<int>(newValue);
        }

        public override void Destroy()
        {
            if (guiIntField != null)
                guiIntField.Destroy();

            base.Destroy();
        }
    }
}
