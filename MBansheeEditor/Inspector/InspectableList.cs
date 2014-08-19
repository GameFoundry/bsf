using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using BansheeEngine;

namespace BansheeEditor
{
    public class InspectableList : InspectableObjectBase
    {
        private class EntryRow
        {
            public GUILayoutX rowLayout;
            public GUILayoutY contentLayout;
            public GUIButton cloneBtn;
            public GUIButton deleteBtn;
            public GUIButton moveUpBtn;
            public GUIButton moveDownBtn;

            public EntryRow(GUILayout parentLayout, int seqIndex, InspectableList parent)
            {
                rowLayout = parentLayout.AddLayoutX();
                contentLayout = rowLayout.AddLayoutY();
                cloneBtn = new GUIButton("C");
                deleteBtn = new GUIButton("X");
                moveUpBtn = new GUIButton("Up");
                moveDownBtn = new GUIButton("Down");

                cloneBtn.OnClick += () => parent.OnCloneButtonClicked(seqIndex);
                deleteBtn.OnClick += () => parent.OnDeleteButtonClicked(seqIndex);
                moveUpBtn.OnClick += () => parent.OnMoveUpButtonClicked(seqIndex);
                moveDownBtn.OnClick += () => parent.OnMoveDownButtonClicked(seqIndex);

                rowLayout.AddElement(cloneBtn);
                rowLayout.AddElement(deleteBtn);
                rowLayout.AddElement(moveUpBtn);
                rowLayout.AddElement(moveDownBtn);
            }

            public void Destroy()
            {
                rowLayout.Destroy();
                contentLayout.Destroy();
                cloneBtn.Destroy();
                deleteBtn.Destroy();
                moveUpBtn.Destroy();
                moveDownBtn.Destroy();
            }
        }

        private const int IndentAmount = 15;

        private object propertyValue; // TODO - This will unnecessarily hold references to the object
        private int numArrayElements;

        private GUIIntField guiSizeField;
        private GUILayoutX guiChildLayout;
        private List<EntryRow> rows = new List<EntryRow>();

        private bool forceUpdate = true;
        private bool isExpanded;

        public InspectableList(string title, InspectableFieldLayout layout, SerializableProperty property)
            : base(title, layout, property)
        {

        }

        protected override bool IsModified()
        {
            if (forceUpdate)
                return true;

            object newPropertyValue = property.GetValue<object>();
            if (propertyValue != newPropertyValue)
                return true;

            if (newPropertyValue != null)
            {
                SerializableList list = property.GetList();
                if (list.GetLength() != numArrayElements)
                    return true;
            }

            return base.IsModified();
        }

        protected override void Update(int layoutIndex)
        {
            base.Update(layoutIndex);
            forceUpdate = false;

            if (property.Type != SerializableProperty.FieldType.List)
                return;

            foreach (var row in rows)
                row.Destroy();

            rows.Clear();

            propertyValue = property.GetValue<object>();
            if (propertyValue == null)
            {
                guiChildLayout = null;
                GUILayoutX guiTitleLayout = layout.AddLayoutX(layoutIndex);

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

                GUILayoutX guiTitleLayout = layout.AddLayoutX(layoutIndex);
                guiTitleLayout.AddElement(guiFoldout);
                guiTitleLayout.AddElement(guiSizeField);
                guiTitleLayout.AddElement(guiResizeBtn);
                guiTitleLayout.AddElement(guiClearBtn);

                guiChildLayout = layout.AddLayoutX(layoutIndex);
                guiChildLayout.SetVisible(isExpanded);
                guiChildLayout.AddSpace(IndentAmount);

                GUILayoutY guiContentLayout = guiChildLayout.AddLayoutY();
                SerializableList list = property.GetList();

                numArrayElements = list.GetLength();
                for (int i = 0; i < numArrayElements; i++)
                {
                    EntryRow newRow = new EntryRow(guiContentLayout, i, this);
                    rows.Add(newRow);

                    InspectableObjectBase childObj = CreateDefaultInspectable(i + ".", new InspectableFieldLayout(newRow.contentLayout), list.GetProperty(i));
                    AddChild(childObj);

                    childObj.Refresh(0);
                }

                guiSizeField.Value = numArrayElements;
            }
        }

        private void OnFoldoutToggled(bool expanded)
        {
            if (guiChildLayout != null)
                guiChildLayout.SetVisible(expanded);

            isExpanded = expanded;
            forceUpdate = true;
        }

        private void OnResizeButtonClicked()
        {
            int size = guiSizeField.Value;

            IList newList = property.CreateListInstance(size);
            IList list = property.GetValue<IList>();

            int maxSize = MathEx.Min(size, list.Count);

            for (int i = 0; i < maxSize; i++)
                newList[i] = list[i];

            property.SetValue(newList);
        }

        private void OnDeleteButtonClicked(int index)
        {
            IList list = property.GetValue<IList>();

            if (index >= 0 && index < list.Count)
                list.RemoveAt(index);
        }

        private void OnCloneButtonClicked(int index)
        {
            SerializableList serializableList = property.GetList();
            IList list = property.GetValue<IList>();

            int size = serializableList.GetLength() + 1;

            if (index >= 0 && index < list.Count)
            {
                list.Add(serializableList.GetProperty(index).GetValueCopy<object>());
            }
        }

        private void OnMoveUpButtonClicked(int index)
        {
            IList list = property.GetValue<IList>();

            if ((index - 1) >= 0)
            {
                object previousEntry = list[index - 1];

                list[index - 1] = list[index];
                list[index] = previousEntry;
            }
        }

        private void OnMoveDownButtonClicked(int index)
        {
            IList list = property.GetValue<IList>();

            if ((index + 1) < list.Count)
            {
                object nextEntry = list[index + 1];

                list[index + 1] = list[index];
                list[index] = nextEntry;
            }
        }

        private void OnCreateButtonClicked()
        {
            property.SetValue(property.CreateListInstance(0));
        }

        private void OnClearButtonClicked()
        {
            property.SetValue<object>(null);
        }
    }
}
