using BansheeEngine;

namespace BansheeEditor
{
    /// <summary>
    /// Displays GUI for a serializable property containing a 3D vector.
    /// </summary>
    public class InspectableVector3 : InspectableField
    {
        private GUIVector3Field guiField;
        private InspectableState state;

        /// <summary>
        /// Creates a new inspectable 3D vector GUI for the specified property.
        /// </summary>
        /// <param name="title">Name of the property, or some other value to set as the title.</param>
        /// <param name="depth">Determines how deep within the inspector nesting hierarchy is this field. Some fields may
        ///                     contain other fields, in which case you should increase this value by one.</param>
        /// <param name="layout">Parent layout that all the field elements will be added to.</param>
        /// <param name="property">Serializable property referencing the array whose contents to display.</param>
        public InspectableVector3(string title, int depth, InspectableFieldLayout layout, SerializableProperty property)
            : base(title, SerializableProperty.FieldType.Vector3, depth, layout, property)
        {

        }

        /// <inheritoc/>
        protected internal override void Initialize(int layoutIndex)
        {
            if (property.Type == SerializableProperty.FieldType.Vector3)
            {
                guiField = new GUIVector3Field(new GUIContent(title));
                guiField.OnChanged += OnFieldValueChanged;
                guiField.OnConfirmed += OnFieldValueConfirm;
                guiField.OnFocusLost += OnFieldValueConfirm;

                layout.AddElement(layoutIndex, guiField);
            }
        }

        /// <inheritdoc/>
        public override InspectableState Refresh(int layoutIndex)
        {
            if (guiField != null && !guiField.HasInputFocus)
                guiField.Value = property.GetValue<Vector3>();

            InspectableState oldState = state;
            if (state.HasFlag(InspectableState.Modified))
                state = InspectableState.NotModified;

            return oldState;
        }

        /// <summary>
        /// Triggered when the user changes the field value.
        /// </summary>
        /// <param name="newValue">New value of the 3D vector field.</param>
        private void OnFieldValueChanged(Vector3 newValue)
        {
            property.SetValue(newValue);
            state |= InspectableState.ModifyInProgress;
        }

        /// <summary>
        /// Triggered when the user confirms input in the 3D vector field.
        /// </summary>
        private void OnFieldValueConfirm()
        {
            if (state.HasFlag(InspectableState.ModifyInProgress))
                state |= InspectableState.Modified;
        }
    }
}
