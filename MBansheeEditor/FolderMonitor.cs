using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Text;
using System.Threading.Tasks;
using BansheeEngine;

namespace BansheeEditor
{
    /// <summary>
    /// Allows monitoring a file system folder for changes. Depending on the flags set this monitor can notify you when 
    /// file is changed/moved/renamed and similar.
    /// </summary>
    public class FolderMonitor : ScriptObject
    {
        /// <summary>
        /// Triggers when a file in the monitored folder is modified. Provides absolute path to the modified file.
        /// </summary>
        public Action<string> OnModified;

        /// <summary>
        /// Triggers when a file/folder is added in the monitored folder. Provides absolute path to the added file/folder.
        /// </summary>
        public Action<string> OnAdded;

        /// <summary>
        /// Triggers when a file/folder is removed from the monitored folder. Provides absolute path to the removed 
        /// file/folder.
        /// </summary>
        public Action<string> OnRemoved;

        /// <summary>
        /// Triggers when a file/folder is renamed in the monitored folder. Provides absolute path with old and new names.
        /// </summary>
        public Action<string, string> OnRenamed;

        /// <summary>
        /// Constructs a new folder monitor instance that immediately starts monitor the provided folder.
        /// </summary>
        /// <param name="folderToMonitor">Absolute path to the folder to monitor.</param>
        public FolderMonitor(string folderToMonitor)
        {
            Internal_CreateInstance(this, folderToMonitor);
        }

        /// <summary>
        /// Stops monitoring the folder.
        /// </summary>
        public void Destroy()
        {
            Internal_Destroy(mCachedPtr);
        }

        /// <summary>
        /// Triggered by the runtime when a file in the monitored folder is modified.
        /// </summary>
        /// <param name="path">Absolute path to the modified file/folder.</param>
        private void Internal_DoOnModified(string path)
        {
            if (OnModified != null)
                OnModified(path);
        }

        /// <summary>
        /// Triggered by the runtime when a file in the monitored folder is added.
        /// </summary>
        /// <param name="path">Absolute path to the added file/folder.</param>
        private void Internal_DoOnAdded(string path)
        {
            if (OnAdded != null)
                OnAdded(path);
        }

        /// <summary>
        /// Triggered by the runtime when a file in the monitored folder is removed.
        /// </summary>
        /// <param name="path">Absolute path to the removed file/folder.</param>
        private void Internal_DoOnRemoved(string path)
        {
            if (OnRemoved != null)
                OnRemoved(path);
        }

        /// <summary>
        /// Triggered by the runtime when a file in the monitored folder is renamed.
        /// </summary>
        /// <param name="from">Absolute path to the original file/folder location.</param>
        /// <param name="to">Absolute path to the new file/folder location.</param>
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
