using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using BansheeEngine;

namespace BansheeEditor
{
    class DbgGizmo
    {
        [DrawGizmo(DrawGizmoFlags.NotSelected | DrawGizmoFlags.Pickable)]
        private static void DrawDbgComponentGizmo(DbgGizmoComponent target)
        {
            Gizmos.DrawCube(target.sceneObject.position, new Vector3(1, 1, 1));
        }
    }
}
