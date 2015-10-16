using BansheeEngine;

namespace BansheeEditor
{
    /// <summary>
    /// Displays GUI for a serializable property containing a string.
    /// </summary>
    public class InspectableString : InspectableField
    {
        private string propertyValue;
        private GUITextField guiField;

        /// <summary>
        /// Creates a new inspectable string GUI for the specified property.
        /// </summary>
        /// <param name="title">Name of the property, or some other value to set as the title.</param>
        /// <param name="depth">Determines how deep within the inspector nesting hierarchy is this field. Some fields may
        ///                     contain other fields, in which case you should increase this value by one.</param>
        /// <param name="layout">Parent layout that all the field elements will be added to.</param>
        /// <param name="property">Serializable property referencing the array whose contents to display.</param>
        public InspectableString(string title, int depth, InspectableFieldLayout layout, SerializableProperty property)
            : base(title, depth, layout, property)
        {

        }

        /// <inheritoc/>
        protected internal override void BuildGUI(int layoutIndex)
        {
            if (property.Type == SerializableProperty.FieldType.String)
            {
                guiField = new GUITextField(new GUIContent(title));
                guiField.OnChanged += OnFieldValueChanged;

                layout.AddElement(layoutIndex, guiField);
            }
        }

        /// <inheritdoc/>
        public override bool IsModified()
        {
            string newPropertyValue = property.GetValue<string>();
            if (propertyValue != newPropertyValue)
                return true;

            return base.IsModified();
        }

        /// <inheritdoc/>
        protected internal override void Update(int layoutIndex)
        {
            propertyValue = property.GetValue<string>();
            if (guiField != null)
            {
                if (guiField.HasInputFocus)
                    return;

                guiField.Value = propertyValue;
            }
        }

        /// <summary>
        /// Triggered when the user inputs a new string.
        /// </summary>
        /// <param name="newValue">New value of the string field.</param>
        private void OnFieldValueChanged(string newValue)
        {
            property.SetValue(newValue);
        }
    }
}
