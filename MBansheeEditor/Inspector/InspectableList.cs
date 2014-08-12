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

        private object oldPropertyValue; // TODO - This will unnecessarily hold references to the object
        private int numArrayElements;

        private GUILabel guiLabel;
        private GUIIntField guiSizeField;
        private GUIButton guiResizeBtn;

        private GUILayout guiTitleLayout;
        private GUILayout guiChildLayout;
        private GUILayoutY guiContentLayout;

        private List<EntryRow> rows = new List<EntryRow>();

        private bool isInitialized;

        public InspectableList(string title, InspectableFieldLayout layout, SerializableProperty property)
            : base(title, layout, property)
        {

        }

        private void Initialize(int layoutIndex)
        {
            if (property.Type != SerializableProperty.FieldType.List)
                return;

            guiLabel = new GUILabel(title); // TODO - Add foldout and hook up its callbacks
            guiSizeField = new GUIIntField();
            guiSizeField.SetRange(0, int.MaxValue);
            guiResizeBtn = new GUIButton("Resize");
            guiResizeBtn.OnClick += OnResizeButtonClicked;

            guiTitleLayout = layout.AddLayoutX(layoutIndex);
            guiTitleLayout.AddElement(guiLabel);
            guiTitleLayout.AddElement(guiSizeField);
            guiTitleLayout.AddElement(guiResizeBtn);

            guiChildLayout = layout.AddLayoutX(layoutIndex);

            guiChildLayout.AddSpace(IndentAmount);
            guiContentLayout = guiChildLayout.AddLayoutY();

            isInitialized = true;
        }

        protected override bool IsModified()
        {
            if (!isInitialized)
                return true;

            object newPropertyValue = property.GetValue<object>();
            if (oldPropertyValue != newPropertyValue)
            {
                oldPropertyValue = newPropertyValue;

                return true;
            }

            SerializableList list = property.GetList();
            if (list.GetLength() != numArrayElements)
                return true;

            return base.IsModified();
        }

        protected override void Update(int layoutIndex)
        {
            base.Update(layoutIndex);

            if (!isInitialized)
                Initialize(layoutIndex);

            foreach (var row in rows)
                row.Destroy();

            rows.Clear();

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
    }
}
