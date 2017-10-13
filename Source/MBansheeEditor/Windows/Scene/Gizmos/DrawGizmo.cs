//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
using System;

namespace BansheeEditor
{
    /** @addtogroup Gizmos
     *  @{
     */

    /// <summary>
    /// Controls when a gizmo is displayed and whether it can be interacted with. These flags can be combined to achieve
    /// different effect.
    /// </summary>
    [Flags]
    public enum DrawGizmoFlags // Note: Must match the C++ enum DrawGizmoFlags
    {
        /// <summary>Gizmo is only displayed when its scene object is selected.</summary>
        Selected = 0x01,
        /// <summary>Gizmo is only displayed when its parent scene object is selected.</summary>
        ParentSelected = 0x02,
        /// <summary>Gizmo is only displayed when its scene object is not selected.</summary>
        NotSelected = 0x04,
        /// <summary>Gizmo can be clicked on in scene view, which will select its scene object.</summary>
        Pickable = 0x08
    }

    /// <summary>
    /// Notifies the runtime that the method this attribute is specified on serves for gizmo drawing. All drawing in the 
    /// method should be done using the <see cref="Gizmos"/> class. The method must be static and must accept a single
    /// parameter of type deriving from <see cref="Component"/>. Type of the parameter determines for which objects will
    /// the gizmo be drawn for.
    /// </summary>
    [AttributeUsage(AttributeTargets.Method)]
    public sealed class DrawGizmo : Attribute
    {
        #pragma warning disable 0414
        private DrawGizmoFlags flags;
        #pragma warning restore 0414

        /// <summary>
        /// Creates a new draw gizmos attribute.
        /// </summary>
        /// <param name="flags">Flags that control how and when the gizmos is drawn.</param>
        public DrawGizmo(DrawGizmoFlags flags)
        {
            this.flags = flags;
        }
    }

    /** @} */
}
