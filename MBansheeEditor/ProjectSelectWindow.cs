using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace BansheeEditor
{
    internal sealed class ProjectSelectWindow : ModalWindow
    {
        public ProjectSelectWindow()
            :base(0, 0, 200, 200)
        {
            GUI.main.AddLabel("Test test");
        }
    }
}
