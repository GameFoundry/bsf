using BansheeEngine;

namespace BansheeEditor
{
    /// <summary>
    /// Displays GUI for a serializable property containing a floating point value.
    /// </summary>
    public class InspectableFloat : InspectableField
    {
        private float propertyValue;
        private GUIFloatField guiFloatField;

        /// <summary>
        /// Creates a new inspectable float GUI for the specified property.
        /// </summary>
        /// <param name="title">Name of the property, or some other value to set as the title.</param>
        /// <param name="depth">Determines how deep within the inspector nesting hierarchy is this field. Some fields may
        ///                     contain other fields, in which case you should increase this value by one.</param>
        /// <param name="layout">Parent layout that all the field elements will be added to.</param>
        /// <param name="property">Serializable property referencing the array whose contents to display.</param>
        public InspectableFloat(string title, int depth, InspectableFieldLayout layout, SerializableProperty property)
            : base(title, depth, layout, property)
        {

        }

        /// <inheritoc/>
        protected internal override void BuildGUI(int layoutIndex)
        {
            if (property.Type == SerializableProperty.FieldType.Float)
            {
                guiFloatField = new GUIFloatField(new GUIContent(title));
                guiFloatField.OnChanged += OnFieldValueChanged;

                layout.AddElement(layoutIndex, guiFloatField);
            }
        }

        /// <inheritdoc/>
        public override bool IsModified()
        {
            float newPropertyValue = property.GetValue<float>();
            if (propertyValue != newPropertyValue)
                return true;
                
            return base.IsModified();
        }

        /// <inheritdoc/>
        protected internal override void Update(int layoutIndex)
        {
            propertyValue = property.GetValue<float>();
            if (guiFloatField != null)
            {
                if (guiFloatField.HasInputFocus())
                    return;

                guiFloatField.Value = propertyValue;
            }
        }

        /// <summary>
        /// Triggered when the user inputs a new floating point value.
        /// </summary>
        /// <param name="newValue">New value of the float field.</param>
        private void OnFieldValueChanged(float newValue)
        {
            property.SetValue(newValue);
        }
    }
}
