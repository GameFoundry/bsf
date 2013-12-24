using System.Runtime.InteropServices;

namespace BansheeEngine
{
    public sealed class Font : Resource // TODO - Dummy class
    {
        public Font()
        {
            Internal_CreateInstance(this);
        }

        [DllImport("__Internal")]
        private static extern void Internal_CreateInstance(Font instance);
    }
}
