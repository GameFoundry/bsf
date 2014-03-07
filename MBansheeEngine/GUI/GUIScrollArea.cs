using System.Runtime.CompilerServices;

namespace BansheeEngine
{
    public enum ScrollBarType
	{
		ShowIfDoesntFit,
		AlwaysShow,
		NeverShow
	};

    public sealed class GUIScrollArea : GUIElement
    {
        private GUILayout _mainLayout;

        public GUILayout layout
        {
            get { return _mainLayout; }
        }

        internal GUIScrollArea(GUILayout parentLayout, ScrollBarType vertBarType, ScrollBarType horzBarType, 
            GUIElementStyle scrollBarStyle, GUIElementStyle scrollAreaStyle, params GUIOption[] options)
            :base(parentLayout)
        {
            Internal_CreateInstance(this, parentLayout, vertBarType, horzBarType, scrollBarStyle, scrollAreaStyle, options);
            _mainLayout = new GUILayoutY(this);
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_CreateInstance(GUIScrollArea instance, GUILayout parentLayout, ScrollBarType vertBarType, ScrollBarType horzBarType,
            GUIElementStyle scrollBarStyle, GUIElementStyle scrollAreaStyle, params GUIOption[] options);
    }
}
