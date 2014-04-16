using System;
using BansheeEngine;

namespace BansheeEditor
{
    [CustomInspector(typeof(DbgComponent))]
    public class DbgCustomInspector : Inspector
    {
        internal override void Refresh()
        {
            throw new NotImplementedException();
        }

        internal override int GetOptimalHeight()
        {
            throw new NotImplementedException();
        }
    }
}
