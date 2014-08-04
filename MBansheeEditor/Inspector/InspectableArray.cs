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
        private const int IndentAmount = 15;

        private object oldPropertyValue; // TODO - This will unnecessarily hold references to the object
        private int numArrayElements;

        private GUILabel guiLabel;
        private GUILayout guiChildLayout;
        private GUILayoutY guiContentLayout;
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
            layout.AddElement(layoutIndex, guiLabel);

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

            SerializableArray array = property.GetArray();

            int childLayoutIndex = 0;
            numArrayElements = array.GetLength();
            for (int i = 0; i < numArrayElements; i++)
            {
                InspectableObjectBase childObj = CreateDefaultInspectable(i + ".", new InspectableFieldLayout(guiContentLayout), array.GetProperty(i));
                AddChild(childObj);

                childObj.Refresh(childLayoutIndex);
                childLayoutIndex += childObj.GetNumLayoutElements();
            }
        }
    }
}
