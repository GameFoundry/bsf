using System.Runtime.CompilerServices;

namespace BansheeEngine
{
    public sealed class GUIScrollArea : GUIElement
    {
        private GUILayout _mainLayout;

        public GUILayout layout
        {
            get { return _mainLayout; }
        }

        internal GUIScrollArea(GUILayout parentLayout, GUIElementStyle style, params GUIOption[] options)
            :base(parentLayout)
        {
            Internal_CreateInstance(this, parentLayout, style, options);
            _mainLayout = new GUILayoutY(this);
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_CreateInstance(GUIScrollArea instance, GUILayout parentLayout, GUIElementStyle style, params GUIOption[] options);
    }
}
