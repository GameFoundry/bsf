using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using BansheeEngine;

namespace BansheeEditor
{
    /// <summary>
    /// Base class for objects that display GUI for a modifyable list of elements. Elements can be added, removed and moved.
    /// </summary>
    public abstract class GUIListBase
    {
        private const int IndentAmount = 5;

        protected IList list;
        protected Type listType;

        protected List<GUIListRow> rows = new List<GUIListRow>();
        protected GUIIntField guiSizeField;
        protected GUILayoutX guiChildLayout;
        protected GUILayoutX guiTitleLayout;
        protected bool isExpanded;

        /// <summary>
        /// Constructs a new GUI list.
        /// </summary>
        protected GUIListBase()
        { }

        /// <summary>
        /// Constructs a new GUI list with the specified row types. Must be called right after the constructor.
        /// </summary>
        /// <typeparam name="T">Type of rows that are used to handle GUI for individual list elements.</typeparam>
        /// <param name="title">Label to display on the list GUI title.</param>
        /// <param name="list">Object containing the list data. Can be null.</param>
        /// <param name="listType">Type of the <paramref name="list"/> parameter. Needs to be specified in case that
        ///                        parameter is null.</param>
        /// <param name="layout">Layout to which to append the list GUI elements to.</param>
        protected void Construct<T>(LocString title, IList list, Type listType, GUILayout layout) where T : GUIListRow, new()
        {
            this.list = list;
            this.listType = listType;

            if (list == null)
            {
                guiChildLayout = null;
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
                guiSizeField = new GUIIntField("", GUIOption.FixedWidth(50));
                guiSizeField.SetRange(0, int.MaxValue);

                GUIContent resizeIcon = new GUIContent(EditorBuiltin.GetInspectorWindowIcon(InspectorWindowIcon.Resize));
                GUIButton guiResizeBtn = new GUIButton(resizeIcon, GUIOption.FixedWidth(30));
                guiResizeBtn.OnClick += OnResizeButtonClicked;

                GUIContent clearIcon = new GUIContent(EditorBuiltin.GetInspectorWindowIcon(InspectorWindowIcon.Clear));
                GUIButton guiClearBtn = new GUIButton(clearIcon, GUIOption.FixedWidth(30));
                guiClearBtn.OnClick += OnClearButtonClicked;

                guiTitleLayout = layout.AddLayoutX();
                guiTitleLayout.AddElement(guiFoldout);
                guiTitleLayout.AddElement(guiSizeField);
                guiTitleLayout.AddElement(guiResizeBtn);
                guiTitleLayout.AddElement(guiClearBtn);

                guiSizeField.Value = list.Count;

                if (list.Count > 0)
                {
                    guiChildLayout = layout.AddLayoutX();
                    guiChildLayout.AddSpace(IndentAmount);
                    guiChildLayout.Enabled = isExpanded;

                    GUIPanel guiContentPanel = guiChildLayout.AddPanel();
                    GUILayoutX guiIndentLayoutX = guiContentPanel.AddLayoutX();
                    guiIndentLayoutX.AddSpace(IndentAmount);
                    GUILayoutY guiIndentLayoutY = guiIndentLayoutX.AddLayoutY();
                    guiIndentLayoutY.AddSpace(IndentAmount);
                    GUILayoutY guiContentLayout = guiIndentLayoutY.AddLayoutY();
                    guiIndentLayoutY.AddSpace(IndentAmount);
                    guiIndentLayoutX.AddSpace(IndentAmount);
                    guiChildLayout.AddSpace(IndentAmount);

                    GUIPanel backgroundPanel = guiContentPanel.AddPanel(Inspector.START_BACKGROUND_DEPTH);
                    GUITexture inspectorContentBg = new GUITexture(null, EditorStyles.InspectorContentBg);
                    backgroundPanel.AddElement(inspectorContentBg);

                    for (int i = 0; i < list.Count; i++)
                    {
                        GUIListRow newRow = new T();
                        newRow.Update(this, guiContentLayout, i);

                        rows.Add(newRow);
                    }
                }
            }
        }

        /// <summary>
        /// Gets a value of an element at the specified index in the list.
        /// </summary>
        /// <param name="seqIndex">Sequential index of the element whose value to retrieve.</param>
        /// <returns>Value of the list element at the specified index.</returns>
        protected internal abstract object GetValue(int seqIndex);

        /// <summary>
        /// Sets a value of an element at the specified index in the list.
        /// </summary>
        /// <param name="seqIndex">Sequential index of the element whose value to set.</param>
        /// <param name="value">Value to assign to the element. Caller must ensure it is of valid type.</param>
        protected internal abstract void SetValue(int seqIndex, object value);

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
        /// Triggered when the user clicks on the create button on the title bar. Creates a brand new list with zero
        /// elements in the place of the current list.
        /// </summary>
        protected abstract void OnCreateButtonClicked();

        /// <summary>
        /// Triggered when the user clicks on the resize button on the title bar. Changes the size of the list while
        /// preserving existing contents.
        /// </summary>
        protected abstract void OnResizeButtonClicked();

        /// <summary>
        /// Triggered when the user clicks on the clear button on the title bar. Deletes the current list object.
        /// </summary>
        protected abstract void OnClearButtonClicked();

        /// <summary>
        /// Triggered when the user clicks on the delete button next to the list entry. Deletes an element in the list.
        /// </summary>
        /// <param name="index">Sequential index of the element in the list to remove.</param>
        protected internal abstract void OnDeleteButtonClicked(int index);

        /// <summary>
        /// Triggered when the user clicks on the clone button next to the list entry. Clones an element in the list and
        /// adds the clone to the back of the list. Non-value types must implement the <see cref="ICloneable"/> interface 
        /// in order to be cloned. If it doesn't the clone will point to a null reference.
        /// </summary>
        /// <param name="index">Sequential index of the element in the list to clone.</param>
        protected internal abstract void OnCloneButtonClicked(int index);

        /// <summary>
        /// Triggered when the user clicks on the move up button next to the list entry. Moves an element from the current
        /// list index to the one right before it, if not at zero.
        /// </summary>
        /// <param name="index">Sequential index of the element in the list to move.</param>
        protected internal abstract void OnMoveUpButtonClicked(int index);

        /// <summary>
        /// Triggered when the user clicks on the move down button next to the list entry. Moves an element from the current
        /// list index to the one right after it, if the element isn't already the last element.
        /// </summary>
        /// <param name="index">Sequential index of the element in the list to move.</param>
        protected internal abstract void OnMoveDownButtonClicked(int index);
    }

    /// <summary>
    /// Creates GUI elements that allow viewing and manipulation of a <see cref="System.Array"/>. When constructing the
    /// object user can provide a custom type that manages GUI for individual array elements.
    /// </summary>
    public class GUIArray : GUIListBase
    {
        /// <summary>
        /// Triggered when the reference array has been changed. This does not include changes that only happen to its 
        /// internal elements.
        /// </summary>
        public Action<Array> OnChanged;

        /// <summary>
        /// Triggered when an element in the array has been changed.
        /// </summary>
        public Action OnValueChanged;

        /// <summary>
        /// Array object whose contents are displayed.
        /// </summary>
        public Array Array { get { return (Array)list; } }

        /// <summary>
        /// Constructs a new GUI array.
        /// </summary>
        private GUIArray()
        { }

        /// <summary>
        /// Creates a new GUI array.
        /// </summary>
        /// <typeparam name="RowType">Type of rows that are used to handle GUI for individual list elements.</typeparam>
        /// <typeparam name="ElementType">Type of elements stored in the array.</typeparam>
        /// <param name="title">Label to display on the list GUI title.</param>
        /// <param name="array">Object containing the list data. Cannot be null.</param>
        /// <param name="layout">Layout to which to append the list GUI elements to.</param>
        public static GUIArray Create<RowType, ElementType>(LocString title, ElementType[] array, GUILayout layout) 
            where RowType : GUIListRow, new() 
        {
            GUIArray newArray = new GUIArray();
            newArray.Construct<RowType>(title, array, typeof(ElementType[]), layout);

            return newArray;
        }

        /// <summary>
        /// Refreshes contents of all array rows and checks if anything was modified.
        /// </summary>
        /// <returns>True if any entry in the array was modified, false otherwise.</returns>
        public bool Refresh()
        {
            bool anythingModified = false;

            for (int i = 0; i < rows.Count; i++)
                anythingModified |= rows[i].Refresh();

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

        /// <inheritdoc/>
        protected override void OnCreateButtonClicked()
        {
            list = Array.CreateInstance(listType.GetElementType(), 0);

            if (OnChanged != null)
                OnChanged((Array)list);
        }

        /// <inheritdoc/>
        protected override void OnResizeButtonClicked()
        {
            int size = guiSizeField.Value;

            Array newArray = Array.CreateInstance(listType.GetElementType(), size);

            int maxSize = MathEx.Min(size, list.Count);

            for (int i = 0; i < maxSize; i++)
                newArray.SetValue(list[i], i);

            list = newArray;

            if(OnChanged != null)
                OnChanged((Array)list);
        }

        /// <inheritdoc/>
        protected override void OnClearButtonClicked()
        {
            list = null;

            if (OnChanged != null)
                OnChanged((Array)list);
        }

        /// <inheritdoc/>
        protected internal override object GetValue(int seqIndex)
        {
            return list[seqIndex];
        }

        /// <inheritdoc/>
        protected internal override void SetValue(int seqIndex, object value)
        {
            list[seqIndex] = value;

            if (OnValueChanged != null)
                OnValueChanged();
        }

        /// <inheritdoc/>
        protected internal override void OnDeleteButtonClicked(int index)
        {
            int size = MathEx.Max(0, list.Count - 1);
            Array newArray = Array.CreateInstance(listType.GetElementType(), size);

            int destIdx = 0;
            for (int i = 0; i < list.Count; i++)
            {
                if (i == index)
                    continue;

                newArray.SetValue(list[i], destIdx);
                destIdx++;
            }

            list = newArray;

            if (OnChanged != null)
                OnChanged((Array)list);
        }

        /// <inheritdoc/>
        protected internal override void OnCloneButtonClicked(int index)
        {
            int size = list.Count + 1;
            Array newArray = Array.CreateInstance(listType.GetElementType(), size);

            object clonedEntry = null;
            for (int i = 0; i < list.Count; i++)
            {
                object value = list[i];
                newArray.SetValue(value, i);

                if (i == index)
                {
                    if (value == null)
                        clonedEntry = null;
                    else
                    {
                        ValueType valueType = value as ValueType;
                        if (valueType != null)
                            clonedEntry = valueType;
                        else
                        {
                            ICloneable cloneable = value as ICloneable;

                            if (cloneable != null)
                                clonedEntry = cloneable.Clone();
                            else
                                clonedEntry = null;
                        }
                    }
                }
            }

            newArray.SetValue(clonedEntry, size - 1);

            list = newArray;

            if (OnChanged != null)
                OnChanged((Array)list);
        }

        /// <inheritdoc/>
        protected internal override void OnMoveUpButtonClicked(int index)
        {
            if ((index - 1) >= 0)
            {
                object previousEntry = list[index - 1];

                list[index - 1] = list[index];
                list[index] = previousEntry;

                if (OnValueChanged != null)
                    OnValueChanged();
            }
        }

        /// <inheritdoc/>
        protected internal override void OnMoveDownButtonClicked(int index)
        {
            if ((index + 1) < list.Count)
            {
                object nextEntry = list[index + 1];

                list[index + 1] = list[index];
                list[index] = nextEntry;

                if (OnValueChanged != null)
                    OnValueChanged();
            }
        }
    }

    /// <summary>
    /// Contains GUI elements for a single entry in a list.
    /// </summary>
    public abstract class GUIListRow
    {
        private GUILayoutX rowLayout;
        private GUIListBase parent;

        protected int seqIndex;

        /// <summary>
        /// Constructs a new array row object.
        /// </summary>
        protected GUIListRow()
        {

        }

        /// <summary>
        /// Recreates all row GUI elements.
        /// </summary>
        /// <param name="parent">Parent array GUI object that the entry is contained in.</param>
        /// <param name="parentLayout">Parent layout that row GUI elements will be added to.</param>
        /// <param name="seqIndex">Sequential index of the array entry.</param>
        public void Update(GUIListBase parent, GUILayout parentLayout, int seqIndex)
        {
            this.parent = parent;
            this.seqIndex = seqIndex;

            if (rowLayout != null)
            {
                rowLayout.Destroy();
                rowLayout = null;
            }

            rowLayout = parentLayout.AddLayoutX();
            GUILayoutY contentLayout = rowLayout.AddLayoutY();

            GUILayoutX titleLayout = CreateGUI(contentLayout);

            if (titleLayout == null)
            {
                GUILayoutY buttonCenter = rowLayout.AddLayoutY();
                buttonCenter.AddFlexibleSpace();
                titleLayout = buttonCenter.AddLayoutX();
                buttonCenter.AddFlexibleSpace();
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
        /// Creates GUI elements specific to type in the array row.
        /// </summary>
        /// <param name="layout">Layout to insert the row GUI elements to.</param>
        /// <returns>An optional title bar layout that the standard array buttons will be appended to.</returns>
        protected abstract GUILayoutX CreateGUI(GUILayoutY layout);
        
        /// <summary>
        /// Refreshes the GUI for the list row and checks if anything was modified.
        /// </summary>
        /// <returns>True if any modifications were made, false otherwise.</returns>
        internal protected virtual bool Refresh()
        {
            return false;
        }

        /// <summary>
        /// Gets the value contained in this list row.
        /// </summary>
        /// <typeparam name="T">Type of the value. Must match the list's element type.</typeparam>
        /// <returns>Value in this list row.</returns>
        protected T GetValue<T>()
        {
            return (T)parent.GetValue(seqIndex);
        }

        /// <summary>
        /// Sets the value contained in this list row.
        /// </summary>
        /// <typeparam name="T">Type of the value. Must match the list's element type.</typeparam>
        /// <param name="value">Value to set.</param>
        protected void SetValue<T>(T value)
        {
            parent.SetValue(seqIndex, value);
        }

        /// <summary>
        /// Destroys all row GUI elements.
        /// </summary>
        public void Destroy()
        {
            rowLayout.Destroy();
        }
    }
}
