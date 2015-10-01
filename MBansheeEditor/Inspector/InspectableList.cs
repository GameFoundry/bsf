using System;
using System.Collections;
using System.Collections.Generic;
using BansheeEngine;

namespace BansheeEditor
{
    /// <summary>
    /// Displays GUI for a serializable property containing a list. List contents are displayed as rows of entries 
    /// that can be shown, hidden or manipulated.
    /// </summary>
    public class InspectableList : InspectableField
    {
        private object propertyValue; // TODO - This will unnecessarily hold references to the object
        private int numArrayElements;
        private InspectableListGUI listGUIField;

        /// <summary>
        /// Creates a new inspectable list GUI for the specified property.
        /// </summary>
        /// <param name="title">Name of the property, or some other value to set as the title.</param>
        /// <param name="depth">Determines how deep within the inspector nesting hierarchy is this field.Some fields may
        ///                     contain other fields, in which case you should increase this value by one.</param>
        /// <param name="layout">Parent layout that all the field elements will be added to.</param>
        /// <param name="property">Serializable property referencing the array whose contents to display.</param>
        public InspectableList(string title, int depth, InspectableFieldLayout layout, SerializableProperty property)
            : base(title, depth, layout, property)
        {

        }

        /// <inheritdoc/>
        public override GUILayoutX GetTitleLayout()
        {
            return listGUIField.GetTitleLayout();
        }

        /// <inheritdoc/>
        public override bool IsModified()
        {
            object newPropertyValue = property.GetValue<object>();
            if (propertyValue == null)
                return newPropertyValue != null;

            if (newPropertyValue == null)
                return propertyValue != null;

            SerializableList list = property.GetList();
            if (list.GetLength() != numArrayElements)
                return true;

            return base.IsModified();
        }

        /// <inheritdoc/>
        public override bool Refresh(int layoutIndex)
        {
            bool anythingModified = false;

            if (IsModified())
            {
                Update(layoutIndex);
                anythingModified = true;
            }

            anythingModified |= listGUIField.Refresh();
            return anythingModified;
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

            listGUIField = InspectableListGUI.Create(title, property, arrayLayout);
        }

        /// <inheritdoc/>
        protected internal override void Update(int layoutIndex)
        {
            propertyValue = property.GetValue<object>();
            if (propertyValue != null)
            {
                SerializableList list = property.GetList();
                numArrayElements = list.GetLength();
            }
            else
                numArrayElements = 0;

            layout.DestroyElements();
            BuildGUI(layoutIndex);
        }

        /// <summary>
        /// Handles creation of GUI elements for a GUI list field that displays a <see cref="SerializableList"/> object.
        /// </summary>
        private class InspectableListGUI : GUIListFieldBase
        {
            private SerializableProperty property;

            /// <summary>
            /// Constructs an inspectable list GUI.
            /// </summary>
            /// <param name="property">Serializable property referencing a list.</param>
            private InspectableListGUI(SerializableProperty property)
            {
                this.property = property;
            }
            
            /// <summary>
            /// Creates a new inspectable GUI list.
            /// </summary>
            /// <param name="title">Label to display on the list GUI title.</param>
            /// <param name="property">Serializable property referencing a list.</param>
            /// <param name="layout">Layout to which to append the list GUI elements to.</param>
            /// <returns>Newly created inspectable GUI list object.</returns>
            public static InspectableListGUI Create(LocString title, SerializableProperty property, GUILayout layout)
            {
                InspectableListGUI newArrayField = new InspectableListGUI(property);

                object propertyValue = property.GetValue<object>();
                if (propertyValue != null)
                {
                    SerializableList list = property.GetList();
                    newArrayField.Construct<InspectableListGUIRow>(title, false, list.GetLength(), layout);
                }
                else
                    newArrayField.Construct<InspectableListGUIRow>(title, true, 0, layout);

                return newArrayField;
            }

            /// <inheritdoc/>
            protected internal override object GetValue(int seqIndex)
            {
                SerializableList array = property.GetList();

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
                property.SetValue(property.CreateListInstance(0));
            }

            /// <inheritdoc/>
            protected override void OnResizeButtonClicked()
            {
                int size = guiSizeField.Value;

                IList newList = property.CreateListInstance(size);
                IList list = property.GetValue<IList>();

                int maxSize = MathEx.Min(size, list.Count);
                for (int i = 0; i < maxSize; i++)
                    newList[i] = list[i];

                property.SetValue(newList);
            }

            /// <inheritdoc/>
            protected override void OnClearButtonClicked()
            {
                property.SetValue<object>(null);
            }

            /// <inheritdoc/>
            protected internal override void OnDeleteButtonClicked(int index)
            {
                IList list = property.GetValue<IList>();

                if (index >= 0 && index < list.Count)
                    list.RemoveAt(index);
            }

            /// <inheritdoc/>
            protected internal override void OnCloneButtonClicked(int index)
            {
                SerializableList serializableList = property.GetList();
                IList list = property.GetValue<IList>();

                if (index >= 0 && index < list.Count)
                    list.Add(serializableList.GetProperty(index).GetValueCopy<object>());
            }

            /// <inheritdoc/>
            protected internal override void OnMoveUpButtonClicked(int index)
            {
                IList list = property.GetValue<IList>();

                if ((index - 1) >= 0)
                {
                    object previousEntry = list[index - 1];

                    list[index - 1] = list[index];
                    list[index] = previousEntry;
                }
            }

            /// <inheritdoc/>
            protected internal override void OnMoveDownButtonClicked(int index)
            {
                IList list = property.GetValue<IList>();

                if ((index + 1) < list.Count)
                {
                    object nextEntry = list[index + 1];

                    list[index + 1] = list[index];
                    list[index] = nextEntry;
                }
            }
        }

        /// <summary>
        /// Contains GUI elements for a single entry in the array.
        /// </summary>
        private class InspectableListGUIRow : GUIListFieldRow
        {
            private InspectableField field;

            /// <inheritdoc/>
            protected override GUILayoutX CreateGUI(GUILayoutY layout)
            {
                if (field == null)
                {
                    SerializableProperty property = GetValue<SerializableProperty>();

                    field = CreateInspectable(seqIndex + ".", 0, 0,
                        new InspectableFieldLayout(layout), property);
                }

                return field.GetTitleLayout();
            }

            /// <inheritdoc/>
            protected internal override bool Refresh(out bool rebuildGUI)
            {
                if (field.IsModified())
                {
                    rebuildGUI = field.ShouldRebuildOnModify();
                    return field.Refresh(0);
                }

                rebuildGUI = false;
                return field.Refresh(0);
            }
        }
    }
}
