using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using BansheeEngine;

namespace BansheeEditor
{
    /// <summary>
    /// Displays GUI for a serializable property containing an integer value.
    /// </summary>
    public class InspectableInt : InspectableField
    {
        private int propertyValue;
        private GUIIntField guiIntField;

        /// <summary>
        /// Creates a new inspectable integer GUI for the specified property.
        /// </summary>
        /// <param name="title">Name of the property, or some other value to set as the title.</param>
        /// <param name="depth">Determines how deep within the inspector nesting hierarchy is this field. Some fields may
        ///                     contain other fields, in which case you should increase this value by one.</param>
        /// <param name="layout">Parent layout that all the field elements will be added to.</param>
        /// <param name="property">Serializable property referencing the array whose contents to display.</param>
        public InspectableInt(string title, int depth, InspectableFieldLayout layout, SerializableProperty property)
            : base(title, depth, layout, property)
        {

        }

        /// <inheritdoc/>
        protected override void BuildGUI(int layoutIndex)
        {
            if (property.Type == SerializableProperty.FieldType.Int)
            {
                guiIntField = new GUIIntField(new GUIContent(title));
                guiIntField.OnChanged += OnFieldValueChanged;

                layout.AddElement(layoutIndex, guiIntField);
            }
        }

        /// <inheritdoc/>
        protected override bool IsModified(out bool rebuildGUI)
        {
            int newPropertyValue = property.GetValue<int>();
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

            propertyValue = property.GetValue<int>();
            if (guiIntField != null)
            {
                if (guiIntField.HasInputFocus())
                    return;

                guiIntField.Value = propertyValue;
            }
        }

        /// <summary>
        /// Triggered when the user inputs a new integer value.
        /// </summary>
        /// <param name="newValue">New value of the int field.</param>
        private void OnFieldValueChanged(int newValue)
        {
            property.SetValue(newValue);
        }
    }
}
