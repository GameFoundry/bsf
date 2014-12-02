using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace BansheeEditor
{
    internal sealed class SceneWindow : EditorWindow
    {
        internal SceneWindow()
        {
            
        }

        protected override void WindowResized(int width, int height)
        {
            base.WindowResized(width, height);
        }
    }
}
