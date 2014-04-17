using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace BansheeEditor
{
    internal sealed class GenericInspector : Inspector
    {
        internal override void Refresh()
        {
            InspectableObject obj = new InspectableObject(serializableObject);

            obj.CreateGUI(GUI.layout);
        }

        internal override int GetOptimalHeight()
        {
            throw new NotImplementedException();
        }
    }
}
