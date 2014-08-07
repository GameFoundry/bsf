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
            public GUILayoutX rowLayout;
            public GUILayoutY contentLayout;
            public GUIButton cloneBtn;
            public GUIButton deleteBtn;
            public GUIButton moveUpBtn;
            public GUIButton moveDownBtn;

            public EntryRow(GUILayout parentLayout, int seqIndex, InspectableArray parent)
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

        public InspectableArray(string title, InspectableFieldLayout layout, SerializableProperty property)
            : base(title, layout, property)
        {

        }

        private void Initialize(int layoutIndex)
        {
            if (property.Type != SerializableProperty.FieldType.Array)
                return;

            guiLabel = new GUILabel(title); // TODO - Add foldout and hook up its callbacks
            guiSizeField = new GUIIntField();
            guiSizeField.Value = property.GetArray().GetLength();
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

            SerializableArray array = property.GetArray();
            if (array.GetLength() != numArrayElements)
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

            SerializableArray array = property.GetArray();

            numArrayElements = array.GetLength();
            for (int i = 0; i < numArrayElements; i++)
            {
                EntryRow newRow = new EntryRow(guiContentLayout, i, this);
                rows.Add(newRow);

                InspectableObjectBase childObj = CreateDefaultInspectable(i + ".", new InspectableFieldLayout(newRow.contentLayout), array.GetProperty(i));
                AddChild(childObj);

                childObj.Refresh(0);
            }
        }

        private void OnResizeButtonClicked()
        {
            int size = guiSizeField.Value; // TODO - Support multi-rank arrays

            Array newArray = property.CreateArrayInstance(new int[] {size});
            SerializableArray array = property.GetArray();

            int maxSize = MathEx.Min(size, array.GetLength());

            for (int i = 0; i < maxSize; i++)
                newArray.SetValue(array.GetProperty(i).GetValue<object>(), i);

            property.SetValue(newArray);
        }

        private void OnDeleteButtonClicked(int index)
        {
            SerializableArray array = property.GetArray();

            int size = MathEx.Max(0, array.GetLength() - 1);
            Array newArray = property.CreateArrayInstance(new int[] { size });

            int destIdx = 0;
            for (int i = 0; i < array.GetLength(); i++)
            {
                if (i == index)
                    continue;

                newArray.SetValue(array.GetProperty(i).GetValue<object>(), destIdx);
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
            SerializableArray array = property.GetArray();

            if ((index - 1) >= 0)
            {
                object previousEntry = array.GetProperty(index - 1).GetValue<object>();

                array.GetProperty(index - 1).SetValue(array.GetProperty(index).GetValue<object>());
                array.GetProperty(index).SetValue(previousEntry);
            }
        }

        private void OnMoveDownButtonClicked(int index)
        {
            SerializableArray array = property.GetArray();

            if ((index + 1) < array.GetLength())
            {
                object nextEntry = array.GetProperty(index + 1).GetValue<object>();

                array.GetProperty(index + 1).SetValue(array.GetProperty(index).GetValue<object>());
                array.GetProperty(index).SetValue(nextEntry);
            }
        }
    }
}
