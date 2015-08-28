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

            Gizmos.DrawCube(target.SceneObject.Position, new Vector3(1, 1, 1));
            Gizmos.DrawSphere(target.SceneObject.Position + 2 * Vector3.XAxis, 1.0f);
            Gizmos.DrawWireCube(target.SceneObject.Position + 4 * Vector3.XAxis, new Vector3(1, 1, 1));
            Gizmos.DrawWireSphere(target.SceneObject.Position + 6 * Vector3.XAxis, 1.0f);
            Gizmos.DrawLine(target.SceneObject.Position + 7.5f * Vector3.XAxis,
                target.SceneObject.Position + 8.5f * Vector3.XAxis);
            Gizmos.DrawFrustum(target.SceneObject.Position + 10 * Vector3.XAxis, 1920.0f / 1080.0f, 90, 1.0f, 1000.0f);

            Gizmos.DrawIcon(target.SceneObject.Position + new Vector3(0, 10, 0), iconTexture, false);
        }
    }
}
