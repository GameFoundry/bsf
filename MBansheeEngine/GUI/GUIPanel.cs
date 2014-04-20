using System;
using System.Collections.Generic;
using System.Runtime.CompilerServices;

namespace BansheeEngine
{
    public sealed class GUIPanel : ScriptObject
    {
        private GUIArea mainArea;
        private GUILayout _mainLayout;

        internal List<GUIArea> childAreas = new List<GUIArea>();

        public GUILayout layout
        {
            get { return _mainLayout; }
        }

        public GUISkin skin; // TODO

        internal GUIPanel()
        {
            Internal_CreateInstance(this);
        }

        ~GUIPanel()
        {
            GUIArea[] childArray = childAreas.ToArray(); // Iterating over it will modify it so make a copy
            for (int i = 0; i < childArray.Length; i++)
                childArray[i].Destroy();

            childAreas.Clear();
        }

        internal void Initialize()
        {
            mainArea = AddArea(0, 0, 0, 0);
            _mainLayout = mainArea.layout;
        }

        public GUIArea AddArea(int x, int y, int width, int height, short depth = 0)
        {
            GUIArea area = GUIArea.Create(this, x, y, width, height, depth);
            area.SetParent(this);

            return area;
        }

        internal void SetArea(int x, int y, int width, int height)
        {
            Internal_SetArea(mCachedPtr, x, y, width, height);

            mainArea.SetArea(x, y, width, height);
        }

        internal void Destroy()
        {
            GUIArea[] tempAreas = childAreas.ToArray();
            for (int i = 0; i < tempAreas.Length; i++)
                tempAreas[i].Destroy();

            childAreas.Clear();

            Internal_Destroy(mCachedPtr);
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetArea(IntPtr nativeInstance, int x, int y, int width, int height);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_CreateInstance(GUIPanel instance);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_Destroy(IntPtr nativeInstance);
    }
}
