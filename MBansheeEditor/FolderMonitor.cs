using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Text;
using System.Threading.Tasks;
using BansheeEngine;

namespace BansheeEditor
{
    public class FolderMonitor : ScriptObject
    {
        public Action<string> OnModified;
        public Action<string> OnAdded;
        public Action<string> OnRemoved;
        public Action<string, string> OnRenamed;

        public FolderMonitor(string folderToMonitor)
        {
            Internal_CreateInstance(this, folderToMonitor);
        }

        public void Destroy()
        {
            Internal_Destroy(mCachedPtr);
        }

        private void Internal_DoOnModified(string path)
        {
            if (OnModified != null)
                OnModified(path);
        }

        private void Internal_DoOnAdded(string path)
        {
            if (OnAdded != null)
                OnAdded(path);
        }

        private void Internal_DoOnRemoved(string path)
        {
            if (OnRemoved != null)
                OnRemoved(path);
        }

        private void Internal_DoOnRenamed(string from, string to)
        {
            if (OnRenamed != null)
                OnRenamed(from, to);
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_CreateInstance(FolderMonitor instance, string folder);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_Destroy(IntPtr thisPtr);
    }
}
