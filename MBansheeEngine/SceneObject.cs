using System;
using System.Runtime.CompilerServices;

namespace BansheeEngine
{
    // TODO - Unfinished
    public sealed class SceneObject : GameObject
    {
        public SceneObject parent
        {
            set { Internal_SetParent(mCachedPtr, value); }
            get { return Internal_GetParent(mCachedPtr); }
        }

        public Vector3 position
        {
            get
            {
                Vector3 value;
                Internal_GetPosition(mCachedPtr, out value);
                return value;
            }

            set
            {
                Internal_SetPosition(mCachedPtr, value);
            }
        }

        public Vector3 localPosition
        {
            get
            {
                Vector3 value;
                Internal_GetLocalPosition(mCachedPtr, out value);
                return value;
            }

            set
            {
                Internal_SetLocalPosition(mCachedPtr, value);
            }
        }

        public Quaternion rotation
        {
            get
            {
                Quaternion value;
                Internal_GetRotation(mCachedPtr, out value);
                return value;
            }

            set
            {
                Internal_SetRotation(mCachedPtr, value);
            }
        }

        public Quaternion localRotation
        {
            get
            {
                Quaternion value;
                Internal_GetLocalRotation(mCachedPtr, out value);
                return value;
            }

            set
            {
                Internal_SetLocalRotation(mCachedPtr, value);
            }
        }

        public Vector3 scale
        {
            get
            {
                Vector3 value;
                Internal_GetScale(mCachedPtr, out value);
                return value;
            }
        }

        public Vector3 localScale
        {
            get
            {
                Vector3 value;
                Internal_GetLocalScale(mCachedPtr, out value);
                return value;
            }

            set
            {
                Internal_SetLocalScale(mCachedPtr, value);
            }
        }

        // For internal use
        private SceneObject()
        {
            
        }

        public SceneObject(string name)
        {
            Internal_CreateInstance(this, name);
        }

        public T AddComponent<T>() where T : Component
        {
            return (T)Component.Internal_AddComponent(this, typeof (T));
        }

        public T GetComponent<T>() where T : Component
        {
            return (T)Component.Internal_GetComponent(this, typeof(T));
        }

        public Component[] GetComponents()
        {
            return Component.Internal_GetComponents(this);
        }

        public void RemoveComponent<T>() where T : Component
        {
            Component.Internal_RemoveComponent(this, typeof(T));
        }

        public int GetNumChildren()
        {
            return Internal_GetNumChildren(mCachedPtr);
        }

        public SceneObject GetChild(int idx)
        {
            return Internal_GetChild(mCachedPtr, idx);
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_CreateInstance(SceneObject instance, string name);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetParent(IntPtr nativeInstance, SceneObject parent);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern SceneObject Internal_GetParent(IntPtr nativeInstance);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern int Internal_GetNumChildren(IntPtr nativeInstance);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern SceneObject Internal_GetChild(IntPtr nativeInstance, int idx);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_GetPosition(IntPtr nativeInstance, out Vector3 value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_GetLocalPosition(IntPtr nativeInstance, out Vector3 value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_GetRotation(IntPtr nativeInstance, out Quaternion value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_GetLocalRotation(IntPtr nativeInstance, out Quaternion value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_GetScale(IntPtr nativeInstance, out Vector3 value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_GetLocalScale(IntPtr nativeInstance, out Vector3 value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetPosition(IntPtr nativeInstance, Vector3 value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetLocalPosition(IntPtr nativeInstance, Vector3 value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetRotation(IntPtr nativeInstance, Quaternion value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetLocalRotation(IntPtr nativeInstance, Quaternion value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetLocalScale(IntPtr nativeInstance, Vector3 value);
    }
}
