using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using BansheeEngine;

namespace BansheeEditor
{
    public class InspectableInt : InspectableObjectBase
    {
        private int oldPropertyValue;
        private GUILabel guiLabel;
        private bool isInitialized = false;

        public InspectableInt(string title, SerializableProperty property)
            :base(title, property)
        {

        }

        protected void Initialize(GUILayout layout)
        {
            if(property.Type == SerializableField.FieldType.Int)
            {
                guiLabel = new GUILabel(title);
                layout.AddElement(guiLabel); // TODO - Use an IntEditorField
            }

            isInitialized = true;
        }

        protected override bool IsModified()
        {
            int newPropertyValue = property.GetValue<int>();
            if (oldPropertyValue != newPropertyValue)
            {
                oldPropertyValue = newPropertyValue;

                return true;
            }

            return base.IsModified();
        }

        protected override void Update(GUILayout layout)
        {
            base.Update(layout);

            if (!isInitialized)
                Initialize(layout);

            // TODO - Update GUI element(s) with value from property
        }

        public override void Destroy()
        {
            if (guiLabel != null)
                guiLabel.Destroy();

            base.Destroy();
        }
    }
}
