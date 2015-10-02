using System;
using System.Collections;
using System.Collections.Generic;
using BansheeEngine;

namespace BansheeEditor
{
    /// <summary>
    /// Base class for objects that display GUI for a modifyable dictionary of elements. Elements can be added, modified or
    /// removed.
    /// </summary>
    public abstract class GUIDictionaryFieldBase
    {
        private const int IndentAmount = 5;

        protected List<GUIDictionaryFieldRow> rows = new List<GUIDictionaryFieldRow>();
        protected GUILayoutX guiChildLayout;
        protected GUILayoutX guiTitleLayout;
        protected GUILayoutY guiContentLayout;
        protected bool isExpanded;
        protected int depth;

        /// <summary>
        /// Constructs a new GUI dictionary.
        /// </summary>
        protected GUIDictionaryFieldBase()
        { }

        /// <summary>
        /// Updates the GUI dictionary contents. Must be called at least once in order for the contents to be populated.
        /// </summary>
        /// <typeparam name="T">Type of rows that are used to handle GUI for individual dictionary elements.</typeparam>
        /// <param name="title">Label to display on the dictionary GUI title.</param>
        /// <param name="empty">Should the created field represent a null object.</param>
        /// <param name="numRows">Number of rows to create GUI for. Only matters for a non-null dictionary.</param>
        /// <param name="layout">Layout to which to append the list GUI elements to.</param>
        /// <param name="depth">Determines at which depth to render the background. Useful when you have multiple
        ///                     nested containers whose backgrounds are overlaping. Also determines background style,
        ///                     depths divisible by two will use an alternate style.</param>
        protected void Update<T>(LocString title, bool empty, int numRows, GUILayout layout,
            int depth = 0) where T : GUIDictionaryFieldRow, new()
        {
            Destroy();

            this.depth = depth;

            if (empty)
            {
                guiChildLayout = null;
                guiContentLayout = null;
                guiTitleLayout = layout.AddLayoutX();

                guiTitleLayout.AddElement(new GUILabel(title));
                guiTitleLayout.AddElement(new GUILabel("Empty", GUIOption.FixedWidth(100)));

                GUIContent createIcon = new GUIContent(EditorBuiltin.GetInspectorWindowIcon(InspectorWindowIcon.Create));
                GUIButton createBtn = new GUIButton(createIcon, GUIOption.FixedWidth(30));
                createBtn.OnClick += OnCreateButtonClicked;
                guiTitleLayout.AddElement(createBtn);
            }
            else
            {
                GUIToggle guiFoldout = new GUIToggle(title, EditorStyles.Foldout);
                guiFoldout.Value = isExpanded;
                guiFoldout.OnToggled += OnFoldoutToggled;

                GUIContent clearIcon = new GUIContent(EditorBuiltin.GetInspectorWindowIcon(InspectorWindowIcon.Clear));
                GUIButton guiClearBtn = new GUIButton(clearIcon, GUIOption.FixedWidth(30));
                guiClearBtn.OnClick += OnClearButtonClicked;

                guiTitleLayout = layout.AddLayoutX();
                guiTitleLayout.AddElement(guiFoldout);
                guiTitleLayout.AddElement(guiClearBtn);

                if (numRows > 0)
                {
                    guiChildLayout = layout.AddLayoutX();
                    guiChildLayout.AddSpace(IndentAmount);
                    guiChildLayout.Enabled = isExpanded;

                    GUIPanel guiContentPanel = guiChildLayout.AddPanel();
                    GUILayoutX guiIndentLayoutX = guiContentPanel.AddLayoutX();
                    guiIndentLayoutX.AddSpace(IndentAmount);
                    GUILayoutY guiIndentLayoutY = guiIndentLayoutX.AddLayoutY();
                    guiIndentLayoutY.AddSpace(IndentAmount);
                    guiContentLayout = guiIndentLayoutY.AddLayoutY();
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

                    for (int i = 0; i < numRows; i++)
                    {
                        GUIDictionaryFieldRow newRow = new T();
                        newRow.BuildGUI(this, guiContentLayout, i, depth);

                        rows.Add(newRow);
                    }
                }
            }
        }

        /// <summary>
        /// Returns the layout that is used for positioning the elements in the title bar.
        /// </summary>
        /// <returns>Horizontal layout for positioning the title bar elements.</returns>
        public GUILayoutX GetTitleLayout()
        {
            return guiTitleLayout;
        }

        /// <summary>
        /// Refreshes contents of all dictionary rows and checks if anything was modified.
        /// </summary>
        /// <returns>True if any entry in the list was modified, false otherwise.</returns>
        public bool Refresh()
        {
            bool anythingModified = false;

            for (int i = 0; i < rows.Count; i++)
            {
                bool updateGUI;

                anythingModified |= rows[i].Refresh(out updateGUI);

                if (updateGUI)
                    rows[i].BuildGUI(this, guiContentLayout, i, depth);
            }

            return anythingModified;
        }

        /// <summary>
        /// Destroys the GUI elements.
        /// </summary>
        public void Destroy()
        {
            if (guiTitleLayout != null)
            {
                guiTitleLayout.Destroy();
                guiTitleLayout = null;
            }

            if (guiChildLayout != null)
            {
                guiChildLayout.Destroy();
                guiChildLayout = null;
            }

            for (int i = 0; i < rows.Count; i++)
                rows[i].Destroy();

            rows.Clear();
        }

        /// <summary>
        /// Gets a value of an element at the specified index in the list.
        /// </summary>
        /// <param name="key">Key of the element whose value to retrieve.</param>
        /// <returns>Value of the list element at the specified key.</returns>
        protected internal abstract object GetValue(object key);

        /// <summary>
        /// Sets a value of an element at the specified index in the list.
        /// </summary>
        /// <param name="key">Key of the element whose value to set.</param>
        /// <param name="value">Value to assign to the element. Caller must ensure it is of valid type.</param>
        protected internal abstract void SetValue(object key, object value);

        /// <summary>
        /// Checks does the element with the specified key exist in the dictionary.
        /// </summary>
        /// <param name="key">Key of the element to check for existence.</param>
        /// <returns>True if the key exists in the dictionary, false otherwise.</returns>
        protected internal abstract bool Contains(object key);

        /// <summary>
        /// Triggered when the user clicks on the expand/collapse toggle in the title bar.
        /// </summary>
        /// <param name="expanded">Determines whether the contents were expanded or collapsed.</param>
        private void OnFoldoutToggled(bool expanded)
        {
            isExpanded = expanded;

            if (guiChildLayout != null)
                guiChildLayout.Enabled = isExpanded;
        }

        /// <summary>
        /// Triggered when the user clicks on the create button on the title bar. Creates a brand new dictionary with zero
        /// elements in the place of the current dictionary.
        /// </summary>
        protected abstract void OnCreateButtonClicked();

        /// <summary>
        /// Triggered when the user clicks on the clear button on the title bar. Deletes the current dictionary object.
        /// </summary>
        protected abstract void OnClearButtonClicked();

        /// <summary>
        /// Triggered when the user clicks on the delete button next to a dictionary entry. Deletes an element in the 
        /// dictionary.
        /// </summary>
        /// <param name="key">Key of the element to remove.</param>
        protected internal abstract void OnDeleteButtonClicked(object key);

        /// <summary>
        /// Triggered when the user clicks on the clone button next to a dictionary entry. Clones an element and
        /// adds the clone to the dictionary. 
        /// </summary>
        /// <param name="key">Key of the element to clone.</param>
        protected internal abstract void OnCloneButtonClicked(object key);
    }

    /// <summary>
    /// Creates GUI elements that allow viewing and manipulation of a <see cref="Dictionary{TKey,TValue}"/>. When constructing the
    /// object user can provide a custom type that manages GUI for individual dictionary elements.
    /// </summary>
    /// <typeparam name="Key">Type of key used by the dictionary.</typeparam>
    /// <typeparam name="Value">Type of value stored in the dictionary.</typeparam>
    public class GUIDictionaryField<Key, Value> : GUIDictionaryFieldBase
    {
        /// <summary>
        /// Triggered when the reference array has been changed. This does not include changes that only happen to its 
        /// internal elements.
        /// </summary>
        public Action<IDictionary> OnChanged;

        /// <summary>
        /// Triggered when an element in the list has been changed.
        /// </summary>
        public Action OnValueChanged;

        /// <summary>
        /// Array object whose contents are displayed.
        /// </summary>
        public IDictionary Dictionary { get { return dictionary; } }

        protected IDictionary dictionary;
        protected Type keyType;
        protected Type valueType;

        /// <summary>
        /// Constructs a new empty dictionary GUI.
        /// </summary>
        public GUIDictionaryField()
        { }

        /// <summary>
        /// Updates the GUI dictionary contents. Must be called at least once in order for the contents to be populated.
        /// </summary>
        /// <typeparam name="RowType">Type of rows that are used to handle GUI for individual dictionary elements.</typeparam>
        /// <param name="title">Label to display on the list GUI title.</param>
        /// <param name="dictionary">Object containing the data. Can be null.</param>
        /// <param name="layout">Layout to which to append the list GUI elements to.</param>
        /// <param name="depth">Determines at which depth to render the background. Useful when you have multiple
        ///                     nested containers whose backgrounds are overlaping. Also determines background style,
        ///                     depths divisible by two will use an alternate style.</param>
        public void Update<RowType>(LocString title, Dictionary<Key, Value> dictionary, 
            GUILayout layout, int depth = 0)
            where RowType : GUIDictionaryFieldRow, new()
        {
            this.keyType = typeof(Key);
            this.valueType = typeof(Value);
            this.dictionary = dictionary;

            if (dictionary != null)
                base.Update<RowType>(title, false, dictionary.Count, layout, depth);
            else
                base.Update<RowType>(title, true, 0, layout, depth);
        }

        /// <inheritdoc/>
        protected internal override object GetValue(object key)
        {
            return dictionary[key];
        }

        /// <inheritdoc/>
        protected internal override void SetValue(object key, object value)
        {
            dictionary[key] = value;

            if (OnValueChanged != null)
                OnValueChanged();
        }

        /// <inheritdoc/>
        protected internal override bool Contains(object key)
        {
            return dictionary.Contains(key);;
        }

        /// <inheritdoc/>
        protected override void OnCreateButtonClicked()
        {
            dictionary = new Dictionary<Key, Value>();

            if (OnChanged != null)
                OnChanged(dictionary);
        }

        /// <inheritdoc/>
        protected override void OnClearButtonClicked()
        {
            dictionary = null;

            if (OnChanged != null)
                OnChanged(dictionary);
        }

        /// <inheritdoc/>
        protected internal override void OnDeleteButtonClicked(object key)
        {
            dictionary.Remove(key);

            if (OnValueChanged != null)
                OnValueChanged();
        }

        /// <summary>
        /// Triggered when the user clicks on the clone button next to a dictionary entry. Clones the element and adds the 
        /// clone to the dictionary. Non-value types must implement the <see cref="ICloneable"/> interface in order to be 
        /// cloned. If it doesn't the clone will point to a null reference.
        /// </summary>
        /// <param name="key">Key of the element to clone.</param>
        protected internal override void OnCloneButtonClicked(object key)
        {
            // TODO - Not supported

            //int size = array.GetLength(0) + 1;
            //Array newArray = Array.CreateInstance(arrayType.GetElementType(), size);

            //object clonedEntry = null;
            //for (int i = 0; i < array.GetLength(0); i++)
            //{
            //    object value = array.GetValue(i);
            //    newArray.SetValue(value, i);

            //    if (i == index)
            //    {
            //        if (value == null)
            //            clonedEntry = null;
            //        else
            //        {
            //            ValueType valueType = value as ValueType;
            //            if (valueType != null)
            //                clonedEntry = valueType;
            //            else
            //            {
            //                ICloneable cloneable = value as ICloneable;

            //                if (cloneable != null)
            //                    clonedEntry = cloneable.Clone();
            //                else
            //                    clonedEntry = null;
            //            }
            //        }
            //    }
            //}

            //newArray.SetValue(clonedEntry, size - 1);

            //array = newArray;

            //if (OnChanged != null)
            //    OnChanged(array);
        }
    }

    /// <summary>
    /// Contains GUI elements for a single entry in a dictionary.
    /// </summary>
    public abstract class GUIDictionaryFieldRow
    {
        private GUILayoutX rowLayout;
        private GUILayoutY keyLayout;
        private GUILayoutY valueLayout;
        private GUILayoutX titleLayout;
        private bool localTitleLayout;
        private GUIDictionaryFieldBase parent;

        protected object key;
        protected int depth;

        /// <summary>
        /// Constructs a new dictionary row object.
        /// </summary>
        protected GUIDictionaryFieldRow()
        {

        }

        /// <summary>
        /// (Re)creates all row GUI elements.
        /// </summary>
        /// <param name="parent">Parent array GUI object that the entry is contained in.</param>
        /// <param name="parentLayout">Parent layout that row GUI elements will be added to.</param>
        /// <param name="key">Key of the element to create GUI for.</param>
        /// <param name="depth">Determines the depth at which the element is rendered.</param>
        public void BuildGUI(GUIDictionaryFieldBase parent, GUILayout parentLayout, object key, int depth)
        {
            this.parent = parent;
            this.key = key;
            this.depth = depth;

            if (rowLayout == null)
                rowLayout = parentLayout.AddLayoutX();

            if (keyLayout == null)
                keyLayout = rowLayout.AddLayoutY();

            if (valueLayout == null)
                valueLayout = rowLayout.AddLayoutY();

            CreateKeyGUI(keyLayout);
            GUILayoutX externalTitleLayout = CreateValueGUI(valueLayout);
            if (localTitleLayout || (titleLayout != null && titleLayout == externalTitleLayout))
                return;

            if (externalTitleLayout != null)
            {
                localTitleLayout = false;
                titleLayout = externalTitleLayout;
            }
            else
            {
                GUILayoutY buttonCenter = rowLayout.AddLayoutY();
                buttonCenter.AddFlexibleSpace();
                titleLayout = buttonCenter.AddLayoutX();
                buttonCenter.AddFlexibleSpace();

                localTitleLayout = true;
            }

            GUIContent cloneIcon = new GUIContent(EditorBuiltin.GetInspectorWindowIcon(InspectorWindowIcon.Clone));
            GUIContent deleteIcon = new GUIContent(EditorBuiltin.GetInspectorWindowIcon(InspectorWindowIcon.Delete));

            GUIButton cloneBtn = new GUIButton(cloneIcon, GUIOption.FixedWidth(30));
            GUIButton deleteBtn = new GUIButton(deleteIcon, GUIOption.FixedWidth(30));

            cloneBtn.OnClick += () => parent.OnCloneButtonClicked(key);
            deleteBtn.OnClick += () => parent.OnDeleteButtonClicked(key);

            titleLayout.AddElement(cloneBtn);
            titleLayout.AddElement(deleteBtn);
        }

        /// <summary>
        /// Creates GUI elements specific to type in the key portion of a dictionary entry.
        /// </summary>
        /// <param name="layout">Layout to insert the row GUI elements to.</param>
        protected abstract void CreateKeyGUI(GUILayoutY layout);

        /// <summary>
        /// Creates GUI elements specific to type in the key portion of a dictionary entry.
        /// </summary>
        /// <param name="layout">Layout to insert the row GUI elements to.</param>
        /// <returns>An optional title bar layout that the standard dictionary buttons will be appended to.</returns>
        protected abstract GUILayoutX CreateValueGUI(GUILayoutY layout);

        /// <summary>
        /// Refreshes the GUI for the dictionary row and checks if anything was modified.
        /// </summary>
        /// <param name="rebuildGUI">Determines should the field's GUI elements be updated due to modifications.</param>
        /// <returns>True if any modifications were made, false otherwise.</returns>
        internal protected virtual bool Refresh(out bool rebuildGUI)
        {
            rebuildGUI = false;
            return false;
        }

        /// <summary>
        /// Gets the value contained in this dictionary's row.
        /// </summary>
        /// <typeparam name="T">Type of the value. Must match the dictionary's element type.</typeparam>
        /// <returns>Value in this dictionary's row.</returns>
        protected T GetValue<T>()
        {
            return (T)parent.GetValue(key);
        }

        /// <summary>
        /// Sets the value contained in this dictionary's row.
        /// </summary>
        /// <typeparam name="T">Type of the value. Must match the dictionary's element type.</typeparam>
        /// <param name="value">Value to set.</param>
        protected void SetValue<T>(T value)
        {
            parent.SetValue(key, value);
        }

        /// <summary>
        /// Destroys all row GUI elements.
        /// </summary>
        public void Destroy()
        {
            rowLayout.Destroy();
            rowLayout = null;
        }
    }
}