using System.Collections.Generic;
using System.Runtime.CompilerServices;
using BansheeEngine;

namespace BansheeEditor
{
    public class EditorUtility
    {
        public static AABox CalculateBounds(SceneObject so)
        {
            AABox bounds;
            Internal_CalculateBounds(so, out bounds);
            return bounds;
        }

        public static AABox CalculateBounds(SceneObject[] objects)
        {
            AABox bounds;
            Internal_CalculateBoundsArray(objects, out bounds);
            return bounds;
        }

        public static SceneObject[] FlattenHierarchy(SceneObject so)
        {
            Stack<SceneObject> todo = new Stack<SceneObject>();
            todo.Push(so);

            List<SceneObject> flattenedHierarchy = new List<SceneObject>();
            while (todo.Count > 0)
            {
                SceneObject cur = todo.Pop();
                flattenedHierarchy.Add(cur);

                int numChildren = cur.GetNumChildren();
                for (int i = 0; i < numChildren; i++)
                    todo.Push(cur.GetChild(i));
            }

            return flattenedHierarchy.ToArray();
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_CalculateBounds(SceneObject so, out AABox bounds);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_CalculateBoundsArray(SceneObject[] objects, out AABox bounds);
    }
}
