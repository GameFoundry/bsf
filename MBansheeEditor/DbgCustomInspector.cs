using System;
using BansheeEngine;

namespace BansheeEditor
{
    [CustomInspector(typeof(Camera))]
    public class DbgCustomInspector : Inspector
    {
        internal override bool Refresh()
        {
            throw new NotImplementedException();
        }

        internal override int GetOptimalHeight()
        {
            throw new NotImplementedException();
        }
    }
}
