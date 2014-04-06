using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace BansheeEngine
{
    public abstract class InspectableObjectBase
    {
        private bool _isExpanded = false;
        private InspectableField[] _fields;

        public bool isExpanded { get { return _isExpanded; } }
        public InspectableField[] fields { get { return _fields; } }

        public void Expand()
        {
            // TODO - Show all child "fields"
            //  Reposition all visual child elements
            //  Re-do tab indexes
            _isExpanded = true;
        }

        public void Collapse()
        {
            // TODO - Hide all child "fields"
            //  Reposition all visual child elements
            //  Re-do tab indexes
            _isExpanded = false;
        }

        public void Refresh()
        {
            for (int i = 0; i < fields.Length; i++)
                fields[i].Refresh();
        }

        public void Destroy()
        {
            for (int i = 0; i < fields.Length; i++)
                fields[i].Destroy();
        }
    }
}
