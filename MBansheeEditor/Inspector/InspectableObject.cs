using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using BansheeEngine;

namespace BansheeEditor
{
    public sealed class InspectableObject
    {
        private delegate object Getter();
        private delegate void Setter(object value);

        private const int IndentAmount = 15;

        private object referencedObject;

        public InspectableObject(object obj)
        {
            referencedObject = obj;
        }

        public void CreateGUI(GUILayout layout, SerializableField field)
        {
            if (IsTypePrimitive(field.Type))
            {
                Getter getter = () => field.GetValue<object>();
                Setter setter = (object value) => field.SetValue(value);

                CreatePrimitiveField(layout, name, type, getter, setter);
            }
            else
            {
                CreateComplexField(layout, name, type, field.GetValue<object>());
            }
        }

        public void CreateGUI(GUILayout layout)
        {
            CreateComplexTypeChildren(layout, SerializableField.FieldType.Object, referencedObject);
        }

        private void CreatePrimitiveField(GUILayout layout, string name, SerializableField.FieldType type, Getter getter, Setter setter)
        {
            layout.AddElement(new GUILabel(name)); // TODO - Use an IntEditorField, or others for other types
        }

        private void CreateComplexField(GUILayout layout, string name, SerializableField.FieldType type, object obj)
        {
            layout.AddElement(new GUILabel(name)); // TODO - Add foldout and hook up its callbacks

            GUILayout childLayout = layout.AddLayoutX();
            childLayout.AddSpace(IndentAmount);

            GUILayout childContent = childLayout.AddLayoutY();

            CreateComplexTypeChildren(childContent, type, obj);
        }

        private void CreateComplexTypeChildren(GUILayout layout, SerializableField.FieldType type, object obj)
        {
            if (obj == null)
                return;

            switch (type)
            {
                case SerializableField.FieldType.Object:
                    {
                        SerializableObject serializableObject = new SerializableObject(obj);

                        foreach (var field in serializableObject.fields)
                        {
                            if (!field.Inspectable)
                                continue;

                            CreateGUI(layout, field);
                        }
                    }
                    break;
                case SerializableField.FieldType.Array:
                    {
                        SerializableArrayInfo arrayInfo = new SerializableArrayInfo(obj);

                        Array array = (Array)obj;
                        if (array.Rank > 1) // TODO - I didn't bother implementing multi-rank arrays yet but it is a simple job
                            throw new NotImplementedException();

                        if (IsTypePrimitive(arrayInfo.ElementType))
                        {
                            for (int i = 0; i < array.GetLength(0); i++)
                            {
                                int curIdx = i; // To avoid lambda passing by reference

                                Getter getter = () => array.GetValue(curIdx);
                                Setter setter = (object value) => array.SetValue(value, curIdx);

                                CreatePrimitiveField(layout, i + ".", arrayInfo.ElementType, getter, setter);
                            }
                        }
                        else
                        {
                            for (int i = 0; i < array.GetLength(0); i++)
                            {
                                CreateComplexField(layout, i + ".", arrayInfo.ElementType, array.GetValue(i));
                            }
                        }
                    }
                    break;
                case SerializableField.FieldType.List:
                    {
                        SerializableListInfo listInfo = new SerializableListInfo(obj);
                        IList list = (IList)obj;

                        if (IsTypePrimitive(listInfo.ElementType))
                        {
                            for (int i = 0; i < list.Count; i++)
                            {
                                int curIdx = i; // To avoid lambda passing by reference

                                Getter getter = () => list.Item[curIdx];
                                Setter setter = (object value) => list.Item[curIdx] = value;

                                CreatePrimitiveField(layout, i + ".", listInfo.ElementType, getter, setter);
                            }
                        }
                        else
                        {
                            for (int i = 0; i < array.GetLength(0); i++)
                            {
                                CreateComplexField(layout, i + ".", listInfo.ElementType, list.Item[i]);
                            }
                        }
                    }
                    break;
                case SerializableField.FieldType.Dictionary:
                    {
                        SerializableDictionaryInfo dictionaryInfo = new SerializableDictionaryInfo(obj);
                        IDictionary dictionary = (IDictionary)obj;

                        IEnumerator enumerator = dictionary.Keys.GetEnumerator();
                        int curIdx = 0;
                        while (enumerator.MoveNext())
                        {
                            if (IsTypePrimitive(dictionaryInfo.KeyType))
                            {
                                Getter getter = () => enumerator.Current;
                                Setter setter = (object value) => { }; // TODO - No setter!

                                CreatePrimitiveField(layout, curIdx + ".", dictionaryInfo.KeyType, getter, setter);
                            }
                            else
                            {
                                CreateComplexField(layout, curIdx + ".", dictionaryInfo.KeyType, enumerator.Current);
                            }

                            curIdx++;
                        }
                    }
                    break;
                default:
                    throw new Exception("Invalid complex field type.");
            }
        }

        private bool IsTypePrimitive(SerializableField.FieldType fieldType)
        {
            switch (fieldType)
            {
                case SerializableField.FieldType.Object:
                    return false;
                case SerializableField.FieldType.Array:
                    return false;
                case SerializableField.FieldType.List:
                    return false;
                case SerializableField.FieldType.Dictionary:
                    return false;
            }

            return true;
        }
    }
}
