using System.Runtime.CompilerServices;

namespace BansheeEngine
{
    public sealed class GUIContent
    {
		public GUIContent(HString text)
		{
		    _text = text;
		}

        public GUIContent(HString text, HString tooltip)
        {
            _text = text;
            _tooltip = tooltip;
        }

        public GUIContent(SpriteTexture image)
        {
            _image = image;
        }

        public GUIContent(SpriteTexture image, HString tooltip)
        {
            _image = image;
            _tooltip = tooltip;
        }

        public GUIContent(HString text, SpriteTexture image)
        {
            _text = text;
            _image = image;
        }

        public GUIContent(HString text, SpriteTexture image, HString tooltip)
        {
            _text = text;
            _image = image;
            _tooltip = tooltip;
        }

        public static implicit operator GUIContent(HString text)
        {
            return new GUIContent(text);
        }

        public static implicit operator GUIContent(string text)
        {
            return new GUIContent(new HString(text));
        }

        public HString text
        {
            get { return _text; }
        }

        public SpriteTexture image
        {
            get { return _image; }
        }

        public HString getTooltip
        {
            get { return _tooltip; }
        }

        private HString _text;
        private HString _tooltip;
        private SpriteTexture _image;
    }
}
