using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using BansheeEngine;

namespace BansheeEditor
{
    /// <summary>
    /// Displays GUI for a serializable property containing an array. Array contents are displayed as rows of entries 
    /// that can be shown, hidden or manipulated.
    /// </summary>
    public class InspectableArray : InspectableField
    {
        /// <summary>
        /// Contains GUI elements for a single entry in the array.
        /// </summary>
        private class EntryRow
        {
            public GUILayoutY contentLayout;
            private GUILayoutX rowLayout;
            private GUILayoutX titleLayout;
            private bool ownsTitleLayout;

            /// <summary>
            /// Constructs a new entry row object.
            /// </summary>
            /// <param name="parentLayout">Parent layout that row GUI elements will be added to.</param>
            public EntryRow(GUILayout parentLayout)
            {
                rowLayout = parentLayout.AddLayoutX();
                contentLayout = rowLayout.AddLayoutY();
            }

            /// <summary>
            /// Recreates all row GUI elements.
            /// </summary>
            /// <param name="child">Inspectable field of the array entry.</param>
            /// <param name="seqIndex">Sequential index of the array entry.</param>
            /// <param name="parent">Parent array object that the entry is contained in.</param>
            public void Refresh(InspectableField child, int seqIndex, InspectableArray parent)
            {
                if (ownsTitleLayout || (titleLayout != null && titleLayout == child.GetTitleLayout()))
                    return;

                titleLayout = child.GetTitleLayout();
                if (titleLayout == null)
                {
                    GUILayoutY buttonCenter = rowLayout.AddLayoutY();
                    buttonCenter.AddFlexibleSpace();
                    titleLayout = buttonCenter.AddLayoutX();
                    buttonCenter.AddFlexibleSpace();

                    ownsTitleLayout = true;
                }

                GUIContent cloneIcon = new GUIContent(EditorBuiltin.GetInspectorWindowIcon(InspectorWindowIcon.Clone));
                GUIContent deleteIcon = new GUIContent(EditorBuiltin.GetInspectorWindowIcon(InspectorWindowIcon.Delete));
                GUIContent moveUp = new GUIContent(EditorBuiltin.GetInspectorWindowIcon(InspectorWindowIcon.MoveUp));
                GUIContent moveDown = new GUIContent(EditorBuiltin.GetInspectorWindowIcon(InspectorWindowIcon.MoveDown));

                GUIButton cloneBtn = new GUIButton(cloneIcon, GUIOption.FixedWidth(30));
                GUIButton deleteBtn = new GUIButton(deleteIcon, GUIOption.FixedWidth(30));
                GUIButton moveUpBtn = new GUIButton(moveUp, GUIOption.FixedWidth(30));
                GUIButton moveDownBtn = new GUIButton(moveDown, GUIOption.FixedWidth(30));

                cloneBtn.OnClick += () => parent.OnCloneButtonClicked(seqIndex);
                deleteBtn.OnClick += () => parent.OnDeleteButtonClicked(seqIndex);
                moveUpBtn.OnClick += () => parent.OnMoveUpButtonClicked(seqIndex);
                moveDownBtn.OnClick += () => parent.OnMoveDownButtonClicked(seqIndex);

                titleLayout.AddElement(cloneBtn);
                titleLayout.AddElement(deleteBtn);
                titleLayout.AddElement(moveUpBtn);
                titleLayout.AddElement(moveDownBtn);
            }

            /// <summary>
            /// Destroys all row GUI elements.
            /// </summary>
            public void Destroy()
            {
                rowLayout.Destroy();
            }
        }

        private const int IndentAmount = 5;

        private object propertyValue; // TODO - This will unnecessarily hold references to the object
        private int numArrayElements;

        private List<EntryRow> rows = new List<EntryRow>();
        private GUIIntField guiSizeField;
        private GUILayoutX guiChildLayout;
        private GUILayoutX guiTitleLayout;
        private bool isExpanded;

        private bool forceUpdate = true;

        /// <summary>
        /// Creates a new inspectable array GUI for the specified property.
        /// </summary>
        /// <param name="title">Name of the property, or some other value to set as the title.</param>
        /// <param name="depth">Determines how deep within the inspector nesting hierarchy is this field.Some fields may
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
            return guiTitleLayout;
        }

        /// <inheritdoc/>
        protected override bool IsModified(out bool rebuildGUI)
        {
            if (forceUpdate)
            {
                rebuildGUI = true;
                return true;
            }

            object newPropertyValue = property.GetValue<object>();
            if (propertyValue == null)
            {
                rebuildGUI = newPropertyValue != null;
                return rebuildGUI;
            }

            if (newPropertyValue == null)
            {
                rebuildGUI = propertyValue != null;
                return rebuildGUI;
            }
                
            SerializableArray array = property.GetArray();
            if (array.GetLength() != numArrayElements)
            {
                rebuildGUI = true;
                return true;
            }
                
            return base.IsModified(out rebuildGUI);
        }

        /// <inheritdoc/>
        public override bool Refresh(int layoutIndex, out bool updateGUI)
        {
            bool anythingModified = false;

            if (IsModified(out updateGUI))
            {
                Update(layoutIndex, updateGUI);
                anythingModified = true;
            }
                
            for (int i = 0; i < ChildCount; i++)
            {
                bool dummy;

                InspectableField child = GetChild(i);
                bool childModified = child.Refresh(0, out dummy);

                if (childModified)
                    rows[i].Refresh(child, i, this);

                anythingModified |= childModified;
            }

            return anythingModified;
        }

        /// <inheritdoc/>
        protected override void BuildGUI(int layoutIndex)
        {
            guiTitleLayout = null;
            guiChildLayout = null;

            foreach (var row in rows)
                row.Destroy();

            rows.Clear();
            layout.DestroyElements();

            if (property.Type != SerializableProperty.FieldType.Array || property.InternalType.GetArrayRank() != 1) // We don't support multirank arrays
                return;

            propertyValue = property.GetValue<object>();
            if (propertyValue == null)
            {
                guiChildLayout = null;
                guiTitleLayout = layout.AddLayoutX(layoutIndex);

                guiTitleLayout.AddElement(new GUILabel(title));
                guiTitleLayout.AddElement(new GUILabel("Empty", GUIOption.FixedWidth(100)));

                if (!property.IsValueType)
                {
                    GUIContent createIcon = new GUIContent(EditorBuiltin.GetInspectorWindowIcon(InspectorWindowIcon.Create));
                    GUIButton createBtn = new GUIButton(createIcon, GUIOption.FixedWidth(30));
                    createBtn.OnClick += OnCreateButtonClicked;
                    guiTitleLayout.AddElement(createBtn);
                }

                numArrayElements = 0;
            }
            else
            {
                GUIToggle guiFoldout = new GUIToggle(title, EditorStyles.Foldout);
                guiFoldout.Value = isExpanded;
                guiFoldout.OnToggled += OnFoldoutToggled;
                guiSizeField = new GUIIntField("", GUIOption.FixedWidth(50));
                guiSizeField.SetRange(0, int.MaxValue);

                GUIContent resizeIcon = new GUIContent(EditorBuiltin.GetInspectorWindowIcon(InspectorWindowIcon.Resize));
                GUIButton guiResizeBtn = new GUIButton(resizeIcon, GUIOption.FixedWidth(30));
                guiResizeBtn.OnClick += OnResizeButtonClicked;

                GUIContent clearIcon = new GUIContent(EditorBuiltin.GetInspectorWindowIcon(InspectorWindowIcon.Clear));
                GUIButton guiClearBtn = new GUIButton(clearIcon, GUIOption.FixedWidth(30));
                guiClearBtn.OnClick += OnClearButtonClicked;

                guiTitleLayout = layout.AddLayoutX(layoutIndex);
                guiTitleLayout.AddElement(guiFoldout);
                guiTitleLayout.AddElement(guiSizeField);
                guiTitleLayout.AddElement(guiResizeBtn);
                guiTitleLayout.AddElement(guiClearBtn);

                SerializableArray array = property.GetArray();
                numArrayElements = array.GetLength();
                guiSizeField.Value = numArrayElements;

                if (isExpanded)
                {
                    if (numArrayElements > 0)
                    {
                        guiChildLayout = layout.AddLayoutX(layoutIndex);
                        guiChildLayout.AddSpace(IndentAmount);

                        GUIPanel guiContentPanel = guiChildLayout.AddPanel();
                        GUILayoutX guiIndentLayoutX = guiContentPanel.AddLayoutX();
                        guiIndentLayoutX.AddSpace(IndentAmount);
                        GUILayoutY guiIndentLayoutY = guiIndentLayoutX.AddLayoutY();
                        guiIndentLayoutY.AddSpace(IndentAmount);
                        GUILayoutY guiContentLayout = guiIndentLayoutY.AddLayoutY();
                        guiIndentLayoutY.AddSpace(IndentAmount);
                        guiIndentLayoutX.AddSpace(IndentAmount);
                        guiChildLayout.AddSpace(IndentAmount);

                        short backgroundDepth = (short)(Inspector.START_BACKGROUND_DEPTH - depth - 1);
                        string bgPanelStyle = depth % 2 == 0
                            ? EditorStyles.InspectorContentBgAlternate
                            : EditorStyles.InspectorContentBg;

                        GUIPanel backgroundPanel = guiContentPanel.AddPanel(backgroundDepth);
                        GUITexture inspectorContentBg = new GUITexture(null, bgPanelStyle);
                        backgroundPanel.AddElement(inspectorContentBg);

                        for (int i = 0; i < numArrayElements; i++)
                        {
                            EntryRow newRow = new EntryRow(guiContentLayout);
                            rows.Add(newRow);

                            InspectableField childObj = CreateInspectable(i + ".", 0, depth + 1,
                                new InspectableFieldLayout(newRow.contentLayout), array.GetProperty(i));
                            AddChild(childObj);

                            rows[i].Refresh(childObj, i, this);
                        }
                    }
                }
                else
                    guiChildLayout = null;
            }
        }

        /// <inheritdoc/>
        protected override void Update(int layoutIndex, bool rebuildGUI)
        {
            base.Update(layoutIndex, true);
            BuildGUI(layoutIndex);

            forceUpdate = false;
            
        }

        /// <summary>
        /// Triggered when the user clicks on the expand/collapse toggle in the title bar.
        /// </summary>
        /// <param name="expanded">Determines whether the contents were expanded or collapsed.</param>
        private void OnFoldoutToggled(bool expanded)
        {
            isExpanded = expanded;
            forceUpdate = true;
        }

        /// <summary>
        /// Triggered when the user clicks on the resize button on the title bar. Changes the size of the array while
        /// preserving existing contents.
        /// </summary>
        private void OnResizeButtonClicked()
        {
            int size = guiSizeField.Value; // TODO - Support multi-rank arrays

            Array newArray = property.CreateArrayInstance(new int[] {size});
            Array array = property.GetValue<Array>();

            int maxSize = MathEx.Min(size, array.Length);

            for (int i = 0; i < maxSize; i++)
                newArray.SetValue(array.GetValue(i), i);

            property.SetValue(newArray);
        }

        /// <summary>
        /// Triggered when the user clicks on the delete button next to the array entry. Deletes an element in the array.
        /// </summary>
        /// <param name="index">Sequential index of the element in the array to remove.</param>
        private void OnDeleteButtonClicked(int index)
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

        /// <summary>
        /// Triggered when the user clicks on the clone button next to the array entry. Clones an element in the array and
        /// adds the clone to the back of the array.
        /// </summary>
        /// <param name="index">Sequential index of the element in the array to clone.</param>
        private void OnCloneButtonClicked(int index)
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
                    clonedEntry = array.GetProperty(i).GetValueCopy<object>();
                }
            }

            newArray.SetValue(clonedEntry, size - 1);

            property.SetValue(newArray);
        }

        /// <summary>
        /// Triggered when the user clicks on the move up button next to the array entry. Moves an element from the current
        /// array index to the one right before it, if not at zero.
        /// </summary>
        /// <param name="index">Sequential index of the element in the array to move.</param>
        private void OnMoveUpButtonClicked(int index)
        {
            Array array = property.GetValue<Array>();

            if ((index - 1) >= 0)
            {
                object previousEntry = array.GetValue(index - 1);

                array.SetValue(array.GetValue(index), index - 1);
                array.SetValue(previousEntry, index);
            }
        }

        /// <summary>
        /// Triggered when the user clicks on the move down button next to the array entry. Moves an element from the current
        /// array index to the one right after it, if the element isn't already the last element.
        /// </summary>
        /// <param name="index">Sequential index of the element in the array to move.</param>
        private void OnMoveDownButtonClicked(int index)
        {
            Array array = property.GetValue<Array>();

            if ((index + 1) < array.Length)
            {
                object nextEntry = array.GetValue(index + 1);

                array.SetValue(array.GetValue(index), index + 1);
                array.SetValue(nextEntry, index);
            }
        }

        /// <summary>
        /// Triggered when the user clicks on the create button on the title bar. Creates a brand new array with zero
        /// elements in the place of the current array.
        /// </summary>
        private void OnCreateButtonClicked()
        {
            property.SetValue(property.CreateArrayInstance(new int[1] { 0 }));
        }

        /// <summary>
        /// Triggered when the user clicks on the clear button on the title bar. Deletes the current array and sets
        /// the reference to the array in the parent object to null.
        /// </summary>
        private void OnClearButtonClicked()
        {
            property.SetValue<object>(null);
        }
    }
}
