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

        public GUIScrollArea(ScrollBarType vertBarType, ScrollBarType horzBarType, string scrollBarStyle,
            string scrollAreaStyle, params GUIOption[] options)
        {
            Internal_CreateInstance(this, vertBarType, horzBarType, scrollBarStyle, scrollAreaStyle, options);
            _mainLayout = new GUILayoutY(this);
        }

        public GUIScrollArea(ScrollBarType vertBarType, ScrollBarType horzBarType, string style, params GUIOption[] options)
        {
            Internal_CreateInstance(this, vertBarType, horzBarType, "", style, options);
            _mainLayout = new GUILayoutY(this);
        }

        public GUIScrollArea(ScrollBarType vertBarType, ScrollBarType horzBarType, params GUIOption[] options)
        {
            Internal_CreateInstance(this, vertBarType, horzBarType, "", "", options);
            _mainLayout = new GUILayoutY(this);
        }

        public GUIScrollArea(string style, params GUIOption[] options)
        {
            Internal_CreateInstance(this, ScrollBarType.ShowIfDoesntFit, ScrollBarType.ShowIfDoesntFit, "", style, options);
            _mainLayout = new GUILayoutY(this);
        }

        public GUIScrollArea(params GUIOption[] options)
        {
            Internal_CreateInstance(this, ScrollBarType.ShowIfDoesntFit, ScrollBarType.ShowIfDoesntFit, "", "", options);
            _mainLayout = new GUILayoutY(this);
        }

        public GUIScrollArea(string scrollBarStyle, string scrollAreaStyle, params GUIOption[] options)
        {
            Internal_CreateInstance(this, ScrollBarType.ShowIfDoesntFit, ScrollBarType.ShowIfDoesntFit, scrollBarStyle, scrollAreaStyle, options);
            _mainLayout = new GUILayoutY(this);
        }

        public override void Destroy()
        {
            _mainLayout.Destroy();

            base.Destroy();
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_CreateInstance(GUIScrollArea instance, ScrollBarType vertBarType, ScrollBarType horzBarType,
            string scrollBarStyle, string scrollAreaStyle, params GUIOption[] options);
    }
}
