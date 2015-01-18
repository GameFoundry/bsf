using System;
using System.Runtime.CompilerServices;

namespace BansheeEngine
{
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

        public Matrix4 worldTransform
        {
            get
            {
                Matrix4 value;
                Internal_GetWorldTransform(mCachedPtr, out value);
                return value;
            }
        }

        public Matrix4 localTransform
        {
            get
            {
                Matrix4 value;
                Internal_GetLocalTransform(mCachedPtr, out value);
                return value;
            }
        }

        public Vector3 forward
        {
            get
            {
                Vector3 value;
                Internal_GetForward(mCachedPtr, out value);
                return value;
            }
            set
            {
                Internal_SetForward(mCachedPtr, value);
            }
        }

        public Vector3 right
        {
            get
            {
                Vector3 value;
                Internal_GetRight(mCachedPtr, out value);
                return value;
            }
        }

        public Vector3 up
        {
            get
            {
                Vector3 value;
                Internal_GetUp(mCachedPtr, out value);
                return value;
            }
        }

        // For internal use by the runtime
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
            int value;
            Internal_GetNumChildren(mCachedPtr, out value);
            return value;
        }

        public SceneObject GetChild(int idx)
        {
            return Internal_GetChild(mCachedPtr, idx);
        }

        public void LookAt(Vector3 direction)
        {
            Internal_LookAt(mCachedPtr, direction, Vector3.yAxis);
        }

        public void LookAt(Vector3 direction, Vector3 up)
        {
            Internal_LookAt(mCachedPtr, direction, up);
        }

        public void Move(Vector3 amount)
        {
            Internal_Move(mCachedPtr, amount);
        }

        public void MoveLocal(Vector3 amount)
        {
            Internal_MoveLocal(mCachedPtr, amount);
        }

        public void Rotate(Quaternion amount)
        {
            Internal_Rotate(mCachedPtr, amount);
        }

        public void Roll(Degree angle)
        {
            Internal_Roll(mCachedPtr, angle);
        }

        public void Yaw(Degree angle)
        {
            Internal_Yaw(mCachedPtr, angle);
        }

        public void Pitch(Degree angle)
        {
            Internal_Pitch(mCachedPtr, angle);
        }

        public void Destroy(bool immediate = false)
        {
            Internal_Destroy(mCachedPtr, immediate);
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_CreateInstance(SceneObject instance, string name);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetParent(IntPtr nativeInstance, SceneObject parent);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern SceneObject Internal_GetParent(IntPtr nativeInstance);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_GetNumChildren(IntPtr nativeInstance, out int value);

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

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_GetLocalTransform(IntPtr nativeInstance, out Matrix4 value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_GetWorldTransform(IntPtr nativeInstance, out Matrix4 value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_LookAt(IntPtr nativeInstance, Vector3 direction, Vector3 up);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_Move(IntPtr nativeInstance, Vector3 value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_MoveLocal(IntPtr nativeInstance, Vector3 value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_Rotate(IntPtr nativeInstance, Quaternion value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_Roll(IntPtr nativeInstance, Radian value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_Yaw(IntPtr nativeInstance, Radian value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_Pitch(IntPtr nativeInstance, Radian value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetForward(IntPtr nativeInstance, Vector3 value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_GetForward(IntPtr nativeInstance, out Vector3 value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_GetUp(IntPtr nativeInstance, out Vector3 value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_GetRight(IntPtr nativeInstance, out Vector3 value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_Destroy(IntPtr nativeInstance, bool immediate);
    }
}
