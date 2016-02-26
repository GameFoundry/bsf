//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
using System.Runtime.InteropServices;

namespace BansheeEngine
{
    /// <summary>
    /// Special physics controller meant to be used for game characters. Uses the "slide-and-collide" physics instead of
    /// of the standard physics model to handle various issues with manually moving kinematic objects.Uses a capsule to
    /// represent the character's bounds. 
    /// </summary>
    public class CharacterController : Component
    {
        [SerializeField]
        internal SerializableData serializableData = new SerializableData();

        // TODO

        /// <summary>
        /// Holds all data the character controller component needs to persist through serialization.
        /// </summary>
        [SerializeObject]
        internal class SerializableData
        {
            public Vector3 position;
            public float contactOffset = 0.1f;
            public float stepOffset = 0.5f;
            public Radian slopeLimit = new Degree(45.0f);
            public float minMoveDistance = 0.0f;
            public float height = 0.0f;
            public float radius = 1.0f;
            public Vector3 up = Vector3.YAxis;
            public CharacterClimbingMode climbingMode = CharacterClimbingMode.Normal;
            public CharacterNonWalkableMode nonWalkableMode = CharacterNonWalkableMode.Prevent;
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
    internal struct ScriptCharacterControllerData
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
        public Collider collider;
        public int triangleIndex;
        public CharacterController controller;
        public bool isControllerCollision;
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

    /** Contains data about a collision between two character controllers. */
    public class ControllerControllerCollision : ControllerCollision
    {
        /// <summary>
        /// Controller that was touched.
        /// </summary>
        public CharacterController controller;
    };
}