using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Text;
using System.Threading.Tasks;
using BansheeEngine;

namespace BansheeEditor
{
    public static class BrowseDialog
    {
        public static bool OpenFile(string defaultPath, string filterList, bool allowMultiselect, out string[] outPaths)
        {
            return Internal_OpenFile(defaultPath, filterList, allowMultiselect, out outPaths);
        }

        public static bool OpenFolder(string defaultPath, string filterList, out string outPath)
        {
            return Internal_OpenFolder(defaultPath, filterList, out outPath);
        }

        public static bool SaveFile(string defaultPath, string filterList, out string outPath)
        {
            return Internal_SaveFile(defaultPath, filterList, out outPath);
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern bool Internal_OpenFile(string defaultPath, string filterList, bool allowMultiselect, out string[] outPaths);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern bool Internal_OpenFolder(string defaultPath, string filterList, out string outPath);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern bool Internal_SaveFile(string defaultPath, string filterList, out string outPath);
    }
}
