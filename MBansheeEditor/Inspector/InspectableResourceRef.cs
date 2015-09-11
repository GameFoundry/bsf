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
        private bool isInitialized;

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

        /// <summary>
        /// Initializes the GUI elements the first time <see cref="Update"/> gets called.
        /// </summary>
        /// <param name="layoutIndex">Index at which to insert the GUI elements.</param>
        private void Initialize(int layoutIndex)
        {
            if (property.Type == SerializableProperty.FieldType.ResourceRef)
            {
                guiField = new GUIResourceField(property.InternalType, new GUIContent(title));
                guiField.OnChanged += OnFieldValueChanged;

                layout.AddElement(layoutIndex, guiField);
            }

            isInitialized = true;
        }

        /// <inheritdoc/>
        protected override bool IsModified()
        {
            if (!isInitialized)
                return true;

            Resource newPropertyValue = property.GetValue<Resource>();
            if (propertyValue != newPropertyValue)
                return true;

            return base.IsModified();
        }

        /// <inheritdoc/>
        protected override void Update(int layoutIndex)
        {
            base.Update(layoutIndex);

            if (!isInitialized)
                Initialize(layoutIndex);

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
