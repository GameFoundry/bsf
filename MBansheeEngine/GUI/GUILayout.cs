using System;
using System.Collections.Generic;
using System.Runtime.CompilerServices;

namespace BansheeEngine
{
    public abstract class GUILayout : GUIElement
    {
        internal List<GUIElement> children = new List<GUIElement>();

        internal bool AddElementInternal(GUIElement element)
        {
            if (IsDestroyed())
            {
                Debug.LogWarning("Attempting to add an element to a destroyed layout. Ignoring operation.");
                return false;
            }

            if (!children.Contains(element))
            {
                element.SetParent(this);
                children.Add(element);

                return true;
            }

            return false;
        }

        internal bool InsertElementInternal(int index, GUIElement element)
        {
            if (IsDestroyed())
            {
                Debug.LogWarning("Attempting to add an element to a destroyed layout. Ignoring operation.");
                return false;
            }

            if (!children.Contains(element))
            {
                element.SetParent(this);
                children.Insert(index, element);

                return true;
            }

            return false;
        }

        internal void RemoveInternal(GUIElement element)
        {
            children.Remove(element);
        }

        internal override void SetParent(GUILayout layout)
        {
            if (parent != null)
                parent.RemoveInternal(this);

            parent = layout;

            if (parent != null)
                parent.children.Add(this);
        }

        public void AddElement(GUIElement element)
        {
            if(AddElementInternal(element))
                Internal_AddElement(mCachedPtr, element.mCachedPtr);
        }

        public void InsertElement(int index, GUIElement element)
        {
            if (InsertElementInternal(index, element))
                Internal_InsertElement(mCachedPtr, index, element.mCachedPtr);
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

        public GUILayoutX AddLayoutX(params GUIOption[] options)
        {
            GUILayoutX layout = new GUILayoutX();
            AddElement(layout);
            return layout;
        }

        public GUILayoutY AddLayoutY(params GUIOption[] options)
        {
            GUILayoutY layout = new GUILayoutY();
            AddElement(layout);
            return layout;
        }

        public GUIPanelNEW AddPanel(params GUIOption[] options)
        {
            GUIPanelNEW layout = new GUIPanelNEW();
            AddElement(layout);
            return layout;
        }

        public GUIFlexibleSpace AddFlexibleSpace()
        {
            GUIFlexibleSpace space = new GUIFlexibleSpace();
            AddElement(space);
            return space;
        }

        public GUIFixedSpace AddSpace(int size)
        {
            GUIFixedSpace space = new GUIFixedSpace(size);
            AddElement(space);
            return space;
        }

        public GUILayoutX InsertLayoutX(int idx, params GUIOption[] options)
        {
            GUILayoutX layout = new GUILayoutX();
            InsertElement(idx, layout);
            return layout;
        }

        public GUILayoutY InsertLayoutY(int idx, params GUIOption[] options)
        {
            GUILayoutY layout = new GUILayoutY();
            InsertElement(idx, layout);
            return layout;
        }

        public GUIPanelNEW InsertPanel(int idx, params GUIOption[] options)
        {
            GUIPanelNEW layout = new GUIPanelNEW();
            InsertElement(idx, layout);
            return layout;
        }

        public GUIFlexibleSpace InsertFlexibleSpace(int idx)
        {
            GUIFlexibleSpace space = new GUIFlexibleSpace();
            InsertElement(idx, space);
            return space;
        }

        public GUIFixedSpace InsertSpace(int idx, int size)
        {
            GUIFixedSpace space = new GUIFixedSpace(size);
            InsertElement(idx, space);
            return space;
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        protected static extern void Internal_CreateInstanceXFromArea(GUILayout instance, GUIArea parentArea);

        [MethodImpl(MethodImplOptions.InternalCall)]
        protected static extern void Internal_CreateInstanceYFromScrollArea(GUILayout instance, GUIScrollArea parentArea);

        [MethodImpl(MethodImplOptions.InternalCall)]
        protected static extern void Internal_CreateInstanceX(GUILayout instance, GUIOption[] options);

        [MethodImpl(MethodImplOptions.InternalCall)]
        protected static extern void Internal_CreateInstanceY(GUILayout instance, GUIOption[] options);

        [MethodImpl(MethodImplOptions.InternalCall)]
        protected static extern void Internal_CreateInstancePanel(GUILayout instance, GUIOption[] options);

        [MethodImpl(MethodImplOptions.InternalCall)]
        protected static extern void Internal_AddElement(IntPtr instance, IntPtr element);

        [MethodImpl(MethodImplOptions.InternalCall)]
        protected static extern void Internal_InsertElement(IntPtr instance, int index, IntPtr element);
    }
}
