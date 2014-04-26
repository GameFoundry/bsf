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

        private object oldPropertyValue;
        private List<InspectableObjectBase> arrayElements = new List<InspectableObjectBase>();

        private GUILabel guiLabel;
        private GUILayout guiChildLayout;
        private GUILayout guiContentLayout;
        private bool isInitialized;

        public InspectableArray(string title, SerializableProperty property)
            : base(title, property)
        {

        }

        protected void Initialize(GUILayout layout)
        {
            if (property.Type != SerializableProperty.FieldType.Array)
                return;

            guiLabel = new GUILabel(title);
            layout.AddElement(guiLabel); // TODO - Add foldout and hook up its callbacks

            guiChildLayout = layout.AddLayoutX();
            guiChildLayout.AddSpace(IndentAmount);

            guiContentLayout = guiChildLayout.AddLayoutY();

            isInitialized = true;
        }

        protected override bool IsModified()
        {
            object newPropertyValue = property.GetValue<object>();
            if (oldPropertyValue != newPropertyValue)
            {
                oldPropertyValue = newPropertyValue;

                return true;
            }

            SerializableArray array = property.GetArray();
            if (array.GetLength() != arrayElements.Count)
                return true;

            return base.IsModified();
        }

        protected override void Update(GUILayout layout)
        {
            base.Update(layout);

            if (!isInitialized)
                Initialize(layout);

            // TODO - Update base GUI elements

            SerializableArray array = property.GetArray();

            for (int i = arrayElements.Count; i < array.GetLength(); i++)
            {
                InspectableObjectBase childObj = CreateDefaultInspectable(i + ".", array.GetProperty(i));
                AddChild(childObj);
                arrayElements.Add(childObj);

                childObj.Refresh(layout);
            }

            for (int i = array.GetLength(); i < arrayElements.Count; i++)
            {
                arrayElements[i].Destroy();
            }

            arrayElements.RemoveRange(array.GetLength(), arrayElements.Count - array.GetLength());
        }

        public override void Destroy()
        {
            if (guiLabel != null)
                guiLabel.Destroy();

            if (guiContentLayout != null)
                guiContentLayout.Destroy();

            if (guiChildLayout != null)
                guiChildLayout.Destroy();

            base.Destroy();
        }
    }
}
