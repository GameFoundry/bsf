using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using BansheeEngine;

namespace BansheeEditor
{
    internal class LocEdString
    {
        // Note: This must match C++ HEString::ENGINE_STRING_TABLE_ID
        private const int EDITOR_STRING_TABLE_ID = 30000;

        private LocString internalString;

        public LocEdString()
        {
            internalString = new LocString(EDITOR_STRING_TABLE_ID);
        }

        public LocEdString(string identifier)
        {
            internalString = new LocString(identifier, EDITOR_STRING_TABLE_ID);
        }

        public static implicit operator LocString(LocEdString edString)
        {
            return edString.internalString;
        }

        public static implicit operator GUIContent(LocEdString edString)
        {
            return new GUIContent(edString.internalString);
        }
    }
}
