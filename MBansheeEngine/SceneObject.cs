using System;
using System.Runtime.CompilerServices;

namespace BansheeEngine
{
    public sealed class SceneObject : GameObject
    {
        public SceneObject Parent
        {
            set { Internal_SetParent(mCachedPtr, value); }
            get { return Internal_GetParent(mCachedPtr); }
        }

        public Vector3 Position
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

        public Vector3 LocalPosition
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

        public Quaternion Rotation
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

        public Quaternion LocalRotation
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

        public Vector3 Scale
        {
            get
            {
                Vector3 value;
                Internal_GetScale(mCachedPtr, out value);
                return value;
            }
        }

        public Vector3 LocalScale
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

        public Matrix4 WorldTransform
        {
            get
            {
                Matrix4 value;
                Internal_GetWorldTransform(mCachedPtr, out value);
                return value;
            }
        }

        public Matrix4 LocalTransform
        {
            get
            {
                Matrix4 value;
                Internal_GetLocalTransform(mCachedPtr, out value);
                return value;
            }
        }

        public Vector3 Forward
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

        public Vector3 Right
        {
            get
            {
                Vector3 value;
                Internal_GetRight(mCachedPtr, out value);
                return value;
            }
        }

        public Vector3 Up
        {
            get
            {
                Vector3 value;
                Internal_GetUp(mCachedPtr, out value);
                return value;
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
