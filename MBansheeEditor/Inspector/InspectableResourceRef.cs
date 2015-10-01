using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using BansheeEngine;

namespace BansheeEditor
{
    /// <summary>
    /// Displays GUI for a serializable property containing a <see cref="Resource"/> reference.
    /// </summary>
    public class InspectableResourceRef : InspectableField
    {
        private Resource propertyValue;
        private GUIResourceField guiField;

        /// <summary>
        /// Creates a new inspectable resource reference GUI for the specified property.
        /// </summary>
        /// <param name="title">Name of the property, or some other value to set as the title.</param>
        /// <param name="depth">Determines how deep within the inspector nesting hierarchy is this field. Some fields may
        ///                     contain other fields, in which case you should increase this value by one.</param>
        /// <param name="layout">Parent layout that all the field elements will be added to.</param>
        /// <param name="property">Serializable property referencing the array whose contents to display.</param>
        public InspectableResourceRef(string title, int depth, InspectableFieldLayout layout, SerializableProperty property)
            : base(title, depth, layout, property)
        {

        }

        /// <inheritoc/>
        protected override void BuildGUI(int layoutIndex)
        {
            if (property.Type == SerializableProperty.FieldType.ResourceRef)
            {
                guiField = new GUIResourceField(property.InternalType, new GUIContent(title));
                guiField.OnChanged += OnFieldValueChanged;

                layout.AddElement(layoutIndex, guiField);
            }
        }

        /// <inheritdoc/>
        protected override bool IsModified(out bool rebuildGUI)
        {
            Resource newPropertyValue = property.GetValue<Resource>();
            if (propertyValue != newPropertyValue)
            {
                rebuildGUI = false;
                return true;
            }

            return base.IsModified(out rebuildGUI);
        }

        /// <inheritdoc/>
        protected override void Update(int layoutIndex, bool rebuildGUI)
        {
            base.Update(layoutIndex, rebuildGUI);

            propertyValue = property.GetValue<Resource>();
            if (guiField != null)
                guiField.Value = propertyValue;
        }

        /// <summary>
        /// Triggered when the user drops a new resource onto the field, or clears the current value.
        /// </summary>
        /// <param name="newValue">New resource to reference.</param>
        private void OnFieldValueChanged(Resource newValue)
        {
            property.SetValue(newValue);
        }
    }
}
