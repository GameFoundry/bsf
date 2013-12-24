using System.Runtime.CompilerServices;

namespace BansheeEngine
{
    public sealed class Font : Resource // TODO - Dummy class
    {
        public Font()
        {
            Internal_CreateInstance(this);
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_CreateInstance(Font instance);
    }
}
