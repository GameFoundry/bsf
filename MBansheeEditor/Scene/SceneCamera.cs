using BansheeEngine;

namespace BansheeEditor
{
    /// <summary>
    /// Handles camera movement in the scene view.
    /// </summary>
    [RunInEditor]
    internal sealed class SceneCamera : Component
    {
        public const string MoveForwardBinding = "SceneForward";
	    public const string MoveLeftBinding = "SceneLeft";
	    public const string MoveRightBinding = "SceneRight";
	    public const string MoveBackBinding = "SceneBackward";
	    public const string FastMoveBinding = "SceneFastMove";
	    public const string RotateBinding = "SceneRotate";
	    public const string HorizontalAxisBinding = "SceneHorizontal";
	    public const string VerticalAxisBinding = "SceneVertical";

	    private const float StartSpeed = 4.0f;
	    private const float TopSpeed = 12.0f;
	    private const float Acceleration = 1.0f;
	    private const float FastModeMultiplier = 2.0f;
	    private const float RotationalSpeed = 360.0f; // Degrees/second

        private VirtualButton moveForwardBtn;
        private VirtualButton moveLeftBtn;
        private VirtualButton moveRightBtn;
        private VirtualButton moveBackwardBtn;
        private VirtualButton fastMoveBtn;
        private VirtualButton activeBtn;
        private VirtualAxis horizontalAxis;
        private VirtualAxis verticalAxis;

        private float currentSpeed;
        private Degree yaw;
        private Radian pitch;
        private bool lastButtonState;

        private void OnReset()
        {
            moveForwardBtn = new VirtualButton(MoveForwardBinding);
            moveLeftBtn = new VirtualButton(MoveLeftBinding);
            moveRightBtn = new VirtualButton(MoveRightBinding);
            moveBackwardBtn = new VirtualButton(MoveBackBinding);
            fastMoveBtn = new VirtualButton(FastMoveBinding);
            activeBtn = new VirtualButton(RotateBinding);
            horizontalAxis = new VirtualAxis(HorizontalAxisBinding);
            verticalAxis = new VirtualAxis(VerticalAxisBinding);
        }

        private void OnDisable()
        {
            if (VirtualInput.IsButtonHeld(activeBtn))
                Cursor.Show();
        }

        private void Update()
        {
		    bool goingForward = VirtualInput.IsButtonHeld(moveForwardBtn);
		    bool goingBack = VirtualInput.IsButtonHeld(moveBackwardBtn);
		    bool goingLeft = VirtualInput.IsButtonHeld(moveLeftBtn);
		    bool goingRight = VirtualInput.IsButtonHeld(moveRightBtn);
		    bool fastMove = VirtualInput.IsButtonHeld(fastMoveBtn);
		    bool camActive = VirtualInput.IsButtonHeld(activeBtn);

            if (camActive != lastButtonState)
            {
                if (camActive)
                    Cursor.Hide();
                else
                    Cursor.Show();

                lastButtonState = camActive;
            }

		    float frameDelta = Time.FrameDelta;
		    if (camActive)
		    {
		        float horzValue = VirtualInput.GetAxisValue(horizontalAxis);
                float vertValue = VirtualInput.GetAxisValue(verticalAxis);

                yaw += new Degree(horzValue * RotationalSpeed * frameDelta);
                pitch += new Degree(vertValue * RotationalSpeed * frameDelta);

                yaw = MathEx.WrapAngle(yaw);
                pitch = MathEx.WrapAngle(pitch);

		        Quaternion yRot = Quaternion.FromAxisAngle(Vector3.YAxis, yaw);
                Quaternion xRot = Quaternion.FromAxisAngle(Vector3.XAxis, pitch);

                Quaternion camRot = yRot * xRot;
                camRot.Normalize();

                SceneObject.Rotation = camRot;

                Vector3 direction = Vector3.Zero;
                if (goingForward) direction += SceneObject.Forward;
                if (goingBack) direction -= SceneObject.Forward;
                if (goingRight) direction += SceneObject.Right;
                if (goingLeft) direction -= SceneObject.Right;

                if (direction.SqrdLength != 0)
                {
                    direction.Normalize();

                    float multiplier = 1.0f;
                    if (fastMove)
                        multiplier = FastModeMultiplier;

                    currentSpeed = MathEx.Clamp(currentSpeed + Acceleration * frameDelta, StartSpeed, TopSpeed);
                    currentSpeed *= multiplier;
                }
                else
                {
                    currentSpeed = 0.0f;
                }

                const float tooSmall = 0.0001f;
                if (currentSpeed > tooSmall)
                {
                    Vector3 velocity = direction * currentSpeed;
                    SceneObject.Move(velocity * frameDelta);
                }
		    }
        }
    }
}
