using BansheeEngine;

namespace BansheeEditor
{
    /// <summary>
    /// Displays GUI for a serializable property containing a 4D vector.
    /// </summary>
    public class InspectableVector4 : InspectableField
    {
        private Vector4 propertyValue;
        private GUIVector4Field guiField;

        /// <summary>
        /// Creates a new inspectable 4D vector GUI for the specified property.
        /// </summary>
        /// <param name="title">Name of the property, or some other value to set as the title.</param>
        /// <param name="depth">Determines how deep within the inspector nesting hierarchy is this field. Some fields may
        ///                     contain other fields, in which case you should increase this value by one.</param>
        /// <param name="layout">Parent layout that all the field elements will be added to.</param>
        /// <param name="property">Serializable property referencing the array whose contents to display.</param>
        public InspectableVector4(string title, int depth, InspectableFieldLayout layout, SerializableProperty property)
            : base(title, depth, layout, property)
        {

        }

        /// <inheritoc/>
        protected internal override void BuildGUI(int layoutIndex)
        {
            if (property.Type == SerializableProperty.FieldType.Vector4)
            {
                guiField = new GUIVector4Field(new GUIContent(title));
                guiField.OnChanged += OnFieldValueChanged;

                layout.AddElement(layoutIndex, guiField);
            }
        }

        /// <inheritdoc/>
        public override bool IsModified()
        {
            Vector4 newPropertyValue = property.GetValue<Vector4>();
            if (propertyValue != newPropertyValue)
                return true;

            return base.IsModified();
        }

        /// <inheritdoc/>
        protected internal override void Update(int layoutIndex)
        {
            propertyValue = property.GetValue<Vector4>();
            if (guiField != null)
            {
                if (guiField.HasInputFocus)
                    return;

                guiField.Value = propertyValue;
            }
        }

        /// <summary>
        /// Triggered when the user changes the field value.
        /// </summary>
        /// <param name="newValue">New value of the 3D vector field.</param>
        private void OnFieldValueChanged(Vector4 newValue)
        {
            property.SetValue(newValue);
        }
    }
}
