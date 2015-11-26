using System.Runtime.CompilerServices;

namespace BansheeEngine
{
    /// <summary>
    /// Type of GUI element states.
    /// </summary>
	public enum GUIElementState
	{
        /// <summary>Normal state when button is not being iteracted with.</summary>
		Normal = 0x01,
        /// <summary>State when pointer is hovering over the button.</summary>
		Hover = 0x02,
        /// <summary>State when button is being clicked.</summary>
		Active = 0x04,
        /// <summary>State when button has been selected.</summary>
		Focused = 0x08,
        /// <summary>Normal state when button is not being iteracted with and is in "on" state.</summary>
		NormalOn = 0x11,
        /// <summary>State when pointer is hovering over the button and is in "on" state.</summary>
		HoverOn = 0x12,
        /// <summary>State when button is being clicked and is in "on" state.</summary>
		ActiveOn = 0x14,
        /// <summary>State when button has been selected and is in "on" state.</summary>
		FocusedOn = 0x18
	};

    /// <summary>
    /// Holds data used for displaying content in a GUIElement. Content can consist of a string, image, a tooltip or none 
    /// of those.
    /// </summary>
    public sealed class GUIContent
    {
        private LocString text;
        private LocString tooltip;
        private GUIContentImages images;

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
        public SpriteTexture GetImage(GUIElementState state = GUIElementState.Normal)
        {
            switch (state)
		    {
		    case GUIElementState.Normal:
			    return images.normal;
		    case GUIElementState.Hover:
                return images.hover;
		    case GUIElementState.Active:
                return images.active;
		    case GUIElementState.Focused:
                return images.focused;
		    case GUIElementState.NormalOn:
                return images.normalOn;
		    case GUIElementState.HoverOn:
                return images.hoverOn;
		    case GUIElementState.ActiveOn:
                return images.activeOn;
		    case GUIElementState.FocusedOn:
                return images.focusedOn;
		    default:
                return images.normal;
		    }
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
        public GUIContent(GUIContentImages image)
        {
            this.images = image;
        }

        /// <summary>
        /// Constructs content with an image and a tooltip.
        /// </summary>
        /// <param name="image">Image to be displayed on top of the GUI element.</param>
        /// <param name="tooltip">Tooltip to be displayed when user hovers over a GUI element.</param>
        public GUIContent(GUIContentImages image, LocString tooltip)
        {
            this.images = image;
            this.tooltip = tooltip;
        }

        /// <summary>
        /// Constructs content with a string and an image.
        /// </summary>
        /// <param name="text">Textual portion of the content to be displayed as label in GUI elements.</param>
        /// <param name="image">Image to be displayed on top of the GUI element. Image will be placed to the right or to the 
        ///                     left of the text depending on active GUI style.</param>
        public GUIContent(LocString text, GUIContentImages image)
        {
            this.text = text;
            this.images = image;
        }

        /// <summary>
        /// Constructs content with a string, an image and a tooltip.
        /// </summary>
        /// <param name="text">Textual portion of the content to be displayed as label in GUI elements.</param>
        /// <param name="image">Image to be displayed on top of the GUI element. Image will be placed to the right or to the 
        ///                     left of the text depending on active GUI style.</param>
        /// <param name="tooltip">Tooltip to be displayed when user hovers over a GUI element.</param>
        public GUIContent(LocString text, GUIContentImages image, LocString tooltip)
        {
            this.text = text;
            this.images = image;
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

    /// <summary>
    /// Contains separate GUI content images for every possible GUI element state.
    /// </summary>
    public sealed class GUIContentImages
    {
        /// <summary>
        /// Creates an empty content image object referencing no images.
        /// </summary>
        public GUIContentImages()
        { }

        /// <summary>
        /// Creates a new object where content images for all states are the same.
        /// </summary>
        /// <param name="image">Image to assign to all states.</param>
        public GUIContentImages(SpriteTexture image)
        {
            normal = image;
            hover = image;
            active = image;
            focused = image;
            normalOn = image;
            hoverOn = image;
            activeOn = image;
            focusedOn = image;
        }

        /// <summary>
        /// Implicitly converts a sprite texture into a GUI content images object.
        /// </summary>
        /// <param name="image">Image to instantiate the GUI content images with.</param>
        /// <returns>GUI content images with all states set to the provided image.</returns>
        public static implicit operator GUIContentImages(SpriteTexture image)
        {
            return new GUIContentImages(image);
        }

        public SpriteTexture normal;
        public SpriteTexture hover;
        public SpriteTexture active;
        public SpriteTexture focused;
        public SpriteTexture normalOn;
        public SpriteTexture hoverOn;
        public SpriteTexture activeOn;
        public SpriteTexture focusedOn;
    }
}
