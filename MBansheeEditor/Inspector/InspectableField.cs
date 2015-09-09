using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using BansheeEngine;
 
namespace BansheeEditor
{
    public class InspectableField
    {
        private List<InspectableField> children = new List<InspectableField>();
        private InspectableField parent;
        
        protected InspectableFieldLayout layout;
        protected SerializableProperty property;
        protected string title;

        public InspectableField(string title, InspectableFieldLayout layout, SerializableProperty property)
        {
            this.layout = layout;
            this.title = title;
            this.property = property;
        }

        protected void AddChild(InspectableField child)
        {
            if (child.parent == this)
                return;

            if (child.parent != null)
                child.parent.RemoveChild(child);

            children.Add(child);
            child.parent = this;
        }

        protected void RemoveChild(InspectableField child)
        {
            children.Remove(child);
            child.parent = null;
        }

        public virtual bool Refresh(int layoutIndex)
        {
            bool anythingModified = false;

            if (IsModified())
            {
                Update(layoutIndex);
                anythingModified = true;
            }

            int currentIndex = 0;
            for (int i = 0; i < children.Count; i++)
            {
                anythingModified |= children[i].Refresh(currentIndex);
                currentIndex += children[i].GetNumLayoutElements();
            }

            return anythingModified;
        }

        public int GetNumLayoutElements()
        {
            return layout.GetNumElements();
        }

        public virtual GUILayoutX GetTitleLayout()
        {
            return null;
        }

        protected virtual bool IsModified()
        {
            return false;
        }

        protected virtual void Update(int layoutIndex)
        {
            // Destroy all children as we expect update to rebuild them
            InspectableField[] childrenCopy = children.ToArray();
            for (int i = 0; i < childrenCopy.Length; i++)
            {
                childrenCopy[i].Destroy();
            }

            children.Clear();
        }

        protected InspectableField GetChild(int index)
        {
            return children[index];
        }

        protected int GetChildCount()
        {
            return children.Count;
        }

        public virtual void Destroy()
        {
            layout.DestroyElements();

            InspectableField[] childrenCopy = children.ToArray();
            for (int i = 0; i < childrenCopy.Length; i++)
                childrenCopy[i].Destroy();

            children.Clear();

            if (parent != null)
                parent.RemoveChild(this);
        }

        public static InspectableField CreateInspectable(string title, InspectableFieldLayout layout, SerializableProperty property)
        {
            Type customInspectable = InspectorUtility.GetCustomInspectable(property.InternalType);
            if (customInspectable != null)
            {
                return (InspectableField)Activator.CreateInstance(customInspectable, title, property);
            }

            switch (property.Type)
            {
                case SerializableProperty.FieldType.Int:
                    return new InspectableInt(title, layout, property);
                case SerializableProperty.FieldType.Float:
                    return new InspectableFloat(title, layout, property);
                case SerializableProperty.FieldType.Bool:
                    return new InspectableBool(title, layout, property);
                case SerializableProperty.FieldType.Color:
                    return new InspectableColor(title, layout, property);
                case SerializableProperty.FieldType.String:
                    return new InspectableString(title, layout, property);
                case SerializableProperty.FieldType.Vector2:
                    return new InspectableVector2(title, layout, property);
                case SerializableProperty.FieldType.Vector3:
                    return new InspectableVector3(title, layout, property);
                case SerializableProperty.FieldType.Vector4:
                    return new InspectableVector4(title, layout, property);
                case SerializableProperty.FieldType.ResourceRef:
                    return new InspectableResourceRef(title, layout, property);
                case SerializableProperty.FieldType.GameObjectRef:
                    return new InspectableGameObjectRef(title, layout, property);
                case SerializableProperty.FieldType.Object:
                    return new InspectableObject(title, layout, property);
                case SerializableProperty.FieldType.Array:
                    return new InspectableArray(title, layout, property);
                case SerializableProperty.FieldType.List:
                    return new InspectableList(title, layout, property);
            }

            throw new Exception("No inspector exists for the provided field type.");
        }
    }
}
