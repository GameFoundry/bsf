using System;
using System.Collections.Generic;
using System.Runtime.CompilerServices;

namespace BansheeEngine
{
    public abstract class GUILayout : GUIElement
    {
        internal List<GUIElement> children = new List<GUIElement>();

        internal override bool IsStatic()
        {
            return true;
        }

        internal void AddElementInternal(GUIElement element)
        {
            if (IsDestroyed())
            {
                Debug.LogWarning("Attempting to add an element to a destroyed layout. Ignoring operation.");
                return;
            }

            if (!children.Contains(element))
                element.SetParent(this);
        }

        internal override void SetParent(GUILayout layout)
        {
            if (parent != null)
                parent.Remove(this);

            parent = layout;

            if (parent != null)
                parent.children.Add(this);
        }

        public void AddElement(GUIElement element)
        {
            if (element.IsStatic())
            {
                Debug.LogWarning("You are trying to change parent of a static GUI element. Ignoring operation.");
                return;
            }

            AddElementInternal(element);
        }

        public GUIFixedSpace AddSpace(int size)
        {
            GUIFixedSpace fixedSpace = new GUIFixedSpace(this, size);
            AddElementInternal(fixedSpace);

            return fixedSpace;
        }

        public GUIFlexibleSpace AddFlexibleSpace()
        {
            GUIFlexibleSpace flexibleSpace = new GUIFlexibleSpace(this);
            AddElementInternal(flexibleSpace);

            return flexibleSpace;
        }

        public GUILayoutX AddLayoutX()
        {
            GUILayoutX layoutX = new GUILayoutX(this);
            AddElementInternal(layoutX);

            return layoutX;
        }

        public GUILayoutY AddLayoutY()
        {
            GUILayoutY layoutY = new GUILayoutY(this);
            AddElementInternal(layoutY);

            return layoutY;
        }

        public void Remove(GUIElement element)
        {
            if (children.Contains(element))
                element.SetParent(null);
        }

        public void Remove(int childIdx)
        {
            if (childIdx >= 0 && childIdx < children.Count)
            {
                GUIElement element = children[childIdx];
                element.SetParent(null);
            }
        }

        public int GetNumChildren()
        {
            return children.Count;
        }

        public GUIElement GetChild(int index)
        {
            if (index < 0 || index >= children.Count)
                return null;

            return children[index];
        }

        public override void Destroy()
        {
            GUIElement[] childArray = children.ToArray(); // Iterating over it will modify it so make a copy
            for (int i = 0; i < childArray.Length; i++)
                childArray[i].Destroy();

            children.Clear();

            base.Destroy();
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        protected static extern void Internal_CreateInstanceXFromArea(GUILayout instance, GUIArea parentArea);

        [MethodImpl(MethodImplOptions.InternalCall)]
        protected static extern void Internal_CreateInstanceXFromLayout(GUILayout instance, GUILayout parentLayout);

        [MethodImpl(MethodImplOptions.InternalCall)]
        protected static extern void Internal_CreateInstanceYFromScrollArea(GUILayout instance, GUIScrollArea parentArea);

        [MethodImpl(MethodImplOptions.InternalCall)]
        protected static extern void Internal_CreateInstanceYFromLayout(GUILayout instance, GUILayout parentLayout);
    }
}
