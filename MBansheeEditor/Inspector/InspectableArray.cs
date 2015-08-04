using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using BansheeEngine;

namespace BansheeEditor
{
    public class InspectableArray : InspectableObjectBase
    {
        private class EntryRow
        {
            public GUILayoutY contentLayout;
            private GUILayoutX rowLayout;
            private GUILayoutX titleLayout;
            private bool ownsTitleLayout;

            public EntryRow(GUILayout parentLayout)
            {
                rowLayout = parentLayout.AddLayoutX();
                contentLayout = rowLayout.AddLayoutY();
            }

            public void Refresh(InspectableObjectBase child, int seqIndex, InspectableArray parent)
            {
                if (ownsTitleLayout)
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

                GUIButton cloneBtn = new GUIButton("C");
                GUIButton deleteBtn = new GUIButton("X");
                GUIButton moveUpBtn = new GUIButton("Up");
                GUIButton moveDownBtn = new GUIButton("Down");

                cloneBtn.OnClick += () => parent.OnCloneButtonClicked(seqIndex);
                deleteBtn.OnClick += () => parent.OnDeleteButtonClicked(seqIndex);
                moveUpBtn.OnClick += () => parent.OnMoveUpButtonClicked(seqIndex);
                moveDownBtn.OnClick += () => parent.OnMoveDownButtonClicked(seqIndex);

                titleLayout.AddElement(cloneBtn);
                titleLayout.AddElement(deleteBtn);
                titleLayout.AddElement(moveUpBtn);
                titleLayout.AddElement(moveDownBtn);
            }

            public void Destroy()
            {
                rowLayout.Destroy();
            }
        }

        private const int IndentAmount = 15;

        private object propertyValue; // TODO - This will unnecessarily hold references to the object
        private int numArrayElements;

        private List<EntryRow> rows = new List<EntryRow>();
        private GUIIntField guiSizeField;
        private GUILayoutX guiChildLayout;
        private GUILayoutX guiTitleLayout;
        private bool isExpanded;

        private bool forceUpdate = true;

        public InspectableArray(string title, InspectableFieldLayout layout, SerializableProperty property)
            : base(title, layout, property)
        {

        }

        public override GUILayoutX GetTitleLayout()
        {
            return guiTitleLayout;
        }

        protected override bool IsModified()
        {
            if (forceUpdate)
                return true;

            object newPropertyValue = property.GetValue<object>();
            if (propertyValue == null)
                return newPropertyValue != null;

            if (newPropertyValue == null)
                return propertyValue != null;

            if (!propertyValue.Equals(newPropertyValue))
                return true;

            SerializableArray array = property.GetArray();
            if (array.GetLength() != numArrayElements)
                return true;

            return base.IsModified();
        }

        public override bool Refresh(int layoutIndex)
        {
            bool anythingModified = false;

            if (IsModified())
            {
                Update(layoutIndex);
                anythingModified = true;
            }

            for (int i = 0; i < GetChildCount(); i++)
            {
                InspectableObjectBase child = GetChild(i);
                bool childModified = child.Refresh(0);

                if (childModified)
                    rows[i].Refresh(child, i, this);

                anythingModified |= childModified;
            }

            return anythingModified;
        }

        protected override void Update(int layoutIndex)
        {
            base.Update(layoutIndex);
            forceUpdate = false;
            guiTitleLayout = null;

            if (property.Type != SerializableProperty.FieldType.Array || property.InternalType.GetArrayRank() != 1) // We don't support multirank arrays
                return;

            foreach (var row in rows)
                row.Destroy();

            rows.Clear();
            layout.DestroyElements();

            propertyValue = property.GetValue<object>();
            if (propertyValue == null)
            {
                guiChildLayout = null;
                guiTitleLayout = layout.AddLayoutX(layoutIndex);

                guiTitleLayout.AddElement(new GUILabel(title));
                guiTitleLayout.AddElement(new GUILabel("Empty"));

                if (!property.IsValueType)
                {
                    GUIButton createBtn = new GUIButton("Create");
                    createBtn.OnClick += OnCreateButtonClicked;
                    guiTitleLayout.AddElement(createBtn);
                }

                numArrayElements = 0;
            }
            else
            {
                GUIFoldout guiFoldout = new GUIFoldout(title);
                guiFoldout.SetExpanded(isExpanded);
                guiFoldout.OnToggled += OnFoldoutToggled;
                guiSizeField = new GUIIntField();
                guiSizeField.SetRange(0, int.MaxValue);
                GUIButton guiResizeBtn = new GUIButton("Resize");
                guiResizeBtn.OnClick += OnResizeButtonClicked;
                GUIButton guiClearBtn = new GUIButton("Clear");
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
                    guiChildLayout = layout.AddLayoutX(layoutIndex);
                    guiChildLayout.AddSpace(IndentAmount);

                    GUILayoutY guiContentLayout = guiChildLayout.AddLayoutY();

                    for (int i = 0; i < numArrayElements; i++)
                    {
                        EntryRow newRow = new EntryRow(guiContentLayout);
                        rows.Add(newRow);

                        InspectableObjectBase childObj = CreateDefaultInspectable(i + ".", new InspectableFieldLayout(newRow.contentLayout), array.GetProperty(i));
                        AddChild(childObj);

                        childObj.Refresh(0);
                        rows[i].Refresh(childObj, i, this);
                    }
                }
                else
                    guiChildLayout = null;
            }
        }

        private void OnFoldoutToggled(bool expanded)
        {
            isExpanded = expanded;
            forceUpdate = true;
        }

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

        private void OnCreateButtonClicked()
        {
            property.SetValue(property.CreateArrayInstance(new int[1] { 0 }));
        }

        private void OnClearButtonClicked()
        {
            property.SetValue<object>(null);
        }
    }
}
