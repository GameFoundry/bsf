using System;
using System.Collections.Generic;
using System.Runtime.CompilerServices;

namespace BansheeEngine
{
    public class ContextMenu : ScriptObject
    {
        private List<Action> callbacks = new List<Action>(); 

        public ContextMenu()
        {
            Internal_CreateInstance(this);
        }

        public void AddItem(string path, Action callback)
        {
            Internal_AddItem(mCachedPtr, path, callbacks.Count, ShortcutKey.None);
            callbacks.Add(callback);
        }

        public void AddItem(string path, Action callback, ShortcutKey shortcut)
        {
            Internal_AddItem(mCachedPtr, path, callbacks.Count, shortcut);
            callbacks.Add(callback);
        }

        public void AddItem(string path, Action callback, LocString name)
        {
            Internal_AddItem(mCachedPtr, path, callbacks.Count, ShortcutKey.None);
            callbacks.Add(callback);
        }

        public void AddItem(string path, Action callback, ShortcutKey shortcut, LocString name)
        {
            Internal_AddItem(mCachedPtr, path, callbacks.Count, shortcut);
            callbacks.Add(callback);
        }

        public void AddSeparator(string path)
        {
            Internal_AddSeparator(mCachedPtr, path);
        }

        public void SetLocalizedName(string label, LocString name)
        {
            IntPtr namePtr = IntPtr.Zero;
            if (name != null)
                namePtr = name.GetCachedPtr();

            Internal_SetLocalizedName(mCachedPtr, label, namePtr);
        }

        private void InternalDoOnEntryTriggered(int callbackIdx)
        {
            if (callbackIdx < 0 || callbackIdx >= callbacks.Count)
                return;

            Action callback = callbacks[callbackIdx];
            if (callback != null)
                callback();
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_CreateInstance(ContextMenu instance);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_AddItem(IntPtr instance, string path, int callbackIdx, ShortcutKey shortcut);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_AddSeparator(IntPtr instance, string path);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetLocalizedName(IntPtr instance, string label, IntPtr name);
    }
}
