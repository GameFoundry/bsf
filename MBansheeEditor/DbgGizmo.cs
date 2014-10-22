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
        private static SpriteTexture iconTexture;

        [DrawGizmo(DrawGizmoFlags.NotSelected | DrawGizmoFlags.Pickable)]
        private static void DrawDbgComponentGizmo(DbgGizmoComponent target)
        {
            if (iconTexture == null)
            {
                Texture2D iconTex = ProjectLibrary.Load<Texture2D>("debugIcon.psd");
                iconTexture = new SpriteTexture(iconTex);
            }

            Gizmos.DrawCube(target.sceneObject.position, new Vector3(1, 1, 1));
            Gizmos.DrawSphere(target.sceneObject.position + 2 * Vector3.xAxis, 1.0f);
            Gizmos.DrawWireCube(target.sceneObject.position + 4 * Vector3.xAxis, new Vector3(1, 1, 1));
            Gizmos.DrawWireSphere(target.sceneObject.position + 6 * Vector3.xAxis, 1.0f);
            Gizmos.DrawLine(target.sceneObject.position + 7.5f * Vector3.xAxis,
                target.sceneObject.position + 8.5f * Vector3.xAxis);
            Gizmos.DrawFrustum(target.sceneObject.position + 10 * Vector3.xAxis, 1920.0f / 1080.0f, 90, 1.0f, 1000.0f);

            Gizmos.DrawIcon(target.sceneObject.position + new Vector3(0, 10, 0), iconTexture, false);
        }
    }
}
