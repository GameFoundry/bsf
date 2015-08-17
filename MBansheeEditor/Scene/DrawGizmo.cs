using System;

namespace BansheeEditor
{
    // Note: Must match the C++ enum DrawGizmoFlags
    public enum DrawGizmoFlags
    {
        Selected = 0x01,
        ParentSelected = 0x02,
        NotSelected = 0x04,
        Pickable = 0x08
    }

    [AttributeUsage(AttributeTargets.Method)]
    public sealed class DrawGizmo : Attribute
    {
        private DrawGizmoFlags flags;

        public DrawGizmo(DrawGizmoFlags flags)
        {
            this.flags = flags;
        }
    }
}
