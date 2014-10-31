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

            Gizmos.DrawCube(target.sceneObject.Position, new Vector3(1, 1, 1));
            Gizmos.DrawSphere(target.sceneObject.Position + 2 * Vector3.xAxis, 1.0f);
            Gizmos.DrawWireCube(target.sceneObject.Position + 4 * Vector3.xAxis, new Vector3(1, 1, 1));
            Gizmos.DrawWireSphere(target.sceneObject.Position + 6 * Vector3.xAxis, 1.0f);
            Gizmos.DrawLine(target.sceneObject.Position + 7.5f * Vector3.xAxis,
                target.sceneObject.Position + 8.5f * Vector3.xAxis);
            Gizmos.DrawFrustum(target.sceneObject.Position + 10 * Vector3.xAxis, 1920.0f / 1080.0f, 90, 1.0f, 1000.0f);

            Gizmos.DrawIcon(target.sceneObject.Position + new Vector3(0, 10, 0), iconTexture, false);
        }
    }
}
