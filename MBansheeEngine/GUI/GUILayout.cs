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
            if (element.IsStatic())
            {
                Debug.LogWarning("You are trying to change parent of a static GUI element. Ignoring operation.");
                return;
            }

            if(AddElementInternal(element))
                Internal_AddElement(mCachedPtr, element.mCachedPtr);
        }

        public void InsertElement(int index, GUIElement element)
        {
            if (element.IsStatic())
            {
                Debug.LogWarning("You are trying to change parent of a static GUI element. Ignoring operation.");
                return;
            }

            if (InsertElementInternal(index, element))
                Internal_InsertElement(mCachedPtr, index, element.mCachedPtr);
        }

        public GUIFixedSpace AddSpace(int size)
        {
            GUIFixedSpace fixedSpace = new GUIFixedSpace(this, size);
            AddElementInternal(fixedSpace);

            return fixedSpace;
        }

        public GUIFixedSpace InsertSpace(int index, int size)
        {
            GUIFixedSpace fixedSpace = new GUIFixedSpace(this, index, size);
            InsertElementInternal(index, fixedSpace);

            return fixedSpace;
        }

        public GUIFlexibleSpace AddFlexibleSpace()
        {
            GUIFlexibleSpace flexibleSpace = new GUIFlexibleSpace(this);
            AddElementInternal(flexibleSpace);

            return flexibleSpace;
        }

        public GUIFlexibleSpace InsertFlexibleSpace(int index)
        {
            GUIFlexibleSpace flexibleSpace = new GUIFlexibleSpace(this, index);
            InsertElementInternal(index, flexibleSpace);

            return flexibleSpace;
        }

        public GUILayoutX AddLayoutX()
        {
            GUILayoutX layoutX = new GUILayoutX(this);
            AddElementInternal(layoutX);

            return layoutX;
        }

        public GUILayoutX InsertLayoutX(int index)
        {
            GUILayoutX layoutX = new GUILayoutX(this, index);
            InsertElementInternal(index, layoutX);

            return layoutX;
        }

        public GUILayoutY AddLayoutY()
        {
            GUILayoutY layoutY = new GUILayoutY(this);
            AddElementInternal(layoutY);

            return layoutY;
        }

        public GUILayoutY InsertLayoutY(int index)
        {
            GUILayoutY layoutY = new GUILayoutY(this, index);
            InsertElementInternal(index, layoutY);

            return layoutY;
        }

        public GUILayoutExplicit AddLayoutExplicit()
        {
            GUILayoutExplicit layoutY = new GUILayoutExplicit(this);
            AddElementInternal(layoutY);

            return layoutY;
        }

        public GUILayoutExplicit InsertLayoutExplicit(int index)
        {
            GUILayoutExplicit layoutY = new GUILayoutExplicit(this, index);
            InsertElementInternal(index, layoutY);

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
        protected static extern void Internal_CreateInstanceYFromScrollArea(GUILayout instance, GUIScrollArea parentArea);

        [MethodImpl(MethodImplOptions.InternalCall)]
        protected static extern void Internal_CreateInstanceXFromLayoutAdd(GUILayout instance, GUILayout parentLayout);

        [MethodImpl(MethodImplOptions.InternalCall)]
        protected static extern void Internal_CreateInstanceYFromLayoutAdd(GUILayout instance, GUILayout parentLayout);

        [MethodImpl(MethodImplOptions.InternalCall)]
        protected static extern void Internal_CreateInstanceExplicitFromLayoutAdd(GUILayout instance, GUILayout parentLayout);

        [MethodImpl(MethodImplOptions.InternalCall)]
        protected static extern void Internal_CreateInstanceXFromLayoutInsert(GUILayout instance, GUILayout parentLayout, int index);

        [MethodImpl(MethodImplOptions.InternalCall)]
        protected static extern void Internal_CreateInstanceYFromLayoutInsert(GUILayout instance, GUILayout parentLayout, int index);

        [MethodImpl(MethodImplOptions.InternalCall)]
        protected static extern void Internal_CreateInstanceExplicitFromLayoutInsert(GUILayout instance, GUILayout parentLayout, int index);

        [MethodImpl(MethodImplOptions.InternalCall)]
        protected static extern void Internal_AddElement(IntPtr instance, IntPtr element);

        [MethodImpl(MethodImplOptions.InternalCall)]
        protected static extern void Internal_InsertElement(IntPtr instance, int index, IntPtr element);
    }
}
