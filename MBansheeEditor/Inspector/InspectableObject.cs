using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using BansheeEngine;

namespace BansheeEditor
{
    public sealed class InspectableObject
    {
        private const int IndentAmount = 15;

        private SerializableObject serializableObject;
        private SerializableArray serializableArray;

        public InspectableObject(SerializableObject serializableObject)
        {
            this.serializableObject = serializableObject;
        }

        public InspectableObject(SerializableArray serializableArray)
        {
            this.serializableArray = serializableArray;
        }

        public void CreateGUI(GUILayout layout)
        {
            if (serializableObject != null)
            {
                foreach (var field in serializableObject.fields)
                {
                    CreateGUI(layout, field.Name, field.Type, field.GetValue());
                }
            }
            else if (serializableArray != null)
            {
                // TODO - Handle multi-rank arrays

                int length = serializableArray.GetDimension(0);
                for (int i = 0; i < length; i++)
                {
                    CreateGUI(layout, i.ToString(), serializableArray.ElementType, serializableArray.GetValue(i));
                }
            }

            // TODO - Handle List & Dictionary
        }

        private void CreateGUI(GUILayout layout, string name, SerializableField.FieldType type, SerializableValue value)
        {
            switch (type)
            {
                case SerializableField.FieldType.Object:
                    CreateComplexField(layout, name, type, value);
                    break;
                case SerializableField.FieldType.Array:
                    goto case SerializableField.FieldType.Object;
                case SerializableField.FieldType.List:
                    goto case SerializableField.FieldType.Object;
                case SerializableField.FieldType.Dictionary:
                    goto case SerializableField.FieldType.Object;
                default:
                    CreatePrimitiveField(layout, name, type, value);
                    break;
            }
        }

        private void CreatePrimitiveField(GUILayout layout, string name, SerializableField.FieldType type, SerializableValue value)
        {
            layout.AddElement(new GUILabel(name)); // TODO - Use an IntEditorField, or others for other types
        }

        private void CreateComplexField(GUILayout layout, string name, SerializableField.FieldType type, SerializableValue value)
        {
            layout.AddElement(new GUILabel(name)); // TODO - Add foldout and hook up its callbacks

            GUILayout childLayout = layout.AddLayoutX();
            childLayout.AddSpace(IndentAmount);

            GUILayout childContent = childLayout.AddLayoutY();

            switch (type)
            {
                case SerializableField.FieldType.Object:
                    {
                        SerializableObject childObj = new SerializableObject(value.GetValue<object>());
                        InspectableObject childInspectable = new InspectableObject(childObj);
                        childInspectable.CreateGUI(childContent);
                    }
                    break;
                case SerializableField.FieldType.Array:
                    {
                        SerializableArray childArr = new SerializableArray(value.GetValue<object>());
                        InspectableObject childInspectable = new InspectableObject(childArr);
                        childInspectable.CreateGUI(childContent);
                    }
                    break;
                case SerializableField.FieldType.List:
                    // TODO
                    break;
                case SerializableField.FieldType.Dictionary:
                    // TODO
                    break;
                default:
                    throw new Exception("Invalid complex field type.");
            }
        }
    }
}
