using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using BansheeEngine;

namespace BansheeEditor
{
    public class InspectableList : InspectableField
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

            public void Refresh(InspectableField child, int seqIndex, InspectableList parent)
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

                GUIButton cloneBtn = new GUIButton("C", GUIOption.FixedWidth(20));
                GUIButton deleteBtn = new GUIButton("X", GUIOption.FixedWidth(20));
                GUIButton moveUpBtn = new GUIButton("U", GUIOption.FixedWidth(20));
                GUIButton moveDownBtn = new GUIButton("D", GUIOption.FixedWidth(20));

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

        private const int IndentAmount = 5;

        private object propertyValue; // TODO - This will unnecessarily hold references to the object
        private int numArrayElements;

        private GUIIntField guiSizeField;
        private GUILayoutX guiChildLayout;
        private GUILayoutX guiTitleLayout;
        private List<EntryRow> rows = new List<EntryRow>();

        private bool forceUpdate = true;
        private bool isExpanded;

        public InspectableList(string title, int depth, InspectableFieldLayout layout, SerializableProperty property)
            : base(title, depth, layout, property)
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

            SerializableList list = property.GetList();
            if (list.GetLength() != numArrayElements)
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
                InspectableField child = GetChild(i);
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

            if (property.Type != SerializableProperty.FieldType.List)
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
                guiTitleLayout.AddElement(new GUILabel("Empty", GUIOption.FixedWidth(100)));

                if (!property.IsValueType)
                {
                    GUIButton createBtn = new GUIButton("Cr", GUIOption.FixedWidth(20));
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
                GUIButton guiResizeBtn = new GUIButton("R", GUIOption.FixedWidth(20));
                guiResizeBtn.OnClick += OnResizeButtonClicked;
                GUIButton guiClearBtn = new GUIButton("Cl", GUIOption.FixedWidth(20));
                guiClearBtn.OnClick += OnClearButtonClicked;

                guiTitleLayout = layout.AddLayoutX(layoutIndex);
                guiTitleLayout.AddElement(guiFoldout);
                guiTitleLayout.AddElement(guiSizeField);
                guiTitleLayout.AddElement(guiResizeBtn);
                guiTitleLayout.AddElement(guiClearBtn);

                SerializableList list = property.GetList();
                numArrayElements = list.GetLength();
                guiSizeField.Value = numArrayElements;

                if (isExpanded)
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
                    string bgPanelStyle = depth % 2 == 0 ? EditorStyles.InspectorContentBgAlternate : EditorStyles.InspectorContentBg;
                    GUIPanel backgroundPanel = guiContentPanel.AddPanel(backgroundDepth);
                    GUITexture inspectorContentBg = new GUITexture(null, bgPanelStyle);
                    backgroundPanel.AddElement(inspectorContentBg);

                    for (int i = 0; i < numArrayElements; i++)
                    {
                        EntryRow newRow = new EntryRow(guiContentLayout);
                        rows.Add(newRow);

                        InspectableField childObj = CreateInspectable(i + ".", depth + 1, new InspectableFieldLayout(newRow.contentLayout), list.GetProperty(i));
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
