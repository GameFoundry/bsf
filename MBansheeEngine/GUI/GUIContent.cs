using System.Runtime.CompilerServices;

namespace BansheeEngine
{
    public sealed class GUIContent
    {
		public GUIContent(LocString text)
		{
		    _text = text;
		}

        public GUIContent(LocString text, LocString tooltip)
        {
            _text = text;
            _tooltip = tooltip;
        }

        public GUIContent(SpriteTexture image)
        {
            _image = image;
        }

        public GUIContent(SpriteTexture image, LocString tooltip)
        {
            _image = image;
            _tooltip = tooltip;
        }

        public GUIContent(LocString text, SpriteTexture image)
        {
            _text = text;
            _image = image;
        }

        public GUIContent(LocString text, SpriteTexture image, LocString tooltip)
        {
            _text = text;
            _image = image;
            _tooltip = tooltip;
        }

        public static implicit operator GUIContent(LocString text)
        {
            return new GUIContent(text);
        }

        public static implicit operator GUIContent(string text)
        {
            return new GUIContent(new LocString(text));
        }

        public LocString text
        {
            get { return _text; }
        }

        public SpriteTexture image
        {
            get { return _image; }
        }

        public LocString getTooltip
        {
            get { return _tooltip; }
        }

        private LocString _text;
        private LocString _tooltip;
        private SpriteTexture _image;
    }
}
