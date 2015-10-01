using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using BansheeEngine;

namespace BansheeEditor
{
    /// <summary>
    /// Displays GUI for a serializable property containing a 2D vector.
    /// </summary>
    public class InspectableVector2 : InspectableField
    {
        private Vector2 propertyValue;
        private GUIVector2Field guiField;

        /// <summary>
        /// Creates a new inspectable 2D vector GUI for the specified property.
        /// </summary>
        /// <param name="title">Name of the property, or some other value to set as the title.</param>
        /// <param name="depth">Determines how deep within the inspector nesting hierarchy is this field. Some fields may
        ///                     contain other fields, in which case you should increase this value by one.</param>
        /// <param name="layout">Parent layout that all the field elements will be added to.</param>
        /// <param name="property">Serializable property referencing the array whose contents to display.</param>
        public InspectableVector2(string title, int depth, InspectableFieldLayout layout, SerializableProperty property)
            : base(title, depth, layout, property)
        {

        }

        /// <inheritoc/>
        protected override void BuildGUI(int layoutIndex)
        {
            if (property.Type == SerializableProperty.FieldType.Vector2)
            {
                guiField = new GUIVector2Field(new GUIContent(title));
                guiField.OnChanged += OnFieldValueChanged;

                layout.AddElement(layoutIndex, guiField);
            }
        }

        /// <inheritdoc/>
        protected override bool IsModified(out bool rebuildGUI)
        {
            Vector2 newPropertyValue = property.GetValue<Vector2>();
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

            propertyValue = property.GetValue<Vector2>();
            if (guiField != null)
            {
                if (guiField.HasInputFocus())
                    return;

                guiField.Value = propertyValue;
            }
        }

        /// <summary>
        /// Triggered when the user changes the field value.
        /// </summary>
        /// <param name="newValue">New value of the 2D vector field.</param>
        private void OnFieldValueChanged(Vector2 newValue)
        {
            property.SetValue(newValue);
        }
    }
}
