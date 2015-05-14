using System.Runtime.InteropServices;

namespace BansheeEngine
{
    [StructLayout(LayoutKind.Sequential)]
    public struct ShortcutKey
    {
        public ButtonModifier modifier;
        public ButtonCode key;

        public ShortcutKey(ButtonModifier modifier, ButtonCode key)
        {
            this.modifier = modifier;
            this.key = key;
        }

        public static ShortcutKey None = new ShortcutKey(ButtonModifier.None, ButtonCode.Unassigned);
    }
}
