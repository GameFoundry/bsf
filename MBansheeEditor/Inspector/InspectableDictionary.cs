using System;
using System.Collections;
using System.Collections.Generic;
using BansheeEngine;

namespace BansheeEditor
{
    /// <summary>
    /// Displays GUI for a serializable property containing a dictionary. Dictionary contents are displayed as rows of 
    /// entries that can be shown, hidden or manipulated.
    /// </summary>
    public class InspectableDictionary : InspectableField
    {
        private object propertyValue; // TODO - This will unnecessarily hold references to the object
        private int numElements;
        private InspectableDictionaryGUI dictionaryGUIField;

        /// <summary>
        /// Creates a new inspectable dictionary GUI for the specified property.
        /// </summary>
        /// <param name="title">Name of the property, or some other value to set as the title.</param>
        /// <param name="depth">Determines how deep within the inspector nesting hierarchy is this field. Some fields may
        ///                     contain other fields, in which case you should increase this value by one.</param>
        /// <param name="layout">Parent layout that all the field elements will be added to.</param>
        /// <param name="property">Serializable property referencing the dictionary whose contents to display.</param>
        public InspectableDictionary(string title, int depth, InspectableFieldLayout layout, SerializableProperty property)
            : base(title, depth, layout, property)
        {

        }

        /// <inheritdoc/>
        public override GUILayoutX GetTitleLayout()
        {
            return dictionaryGUIField.GetTitleLayout();
        }

        /// <inheritdoc/>
        public override bool IsModified()
        {
            object newPropertyValue = property.GetValue<object>();
            if (propertyValue == null)
                return newPropertyValue != null;

            if (newPropertyValue == null)
                return propertyValue != null;

            SerializableDictionary dictionary = property.GetDictionary();
            if (dictionary.GetLength() != numElements)
                return true;

            return base.IsModified();
        }

        /// <inheritdoc/>
        public override bool Refresh(int layoutIndex)
        {
            bool isModified = IsModified();

            if (isModified)
                Update(layoutIndex);

            isModified |= dictionaryGUIField.Refresh().HasFlag(InspectableState.Modified);

            return isModified;
        }

        /// <inheritdoc/>
        public override bool ShouldRebuildOnModify()
        {
            return true;
        }

        /// <inheritdoc/>
        protected internal override void BuildGUI(int layoutIndex)
        {
            GUILayout dictionaryLayout = layout.AddLayoutY(layoutIndex);

            dictionaryGUIField = InspectableDictionaryGUI.Create(title, property, dictionaryLayout, depth);
        }

        /// <inheritdoc/>
        protected internal override void Update(int layoutIndex)
        {
            propertyValue = property.GetValue<object>();
            if (propertyValue != null)
            {
                SerializableDictionary dictionary = property.GetDictionary();
                numElements = dictionary.GetLength();
            }
            else
                numElements = 0;

            layout.DestroyElements();
            BuildGUI(layoutIndex);
        }

        /// <summary>
        /// Creates GUI elements that allow viewing and manipulation of a <see cref="SerializableDictionary"/> referenced
        /// by a serializable property.
        /// </summary>
        public class InspectableDictionaryGUI : GUIDictionaryFieldBase
        {
            private SerializableProperty property;
            private List<object> orderedKeys = new List<object>();

            /// <summary>
            /// Constructs a new dictionary GUI.
            /// </summary>
            /// <param name="title">Label to display on the list GUI title.</param>
            /// <param name="property">Serializable property referencing a dictionary</param>
            /// <param name="layout">Layout to which to append the list GUI elements to.</param>
            /// <param name="depth">Determines at which depth to render the background. Useful when you have multiple
            ///                     nested containers whose backgrounds are overlaping. Also determines background style,
            ///                     depths divisible by two will use an alternate style.</param>
            protected InspectableDictionaryGUI(LocString title, SerializableProperty property, GUILayout layout, int depth = 0)
            : base(title, layout, depth)
        {
            this.property = property;
            UpdateKeys();
        }

            /// <summary>
            /// Builds the inspectable dictionary GUI elements. Must be called at least once in order for the contents to 
            /// be populated.
            /// </summary>
            /// <param name="title">Label to display on the list GUI title.</param>
            /// <param name="property">Serializable property referencing a dictionary</param>
            /// <param name="layout">Layout to which to append the list GUI elements to.</param>
            /// <param name="depth">Determines at which depth to render the background. Useful when you have multiple
            ///                     nested containers whose backgrounds are overlaping. Also determines background style,
            ///                     depths divisible by two will use an alternate style.</param>
            public static InspectableDictionaryGUI Create(LocString title, SerializableProperty property, GUILayout layout, 
                int depth = 0)
            {
                InspectableDictionaryGUI guiDictionary = new InspectableDictionaryGUI(title, property, layout, depth);

                guiDictionary.BuildGUI();
                return guiDictionary;
            }

            /// <summary>
            /// Updates the ordered set of keys used for mapping sequential indexes to keys. Should be called whenever a 
            /// dictionary key changes.
            /// </summary>
            private void UpdateKeys()
            {
                orderedKeys.Clear();

                IDictionary dictionary = property.GetValue<IDictionary>();
                if (dictionary != null)
                {
                    foreach (var key in dictionary)
                        orderedKeys.Add(key);
                }
            }

            /// <inheritdoc/>
            protected override GUIDictionaryFieldRow CreateRow()
            {
                return new InspectableDictionaryGUIRow();
            }

            /// <inheritdoc/>
            protected override int GetNumRows()
            {
                IDictionary dictionary = property.GetValue<IDictionary>();
                if (dictionary != null)
                    return dictionary.Count;

                return 0;
            }

            /// <inheritdoc/>
            protected override bool IsNull()
            {
                IDictionary dictionary = property.GetValue<IDictionary>();
                return dictionary == null;
            }

            /// <inheritdoc/>
            protected internal override object GetKey(int rowIdx)
            {
                return orderedKeys[rowIdx];
            }

            /// <inheritdoc/>
            protected internal override object GetValue(object key)
            {
                SerializableDictionary dictionary = property.GetDictionary();
                return dictionary.GetProperty(key);
            }

            /// <inheritdoc/>
            protected internal override void SetValue(object key, object value)
            {
                // Setting the value should be done through the property
                throw new InvalidOperationException();
            }

            /// <inheritdoc/>
            protected internal override bool Contains(object key)
            {
                IDictionary dictionary = property.GetValue<IDictionary>();
                return dictionary.Contains(key); ;
            }

            /// <inheritdoc/>
            protected internal override void EditEntry(object oldKey, object newKey, object value)
            {
                IDictionary dictionary = property.GetValue<IDictionary>();
                dictionary.Remove(oldKey);
                dictionary.Add(newKey, value);

                UpdateKeys();
            }

            /// <inheritdoc/>
            protected internal override void AddEntry(object key, object value)
            {
                IDictionary dictionary = property.GetValue<IDictionary>();
                dictionary.Add(key, value);

                UpdateKeys();
            }

            /// <inheritdoc/>
            protected internal override void RemoveEntry(object key)
            {
                IDictionary dictionary = property.GetValue<IDictionary>();
                dictionary.Remove(key);

                UpdateKeys();
            }

            /// <inheritdoc/>
            protected internal override object CreateKey()
            {
                SerializableDictionary dictionary = property.GetDictionary();
                return SerializableUtility.Create(dictionary.KeyType);
            }

            /// <inheritdoc/>
            protected internal override object CreateValue()
            {
                SerializableDictionary dictionary = property.GetDictionary();
                return SerializableUtility.Create(dictionary.ValueType);
            }

            /// <inheritdoc/>
            protected override void CreateDictionary()
            {
                property.SetValue(property.CreateDictionaryInstance());
                UpdateKeys();
            }

            /// <inheritdoc/>
            protected override void DeleteDictionary()
            {
                property.SetValue<object>(null);
                UpdateKeys();
            }
        }

        /// <summary>
        /// Contains GUI elements for a single key/value pair in the dictionary.
        /// </summary>
        private class InspectableDictionaryGUIRow : GUIDictionaryFieldRow
        {
            private InspectableField fieldKey;
            private InspectableField fieldValue;

            /// <inheritdoc/>
            protected override GUILayoutX CreateKeyGUI(GUILayoutY layout)
            {
                if (fieldKey == null)
                {
                    SerializableProperty property = GetKey<SerializableProperty>();

                    fieldKey = CreateInspectable("Key", 0, Depth + 1,
                        new InspectableFieldLayout(layout), property);
                }

                return fieldKey.GetTitleLayout();
            }

            /// <inheritdoc/>
            protected override void CreateValueGUI(GUILayoutY layout)
            {
                if (fieldValue == null)
                {
                    SerializableProperty property = GetValue<SerializableProperty>();

                    fieldValue = CreateInspectable("Value", 0, Depth + 1,
                        new InspectableFieldLayout(layout), property);
                }
            }

            /// <inheritdoc/>
            protected internal override InspectableState Refresh()
            {
                //InspectableState state = fieldKey.Refresh(0) | fieldValue.Refresh(0);
                InspectableState state = InspectableState.NotModified;
                if (fieldKey.Refresh(0) || fieldValue.Refresh(0))
                    state = InspectableState.Modified;

                if (state.HasFlag(InspectableState.Modified))
                {
                    if (fieldKey.ShouldRebuildOnModify())
                        BuildGUI();
                }

                return state;
            }
        }
    }
}
