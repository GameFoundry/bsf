using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using BansheeEngine;

namespace BansheeEditor
{
    internal sealed class SceneWindow : EditorWindow
    {
        private Camera camera;
        private SceneCamera cameraController;
        private RenderTexture2D renderTexture;

        private GUIRenderTexture renderTextureGUI;
        private SceneViewHandler sceneViewHandler;

        public Camera GetCamera()
        {
            return camera;
        }

        internal SceneWindow()
        { }

        private void OnInitialize()
        {
            UpdateRenderTexture(Width, Height);
        }

        private void OnDestroy()
        {
            if (camera != null)
            {
                camera.sceneObject.Destroy();
            }
        }

        private bool ScreenToScenePos(Vector2I screenPos, out Vector2I scenePos)
        {
            scenePos = screenPos;
            Vector2I windowPos = ScreenToWindowPos(screenPos);

            Rect2I bounds = GUILayoutUtility.CalculateBounds(renderTextureGUI);

            if (bounds.Contains(windowPos))
            {
                scenePos.x = windowPos.x - bounds.x;
                scenePos.y = windowPos.y - bounds.y;

                return true;
            }

            return false;
        }

        private void EditorUpdate()
        {
            sceneViewHandler.Update();

            bool handleActive = false;
            if (Input.IsButtonUp(ButtonCode.MouseLeft))
            {
                if (sceneViewHandler.IsHandleActive())
                {
                    sceneViewHandler.ClearHandleSelection();
                    handleActive = true;
                }
            }

            if (!HasFocus)
                return;

            Vector2I scenePos;
            if (ScreenToScenePos(Input.PointerPosition, out scenePos))
            {
                if (Input.IsButtonDown(ButtonCode.MouseLeft))
                {
                    sceneViewHandler.TrySelectHandle(scenePos);
                }
                else if (Input.IsButtonUp(ButtonCode.MouseLeft))
                {
                    if (!handleActive)
                    {
                        bool ctrlHeld = Input.IsButtonHeld(ButtonCode.LeftControl) ||
                            Input.IsButtonHeld(ButtonCode.RightControl);

                        sceneViewHandler.PickObject(scenePos, ctrlHeld);
                    }
                }

                sceneViewHandler.UpdateHandle(scenePos, Input.PointerDelta);
            }
        }

        protected override void WindowResized(int width, int height)
        {
            UpdateRenderTexture(width, height);

            base.WindowResized(width, height);
        }

        protected override void FocusChanged(bool inFocus)
        {
            if (!inFocus)
            {
                sceneViewHandler.ClearHandleSelection();
            }
        }

        private void UpdateRenderTexture(int width, int height)
	    {
            width = MathEx.Max(20, width);
            height = MathEx.Max(20, height);

            renderTexture = new RenderTexture2D(PixelFormat.R8G8B8A8, width, height);
            renderTexture.Priority = 1;

		    if (camera == null)
		    {
                SceneObject sceneCameraSO = new SceneObject("SceneCamera");
                camera = sceneCameraSO.AddComponent<Camera>();
                camera.target = renderTexture;
                camera.viewportRect = new Rect2(0.0f, 0.0f, 1.0f, 1.0f);

                sceneCameraSO.position = new Vector3(0, 0.5f, 1);
                sceneCameraSO.LookAt(new Vector3(0, 0, 0));

                camera.priority = 1;
                camera.nearClipPlane = 0.005f;
                camera.farClipPlane = 1000.0f;

                cameraController = sceneCameraSO.AddComponent<SceneCamera>();

                renderTextureGUI = new GUIRenderTexture(renderTexture);
                GUI.layout.AddElement(renderTextureGUI);

                sceneViewHandler = new SceneViewHandler(this, camera);
		    }
		    else
		    {
		        camera.target = renderTexture;
		        renderTextureGUI.RenderTexture = renderTexture;
		    }

		    // TODO - Consider only doing the resize once user stops resizing the widget in order to reduce constant
		    // render target destroy/create cycle for every single pixel.

		    camera.aspectRatio = width / (float)height;
	    }
    }
}
