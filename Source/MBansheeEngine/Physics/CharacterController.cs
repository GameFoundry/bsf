//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
using System;
using System.Runtime.InteropServices;

namespace BansheeEngine
{
    /** @addtogroup Physics
     *  @{
     */

    /// <summary>
    /// Special physics controller meant to be used for game characters. Uses the "slide-and-collide" physics instead of
    /// of the standard physics model to handle various issues with manually moving kinematic objects.Uses a capsule to
    /// represent the character's bounds. 
    /// </summary>
    public sealed class CharacterController : Component
    {
        internal NativeCharacterController native;

        [SerializeField]
        internal SerializableData serializableData = new SerializableData();

        /// <summary>
        /// Triggered when the controller hits a collider.
        /// </summary>
        public event Action<ControllerColliderCollision> OnColliderHit;

        /// <summary>
        /// Triggered when the controller hits another character controller.
        /// </summary>
        public event Action<ControllerControllerCollision> OnControllerHit;

        /// <summary>
        /// Position of the bottom of the controller. Position takes contact offset into account. Changing this value will 
        /// teleport the character to the location. Use <see cref="Move"/> for movement that includes physics.
        /// </summary>
        public Vector3 FootPosition
        {
            get
            {
                if (native != null)
                    return native.FootPosition;

                return Vector3.Zero;
            }
            set
            {
                if (native != null)
                {
                    native.FootPosition = value;
                    UpdatePositionFromController();
                }
            }
        }

        /// <summary>
        /// Radius of the controller capsule.
        /// </summary>
        public float Radius
        {
            get { return serializableData.radius; }
            set
            {
                serializableData.radius = value;

                if(native != null)
                    UpdateDimensions();
            }
        }

        /// <summary>
        /// Height between the centers of the two spheres of the controller capsule.
        /// </summary>
        public float Height
        {
            get { return serializableData.height; }
            set
            {
                serializableData.height = value;

                if(native != null)
                    UpdateDimensions();
            }
        }

        /// <summary>
        /// Up direction of capsule. Determines capsule orientation.
        /// </summary>
        public Vector3 Up
        {
            get { return serializableData.up; }
            set
            {
                serializableData.up = value;

                if (native != null)
                    native.Up = value;
            }
        }

        /// <summary>
        /// Controls what happens when character encounters a height higher than its step offset. 
        /// </summary>
        public CharacterClimbingMode ClimbingMode
        {
            get { return serializableData.climbingMode; }
            set
            {
                serializableData.climbingMode = value;

                if (native != null)
                    native.ClimbingMode = value;
            }
        }

        /// <summary>
        /// Controls what happens when character encounters a slope higher than its slope offset. 
        /// </summary>
        public CharacterNonWalkableMode NonWalkableMode
        {
            get { return serializableData.nonWalkableMode; }
            set
            {
                serializableData.nonWalkableMode = value;

                if (native != null)
                    native.NonWalkableMode = value;
            }
        }

        /// <summary>
        /// Represents minimum distance that the character will move during a call to <see cref="Move"/>. This is used to
        /// stop the recursive motion algorithm when the remaining distance is too small.
        /// </summary>
        public float MinMoveDistance
        {
            get { return serializableData.minMoveDistance; }
            set
            {
                serializableData.minMoveDistance = value;

                if (native != null)
                    native.MinMoveDistance = value;
            }
        }

        /// <summary>
        /// Contact offset specifies a skin around the object within which contacts will be generated. It should be a small
        /// positive non-zero value.
        /// </summary>
        public float ContactOffset
        {
            get { return serializableData.contactOffset; }
            set
            {
                serializableData.contactOffset = value;

                if (native != null)
                    native.ContactOffset = value;
            }
        }

        /// <summary>
        /// Controls which obstacles will the character be able to automatically step over without being stopped. This is 
        /// the height of the maximum obstacle that will be stepped over (with exceptions, <see cref="ClimbingMode"/>).
        /// </summary>
        public float StepOffset
        {
            get { return serializableData.stepOffset; }
            set
            {
                serializableData.stepOffset = value;

                if (native != null)
                    native.StepOffset = value;
            }
        }

        /// <summary>
        /// Controls which slopes should the character consider too steep and won't be able to move over. 
        /// <see cref="NonWalkableMode"/> for more information.
        /// </summary>
        public Radian SlopeLimit
        {
            get { return serializableData.slopeLimit; }
            set
            {
                serializableData.slopeLimit = value;

                if (native != null)
                    native.SlopeLimit = value;
            }
        }

        /// <summary>
        /// Determines what can the controller collide with. Objects that are allowed to collide with this object must have
        /// the same bits set in their own layers.
        /// </summary>
        public ulong Layer
        {
            get { return serializableData.layer; }
            set
            {
                serializableData.layer = value;

                if (native != null)
                    native.Layer = value;
            }
        }

        /// <summary>
        /// Moves the controller in the specified direction by the specified amount, while interacting with surrounding
        /// geometry.Returns flags signaling where collision occurred after the movement.
        /// 
        /// Does not account for gravity, you must apply it manually.
        /// </summary>
        /// <param name="position">Position to move the controller to, in world space.</param>
        public CharacterCollisionFlag Move(Vector3 position)
        {
            if (native == null)
                return 0;

            CharacterCollisionFlag output = native.Move(position);
            UpdatePositionFromController();

            return output;
        }

        /// <summary>
        /// Triggered when the controller hits a collider.
        /// </summary>
        /// <param name="data">Data about the collision.</param>
        internal void DoOnColliderHit(ControllerColliderCollision data)
        {
            if (OnColliderHit != null)
                OnColliderHit(data);
        }

        /// <summary>
        /// Triggered when the controller hits another character controller.
        /// </summary>
        /// <param name="data">Data about the collision.</param>
        internal void DoOnControllerHit(ControllerControllerCollision data)
        {
            if (OnControllerHit != null)
                OnControllerHit(data);
        }

        private void OnInitialize()
        {
            NotifyFlags = TransformChangedFlags.Transform;
        }

        private void OnEnable()
        {
            RestoreNative();
        }

        private void OnDisable()
        {
            DestroyNative();
        }

        private void OnDestroy()
        {
            DestroyNative();
        }

        private void OnTransformChanged(TransformChangedFlags flags)
        {
            if (!SceneObject.Active || native == null)
                return;

            native.Position = SceneObject.Position;
        }

        /// <summary>
        /// Updates the position by copying it from the controller to the component's scene object.
        /// </summary>
        private void UpdatePositionFromController()
        {
            NotifyFlags = 0;
            SceneObject.Position = native.Position;
            NotifyFlags = TransformChangedFlags.Transform;
        }

        /// <summary>
        /// Updates the dimensions of the controller by taking account scale of the parent scene object.
        /// </summary>
        private void UpdateDimensions()
        {
            Vector3 scale = SceneObject.Scale;
            float height = serializableData.height * MathEx.Abs(scale.y);
            float radius = serializableData.radius * MathEx.Abs(MathEx.Max(scale.x, scale.z));

            native.Height = height;
            native.Radius = radius;
        }

        /// <summary>
        /// Restores the internal character controller representation and initializes it with data stored by the component.
        /// </summary>
        private void RestoreNative()
        {
            ScriptCharacterControllerData initData = new ScriptCharacterControllerData();
            initData.position = SceneObject.Position;
            initData.contactOffset = serializableData.contactOffset;
            initData.stepOffset = serializableData.stepOffset;
            initData.slopeLimit = serializableData.slopeLimit;
            initData.minMoveDistance = serializableData.minMoveDistance;
            initData.height = serializableData.height;
            initData.radius = serializableData.radius;
            initData.up = serializableData.up;
            initData.climbingMode = serializableData.climbingMode;
            initData.nonWalkableMode = serializableData.nonWalkableMode;

            native = new NativeCharacterController(initData);
            native.Component = this;
            native.Layer = serializableData.layer;

            UpdateDimensions();
        }

        /// <summary>
        /// Destroys the internal character controller representation.
        /// </summary>
        private void DestroyNative()
        {
            if (native != null)
            {
                native.Destroy();
                native = null;
            }
        }

        /// <summary>
        /// Holds all data the character controller component needs to persist through serialization.
        /// </summary>
        [SerializeObject]
        internal class SerializableData
        {
            public float contactOffset = 0.1f;
            public float stepOffset = 0.5f;
            public Radian slopeLimit = new Degree(45.0f);
            public float minMoveDistance = 0.0f;
            public float height = 1.0f;
            public float radius = 0.25f;
            public Vector3 up = Vector3.YAxis;
            public CharacterClimbingMode climbingMode = CharacterClimbingMode.Normal;
            public CharacterNonWalkableMode nonWalkableMode = CharacterNonWalkableMode.Prevent;
            public ulong layer = 1;
        }
    }

    /// <summary>
    /// Controls climbing behaviour for a capsule character controller. Normally the character controller will not
    /// automatically climb when heights are greater than the assigned step offset.However due to the shape of the capsule
    /// it might automatically climb over slightly larger heights than assigned step offsets.
    /// </summary>
    public enum CharacterClimbingMode
    {
        /// <summary>
        /// Normal behaviour. Capsule character controller will be able to auto-step even above the step offset.
        /// </summary>
        Normal,
        /// <summary>
        /// The system will attempt to limit auto-step to the provided step offset and no higher.
        /// </summary>
		Constrained
    }

    /// <summary>
    /// Controls behaviour when a character controller reaches a slope thats larger than its slope offset.
    /// </summary>
    public enum CharacterNonWalkableMode
    {
        /// <summary>
        /// Character will be prevented from going further, but will be allowed to move laterally.
        /// </summary>
        Prevent,
        /// <summary>
        /// Character will be prevented from going further, but also slide down the slope.
        /// </summary>
		PreventAndSlide
    }

    /// <summary>
    /// Reports in which directions is the character colliding with other objects.
    /// </summary>
    public enum CharacterCollisionFlag
    {
        /// <summary>
        /// Character is colliding with its sides. 
        /// </summary>
        Sides = 0x1,
        /// <summary>
        /// Character is colliding with the ceiling.
        /// </summary>
		Up = 0x2,
        /// <summary>
        /// Character is colliding with the ground.
        /// </summary>
		Down = 0x4
	}

    /// <summary>
    /// Used for passing CharacterController initialization data between native and managed code.
    /// </summary>
    [StructLayout(LayoutKind.Sequential)]
    internal struct ScriptCharacterControllerData // Note: Must match C++ struct CHAR_CONTROLLER_DESC
    {
        public Vector3 position;
        public float contactOffset;
        public float stepOffset;
        public Radian slopeLimit;
        public float minMoveDistance;
        public float height;
        public float radius;
        public Vector3 up;
        public CharacterClimbingMode climbingMode;
        public CharacterNonWalkableMode nonWalkableMode;
    }

    /// <summary>
    /// Used for passing ControllerCollision data between native and managed code.
    /// </summary>
    internal struct ScriptControllerCollision // Note: Must match C++ struct ScriptControllerCollision
    {
        public Vector3 position;
        public Vector3 normal;
        public Vector3 motionDir;
        public float motionAmount;
        public NativeCollider collider;
        public int triangleIndex;
        public NativeCharacterController controller;
    }

    /// <summary>
    /// Contains data about a collision of a character controller and another object.
    /// </summary>
    public class ControllerCollision
    {
        /// <summary>
        /// Contact position.
        /// </summary>
        public Vector3 position;

        /// <summary>
        /// Contact normal.
        /// </summary>
        public Vector3 normal;

        /// <summary>
        /// Direction of motion after the hit.
        /// </summary>
        public Vector3 motionDir;

        /// <summary>
        /// Magnitude of motion after the hit.
        /// </summary>
        public float motionAmount;
    };

    /// <summary>
    /// Contains data about a collision of a character controller and a collider.
    /// </summary>
    public class ControllerColliderCollision : ControllerCollision
    {
        /// <summary>
        /// Collider that was touched. 
        /// </summary>
        public Collider collider;

        /// <summary>
        /// Touched triangle index for mesh colliders.
        /// </summary>
        public int triangleIndex;
    };

    /// <summary>
    /// Contains data about a collision between two character controllers.
    /// </summary>
    public class ControllerControllerCollision : ControllerCollision
    {
        /// <summary>
        /// Controller that was touched.
        /// </summary>
        public CharacterController controller;
    };

    /** @} */
}