using BansheeEngine;

namespace BansheeEditor
{
    /// <summary>
    /// Displays GUI for a serializable property containing a <see cref="GameObject"/> reference.
    /// </summary>
    public class InspectableGameObjectRef : InspectableField
    {
        private GameObject propertyValue;
        private GUIGameObjectField guiField;

        /// <summary>
        /// Creates a new inspectable game object reference GUI for the specified property.
        /// </summary>
        /// <param name="title">Name of the property, or some other value to set as the title.</param>
        /// <param name="depth">Determines how deep within the inspector nesting hierarchy is this field. Some fields may
        ///                     contain other fields, in which case you should increase this value by one.</param>
        /// <param name="layout">Parent layout that all the field elements will be added to.</param>
        /// <param name="property">Serializable property referencing the array whose contents to display.</param>
        public InspectableGameObjectRef(string title, int depth, InspectableFieldLayout layout, SerializableProperty property)
            : base(title, depth, layout, property)
        {

        }

        /// <inheritoc/>
        protected internal override void BuildGUI(int layoutIndex)
        {
            if (property.Type == SerializableProperty.FieldType.GameObjectRef)
            {
                guiField = new GUIGameObjectField(property.InternalType, new GUIContent(title));
                guiField.OnChanged += OnFieldValueChanged;

                layout.AddElement(layoutIndex, guiField);
            }
        }

        /// <inheritdoc/>
        public override bool IsModified()
        {
            GameObject newPropertyValue = property.GetValue<GameObject>();
            if (propertyValue != newPropertyValue)
                return true;
                
            return base.IsModified();
        }

        /// <inheritdoc/>
        protected internal override void Update(int layoutIndex)
        {
            propertyValue = property.GetValue<GameObject>();
            if (guiField != null)
                guiField.Value = propertyValue;
        }

        /// <summary>
        /// Triggered when the user drops a new game object onto the field, or clears the current value.
        /// </summary>
        /// <param name="newValue">New game object to reference.</param>
        private void OnFieldValueChanged(GameObject newValue)
        {
            property.SetValue(newValue);
        }
    }
}
