using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using BansheeEngine;
 
namespace BansheeEditor
{
    public class InspectableObjectBase
    {
        private List<InspectableObjectBase> children = new List<InspectableObjectBase>();
        private InspectableObjectBase parent;
        
        protected InspectableFieldLayout layout;
        protected SerializableProperty property;
        protected string title;

        public InspectableObjectBase(string title, InspectableFieldLayout layout, SerializableProperty property)
        {
            this.layout = layout;
            this.title = title;
            this.property = property;
        }

        protected void AddChild(InspectableObjectBase child)
        {
            if (child.parent == this)
                return;

            if (child.parent != null)
                child.parent.RemoveChild(child);

            children.Add(child);
            child.parent = this;
        }

        protected void RemoveChild(InspectableObjectBase child)
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

        protected virtual bool IsModified()
        {
            return false;
        }

        protected virtual void Update(int layoutIndex)
        {
            // Destroy all children as we expect update to rebuild them
            InspectableObjectBase[] childrenCopy = children.ToArray();
            for (int i = 0; i < childrenCopy.Length; i++)
            {
                childrenCopy[i].Destroy();
            }

            children.Clear();
        }

        public virtual void Destroy()
        {
            layout.DestroyElements();

            InspectableObjectBase[] childrenCopy = children.ToArray();
            for (int i = 0; i < childrenCopy.Length; i++)
                childrenCopy[i].Destroy();

            children.Clear();

            if (parent != null)
                parent.RemoveChild(this);
        }

        public static InspectableObjectBase CreateDefaultInspectable(string title, InspectableFieldLayout layout, SerializableProperty property)
        {
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

        public static InspectableObjectBase CreateCustomInspectable(Type inspectableType, string title, InspectableFieldLayout layout, SerializableProperty property)
        {
            if (!inspectableType.IsSubclassOf(typeof (InspectableObjectBase)))
                throw new Exception("Invalid inspector type.");

            return (InspectableObjectBase)Activator.CreateInstance(inspectableType, title, property);
        }
    }
}
