//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
using BansheeEngine;

namespace BansheeEditor
{
    /** @addtogroup Inspector
     *  @{
     */

    /// <summary>
    /// Displays GUI for a serializable property containing a <see cref="GameObject"/> reference.
    /// </summary>
    public class InspectableGameObjectRef : InspectableField
    {
        private GUIGameObjectField guiField;
        private InspectableState state;

        /// <summary>
        /// Creates a new inspectable game object reference GUI for the specified property.
        /// </summary>
        /// <param name="parent">Parent Inspector this field belongs to.</param>
        /// <param name="title">Name of the property, or some other value to set as the title.</param>
        /// <param name="path">Full path to this property (includes name of this property and all parent properties).</param>
        /// <param name="depth">Determines how deep within the inspector nesting hierarchy is this field. Some fields may
        ///                     contain other fields, in which case you should increase this value by one.</param>
        /// <param name="layout">Parent layout that all the field elements will be added to.</param>
        /// <param name="property">Serializable property referencing the array whose contents to display.</param>
        public InspectableGameObjectRef(Inspector parent, string title, string path, int depth, InspectableFieldLayout layout,
            SerializableProperty property)
            : base(parent, title, path, SerializableProperty.FieldType.GameObjectRef, depth, layout, property)
        {

        }

        /// <inheritoc/>
        protected internal override void Initialize(int layoutIndex)
        {
            if (property != null)
            {
                guiField = new GUIGameObjectField(property.InternalType, new GUIContent(title));
                guiField.OnChanged += OnFieldValueChanged;

                layout.AddElement(layoutIndex, guiField);
            }
        }

        /// <inheritdoc/>
        public override InspectableState Refresh(int layoutIndex)
        {
            if (guiField != null)
                guiField.Value = property.GetValue<GameObject>();

            InspectableState oldState = state;
            if (state.HasFlag(InspectableState.Modified))
                state = InspectableState.NotModified;

            return oldState;
        }

        /// <summary>
        /// Triggered when the user drops a new game object onto the field, or clears the current value.
        /// </summary>
        /// <param name="newValue">New game object to reference.</param>
        private void OnFieldValueChanged(GameObject newValue)
        {
            property.SetValue(newValue);
            state = InspectableState.Modified;
        }
    }

    /** @} */
}
