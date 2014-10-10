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
            Gizmos.DrawSphere(target.sceneObject.position + 2 * Vector3.xAxis, 1.0f);
            Gizmos.DrawWireCube(target.sceneObject.position + 4 * Vector3.xAxis, new Vector3(1, 1, 1));
            Gizmos.DrawWireSphere(target.sceneObject.position + 6 * Vector3.xAxis, 1.0f);
            Gizmos.DrawLine(target.sceneObject.position + 7.5f * Vector3.xAxis,
                target.sceneObject.position + 8.5f * Vector3.xAxis);
            Gizmos.DrawFrustum(target.sceneObject.position + 10 * Vector3.xAxis, 1920.0f / 1080.0f, 90, 1.0f, 1000.0f);
        }
    }
}
