using System;
using BansheeEngine;

namespace BansheeEditor
{
    /// <summary>
    /// Displays GUI for a serializable property containing an array. Array contents are displayed as rows of entries 
    /// that can be shown, hidden or manipulated.
    /// </summary>
    public class InspectableArray : InspectableField
    {
        private object propertyValue; // TODO - This will unnecessarily hold references to the object
        private int numArrayElements;
        private InspectableArrayGUI arrayGUIField = new InspectableArrayGUI();

        /// <summary>
        /// Creates a new inspectable array GUI for the specified property.
        /// </summary>
        /// <param name="title">Name of the property, or some other value to set as the title.</param>
        /// <param name="depth">Determines how deep within the inspector nesting hierarchy is this field. Some fields may
        ///                     contain other fields, in which case you should increase this value by one.</param>
        /// <param name="layout">Parent layout that all the field elements will be added to.</param>
        /// <param name="property">Serializable property referencing the array whose contents to display.</param>
        public InspectableArray(string title, int depth, InspectableFieldLayout layout, SerializableProperty property)
            : base(title, depth, layout, property)
        {

        }

        /// <inheritdoc/>
        public override GUILayoutX GetTitleLayout()
        {
            return arrayGUIField.GetTitleLayout();
        }

        /// <inheritdoc/>
        public override bool IsModified()
        {
            object newPropertyValue = property.GetValue<object>();
            if (propertyValue == null)
                return newPropertyValue != null;

            if (newPropertyValue == null)
                return propertyValue != null;
                
            SerializableArray array = property.GetArray();
            if (array.GetLength() != numArrayElements)
                return true;

            return base.IsModified();
        }

        /// <inheritdoc/>
        public override void Refresh(int layoutIndex)
        {
            if (IsModified())
                Update(layoutIndex);

            arrayGUIField.Refresh();
        }

        /// <inheritdoc/>
        public override bool ShouldRebuildOnModify()
        {
            return true;
        }

        /// <inheritdoc/>
        protected internal override void BuildGUI(int layoutIndex)
        {
            GUILayout arrayLayout = layout.AddLayoutY(layoutIndex);

            arrayGUIField.Update(title, property, arrayLayout, depth);
        }

        /// <inheritdoc/>
        protected internal override void Update(int layoutIndex)
        {
            propertyValue = property.GetValue<object>();
            if (propertyValue != null)
            {
                SerializableArray array = property.GetArray();
                numArrayElements = array.GetLength();
            }
            else
                numArrayElements = 0;

            layout.DestroyElements();
            BuildGUI(layoutIndex);
        }

        /// <summary>
        /// Handles creation of GUI elements for a GUI list field that displays a <see cref="SerializableArray"/> object.
        /// </summary>
        private class InspectableArrayGUI : GUIListFieldBase
        {
            private SerializableProperty property;

            /// <summary>
            /// Constructs a new empty inspectable array GUI.
            /// </summary>
            public InspectableArrayGUI()
            { }

            /// <summary>
            /// Updates the contents of the inspectable GUI array. Must be called at least once in order for the contents 
            /// to be populated.
            /// </summary>
            /// <param name="title">Label to display on the list GUI title.</param>
            /// <param name="property">Serializable property referencing a single-dimensional array.</param>
            /// <param name="layout">Layout to which to append the list GUI elements to.</param>
            /// <param name="depth">Determines at which depth to render the background. Useful when you have multiple
            ///                     nested containers whose backgrounds are overlaping. Also determines background style,
            ///                     depths divisible by two will use an alternate style.</param>
            public void Update(LocString title, SerializableProperty property, GUILayout layout, int depth)
            {
                this.property = property;

                object propertyValue = property.GetValue<object>();
                if (propertyValue != null)
                {
                    SerializableArray array = property.GetArray();
                    base.Update<InspectableArrayGUIRow>(title, false, array.GetLength(), layout, depth);
                }
                else
                    base.Update<InspectableArrayGUIRow>(title, true, 0, layout, depth);
            }

            /// <inheritdoc/>
            protected internal override object GetValue(int seqIndex)
            {
                SerializableArray array = property.GetArray();

                return array.GetProperty(seqIndex);
            }

            /// <inheritdoc/>
            protected internal override void SetValue(int seqIndex, object value)
            {
                // Setting the value should be done through the property
                throw new InvalidOperationException();
            }

            /// <inheritdoc/>
            protected override void OnCreateButtonClicked()
            {
                property.SetValue(property.CreateArrayInstance(new int[1] { 0 }));
            }

            /// <inheritdoc/>
            protected override void OnResizeButtonClicked()
            {
                int size = guiSizeField.Value; // TODO - Support multi-rank arrays

                Array newArray = property.CreateArrayInstance(new int[] { size });
                Array array = property.GetValue<Array>();

                int maxSize = MathEx.Min(size, array.Length);

                for (int i = 0; i < maxSize; i++)
                    newArray.SetValue(array.GetValue(i), i);

                property.SetValue(newArray);
            }

            /// <inheritdoc/>
            protected override void OnClearButtonClicked()
            {
                property.SetValue<object>(null);
            }

            /// <inheritdoc/>
            protected internal override void OnDeleteButtonClicked(int index)
            {
                Array array = property.GetValue<Array>();

                int size = MathEx.Max(0, array.Length - 1);
                Array newArray = property.CreateArrayInstance(new int[] { size });

                int destIdx = 0;
                for (int i = 0; i < array.Length; i++)
                {
                    if (i == index)
                        continue;

                    newArray.SetValue(array.GetValue(i), destIdx);
                    destIdx++;
                }

                property.SetValue(newArray);
            }

            /// <inheritdoc/>
            protected internal override void OnCloneButtonClicked(int index)
            {
                SerializableArray array = property.GetArray();

                int size = array.GetLength() + 1;
                Array newArray = property.CreateArrayInstance(new int[] { size });

                object clonedEntry = null;
                for (int i = 0; i < array.GetLength(); i++)
                {
                    object value = array.GetProperty(i).GetValue<object>();

                    newArray.SetValue(value, i);

                    if (i == index)
                    {
                        clonedEntry = SerializableUtility.Clone(array.GetProperty(i).GetValue<object>());
                    }
                }

                newArray.SetValue(clonedEntry, size - 1);

                property.SetValue(newArray);
            }

            /// <inheritdoc/>
            protected internal override void OnMoveUpButtonClicked(int index)
            {
                Array array = property.GetValue<Array>();

                if ((index - 1) >= 0)
                {
                    object previousEntry = array.GetValue(index - 1);

                    array.SetValue(array.GetValue(index), index - 1);
                    array.SetValue(previousEntry, index);
                }
            }

            /// <inheritdoc/>
            protected internal override void OnMoveDownButtonClicked(int index)
            {
                Array array = property.GetValue<Array>();

                if ((index + 1) < array.Length)
                {
                    object nextEntry = array.GetValue(index + 1);

                    array.SetValue(array.GetValue(index), index + 1);
                    array.SetValue(nextEntry, index);
                }
            }
        }

        /// <summary>
        /// Contains GUI elements for a single entry in the array.
        /// </summary>
        private class InspectableArrayGUIRow : GUIListFieldRow
        {
            private InspectableField field;

            /// <inheritdoc/>
            protected override GUILayoutX CreateGUI(GUILayoutY layout)
            {
                if (field == null)
                {
                    SerializableProperty property = GetValue<SerializableProperty>();

                    field = CreateInspectable(seqIndex + ".", 0, depth + 1,
                        new InspectableFieldLayout(layout), property);
                }

                return field.GetTitleLayout();
            }

            /// <inheritdoc/>
            protected internal override bool Refresh()
            {
                if (field.IsModified())
                {
                    field.Refresh(0);
                    return field.ShouldRebuildOnModify();
                }

                field.Refresh(0);
                return false;
            }
        }
    }
}
