using System;

namespace BansheeEditor
{
    /// <summary>
    /// Can be placed on <see cref="EditorWindow"/> class to provide a default size for the window when it is first opened.
    /// </summary>
    [AttributeUsage(AttributeTargets.Class)]
    public sealed class DefaultSize : Attribute
    {
        private int width = 400;
        private int height = 400;

        public DefaultSize(int width, int height)
        {
            this.width = width;
            this.height = height;
        }
    }
}
