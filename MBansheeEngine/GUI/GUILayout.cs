using System;
using System.Collections.Generic;
using System.Runtime.CompilerServices;

namespace BansheeEngine
{
    public abstract class GUILayout : GUIElement
    {
        public void AddElement(GUIElement element)
        {
            if(element != null)
                Internal_AddElement(mCachedPtr, element.mCachedPtr);
        }

        public void InsertElement(int index, GUIElement element)
        {
            if(index < 0 || index > GetNumChildren())
                throw new ArgumentOutOfRangeException("index", index, "Index out of range.");

            if (element != null)
                Internal_InsertElement(mCachedPtr, index, element.mCachedPtr);
        }

        public int GetNumChildren()
        {
            return Internal_GetChildCount(mCachedPtr);
        }

        public GUIElement GetChild(int index)
        {
            return Internal_GetChild(mCachedPtr, index);
        }

        public GUILayoutX AddLayoutX(params GUIOption[] options)
        {
            GUILayoutX layout = new GUILayoutX(options);
            AddElement(layout);
            return layout;
        }

        public GUILayoutY AddLayoutY(params GUIOption[] options)
        {
            GUILayoutY layout = new GUILayoutY(options);
            AddElement(layout);
            return layout;
        }

        public GUIPanel AddPanel(params GUIOption[] options)
        {
            GUIPanel layout = new GUIPanel(options);
            AddElement(layout);
            return layout;
        }

        public GUIPanel AddPanel(Int16 depth = 0, ushort depthRangeMin = ushort.MaxValue, ushort depthRangeMax = ushort.MaxValue, params GUIOption[] options)
        {
            GUIPanel layout = new GUIPanel(depth, depthRangeMin, depthRangeMax, options);
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
            GUILayoutX layout = new GUILayoutX(options);
            InsertElement(idx, layout);
            return layout;
        }

        public GUILayoutY InsertLayoutY(int idx, params GUIOption[] options)
        {
            GUILayoutY layout = new GUILayoutY(options);
            InsertElement(idx, layout);
            return layout;
        }

        public GUIPanel InsertPanel(int idx, params GUIOption[] options)
        {
            GUIPanel layout = new GUIPanel(options);
            InsertElement(idx, layout);
            return layout;
        }

        public GUIPanel InsertPanel(int idx, Int16 depth = 0, ushort depthRangeMin = ushort.MaxValue, 
            ushort depthRangeMax = ushort.MaxValue, params GUIOption[] options)
        {
            GUIPanel layout = new GUIPanel(depth, depthRangeMin, depthRangeMax, options);
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
        protected static extern void Internal_CreateInstanceYFromScrollArea(GUILayout instance, GUIScrollArea parentArea);

        [MethodImpl(MethodImplOptions.InternalCall)]
        protected static extern void Internal_CreateInstanceX(GUILayout instance, GUIOption[] options);

        [MethodImpl(MethodImplOptions.InternalCall)]
        protected static extern void Internal_CreateInstanceY(GUILayout instance, GUIOption[] options);

        [MethodImpl(MethodImplOptions.InternalCall)]
        protected static extern void Internal_CreateInstancePanel(GUILayout instance, Int16 depth, ushort depthRangeMin, ushort depthRangeMax, GUIOption[] options);

        [MethodImpl(MethodImplOptions.InternalCall)]
        protected static extern void Internal_AddElement(IntPtr instance, IntPtr element);

        [MethodImpl(MethodImplOptions.InternalCall)]
        protected static extern void Internal_InsertElement(IntPtr instance, int index, IntPtr element);

        [MethodImpl(MethodImplOptions.InternalCall)]
        protected static extern int Internal_GetChildCount(IntPtr instance);

        [MethodImpl(MethodImplOptions.InternalCall)]
        protected static extern GUIElement Internal_GetChild(IntPtr instance, int index);
    }
}
