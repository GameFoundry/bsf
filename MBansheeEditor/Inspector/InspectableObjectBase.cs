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

        protected SerializableProperty property;
        protected string title;

        public InspectableObjectBase(string title, SerializableProperty property)
        {
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

        public virtual void Refresh(GUILayout layout)
        {
            if (IsModified())
                Update(layout);

            for (int i = 0; i < children.Count; i++)
                children[i].Refresh(layout);
        }

        protected virtual bool IsModified()
        {
            return false;
        }

        protected virtual void Update(GUILayout layout)
        {
            // Do nothing
        }

        public virtual void Destroy()
        {
            InspectableObjectBase[] childrenCopy = children.ToArray();
            for (int i = 0; i < childrenCopy.Length; i++)
                children[i].Destroy();

            children.Clear();

            if (parent != null)
                parent.RemoveChild(this);
        }

        public static InspectableObjectBase CreateDefaultInspectable(string title, SerializableProperty property)
        {
            switch (property.Type)
            {
                case SerializableField.FieldType.Int:
                    return new InspectableInt(title, property);
                case SerializableField.FieldType.Object:
                    return new InspectableObject(title, property);
                case SerializableField.FieldType.Array:
                    return new InspectableArray(title, property);
                // TODO - Add all other types
            }

            throw new Exception("No inspector exists for the provided field type.");
        }

        public static InspectableObjectBase CreateCustomInspectable(Type inspectableType, string title, SerializableProperty property)
        {
            if (!inspectableType.IsSubclassOf(typeof (InspectableObjectBase)))
                throw new Exception("Invalid inspector type.");

            return (InspectableObjectBase)Activator.CreateInstance(inspectableType, title, property);
        }
    }
}
