using System.Runtime.CompilerServices;

namespace BansheeEngine
{
    /// <summary>
    /// Holds data used for displaying content in a GUIElement.  Content can consist of a string, image, a tooltip or none 
    /// of those.
    /// </summary>
    public sealed class GUIContent
    {
        private LocString text;
        private LocString tooltip;
        private SpriteTexture image;

        /// <summary>
        /// Returns string content (if any).
        /// </summary>
        public LocString Text
        {
            get { return text; }
        }

        /// <summary>
        /// Returns image content (if any).
        /// </summary>
        public SpriteTexture Image
        {
            get { return image; }
        }

        /// <summary>
        /// Returns tooltip content (if any).
        /// </summary>
        public LocString Tooltip
        {
            get { return tooltip; }
        }

        /// <summary>
        /// Constructs content with just a string.
        /// </summary>
        /// <param name="text">Textual portion of the content to be displayed as label in GUI elements.</param>
		public GUIContent(LocString text)
		{
		    this.text = text;
		}

        /// <summary>
        /// Constructs content with a string and a tooltip.
        /// </summary>
        /// <param name="text">Textual portion of the content to be displayed as label in GUI elements.</param>
        /// <param name="tooltip">Tooltip to be displayed when user hovers over a GUI element.</param>
        public GUIContent(LocString text, LocString tooltip)
        {
            this.text = text;
            this.tooltip = tooltip;
        }

        /// <summary>
        /// Constructs content with just an image.
        /// </summary>
        /// <param name="image">Image to be displayed on top of the GUI element.</param>
        public GUIContent(SpriteTexture image)
        {
            this.image = image;
        }

        /// <summary>
        /// Constructs content with an image and a tooltip.
        /// </summary>
        /// <param name="image">Image to be displayed on top of the GUI element.</param>
        /// <param name="tooltip">Tooltip to be displayed when user hovers over a GUI element.</param>
        public GUIContent(SpriteTexture image, LocString tooltip)
        {
            this.image = image;
            this.tooltip = tooltip;
        }

        /// <summary>
        /// Constructs content with a string and an image.
        /// </summary>
        /// <param name="text">Textual portion of the content to be displayed as label in GUI elements.</param>
        /// <param name="image">Image to be displayed on top of the GUI element. Image will be placed to the right or to the 
        ///                     left of the text depending on active GUI style.</param>
        public GUIContent(LocString text, SpriteTexture image)
        {
            this.text = text;
            this.image = image;
        }

        /// <summary>
        /// Constructs content with a string, an image and a tooltip.
        /// </summary>
        /// <param name="text">Textual portion of the content to be displayed as label in GUI elements.</param>
        /// <param name="image">Image to be displayed on top of the GUI element. Image will be placed to the right or to the 
        ///                     left of the text depending on active GUI style.</param>
        /// <param name="tooltip">Tooltip to be displayed when user hovers over a GUI element.</param>
        public GUIContent(LocString text, SpriteTexture image, LocString tooltip)
        {
            this.text = text;
            this.image = image;
            this.tooltip = tooltip;
        }

        /// <summary>
        /// Implicitly converts a localized string into a GUI content containing only text.
        /// </summary>
        /// <param name="text">Localized string to initialize the GUI content with.</param>
        /// <returns>GUI content containing only a string.</returns>
        public static implicit operator GUIContent(LocString text)
        {
            return new GUIContent(text);
        }

        /// <summary>
        /// Implicitly converts a string into a GUI content containing only text.
        /// </summary>
        /// <param name="text">String to initialize the GUI content with.</param>
        /// <returns>GUI content containing only a string.</returns>
        public static implicit operator GUIContent(string text)
        {
            return new GUIContent(new LocString(text));
        }
    }
}
