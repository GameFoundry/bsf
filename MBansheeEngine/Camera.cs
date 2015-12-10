using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Text;

namespace BansheeEngine
{
    /// <summary>
    /// Camera component that determines how is world geometry projected onto a 2D surface. You may position and orient it 
    /// in space, set options like aspect ratio and field or view and it outputs view and projection matrices required for 
    /// rendering.
    /// </summary>
    [RunInEditor]
    public sealed class Camera : Component
    {
        private NativeCamera native;

        [SerializeField]
        internal SerializableData serializableData = new SerializableData();

        /// <summary>
        /// Returns the non-component version of Camera that is wrapped by this component. 
        /// </summary>
        internal NativeCamera Native
        {
            get { return native; }
        }

        /// <summary>
        /// Ratio between viewport width and height (width / height).
        /// </summary>
        public float AspectRatio
        {
            get { return native.aspectRatio; }
            set { native.aspectRatio = value; serializableData.aspectRatio = value; }
        }

        /// <summary>
        /// Distance from the frustum origin to the near clipping plane. Anything closer than the near clipping plane will 
        /// not be rendered. Decreasing this value decreases depth buffer precision.
        /// </summary>
        public float NearClipPlane
        {
            get { return native.nearClipPlane; }
            set { native.nearClipPlane = value; serializableData.nearClipPlane = value; }
        }

        /// <summary>
        /// Distance from the frustum origin to the far clipping plane. Anything farther than the far clipping plane will 
        /// not be rendered. Increasing this value decreases depth buffer precision.
        /// </summary>
        public float FarClipPlane
        {
            get { return native.farClipPlane; }
            set { native.farClipPlane = value; serializableData.farClipPlane = value; }
        }

        /// <summary>
        /// Horizontal field of view. This determines how wide the camera viewing angle is along the horizontal axis. 
        /// Vertical FOV is calculated from the horizontal FOV and the aspect ratio.
        /// </summary>
        public Degree FieldOfView
        {
            get { return native.fieldOfView; }
            set { native.fieldOfView = value; serializableData.fieldOfView = value; }
        }

        /// <summary>
        /// Returns the area of the render target that the camera renders to, in normalized coordinates. 
        /// </summary>
        public Rect2 ViewportRect
        {
            get { return native.viewportRect; }
            set { native.viewportRect = value; serializableData.viewportRect = value; }
        }

        /// <summary>
        /// Projection type that controls how is 3D geometry projected onto a 2D plane.
        /// </summary>
        public ProjectionType ProjectionType
        {
            get { return native.projectionType; }
            set { native.projectionType = value; serializableData.projectionType = value; }
        }

        /// <summary>
        /// Ortographic height that controls the size of the displayed objects. This value is only relevant when projection
        /// type is set to orthographic. Setting this value will automatically recalculate ortographic width depending on
        /// the aspect ratio.
        /// </summary>
        public float OrthoHeight
        {
            get { return native.orthoHeight; }
            set { native.orthoHeight = value; serializableData.orthoHeight = value; }
        }

        /// <summary>
        /// Returns the ortographic width that controls the size of the displayed object. To change this value modify
        /// <see cref="OrthoHeight"/> or <see cref="AspectRatio"/>.
        /// </summary>
        public float OrthoWidth
        {
            get { return native.orthoWidth; }
        }

        /// <summary>
        /// Color that will be used for clearing the camera's viewport before rendering. Only relevant if color clear is
        /// enabled.
        /// </summary>
        public Color ClearColor
        {
            get { return native.clearColor; }
            set { native.clearColor = value; serializableData.clearColor = value; }
        }

        /// <summary>
        /// Value that will be used for clearing the camera's depth buffer before rendering. Only relevant if depth clear
        /// is enabled.
        /// </summary>
        public float ClearDepth
        {
            get { return native.clearDepth; }
            set { native.clearDepth = value; serializableData.clearDepth = value; }
        }

        /// <summary>
        /// Value that will be used for clearing the camera's stencil buffer before rendering. Only relevant if stencil
        /// clear is enabled.
        /// </summary>
        public UInt16 ClearStencil
        {
            get { return native.clearStencil; }
            set { native.clearStencil = value; serializableData.clearStencil = value; }
        }

        /// <summary>
        /// Flags that control which portions of the camera viewport, if any, are cleared before rendering.
        /// </summary>
        public ClearFlags ClearFlags
        {
            get { return native.clearFlags; }
            set { native.clearFlags = value; serializableData.clearFlags = value; }
        }

        /// <summary>
        /// Determines in which orders are the cameras rendered. This only applies to cameras rendering to the same render 
        /// target. Higher value means the camera will be processed sooner.
        /// </summary>
        public int Priority
        {
            get { return native.priority; }
            set { native.priority = value; serializableData.priority = value; }
        }

        /// <summary>
        /// Sets layer bitfield that is used when determining which object should the camera render. Renderable objects
        /// have their own layer flags that can be set depending on which camera you want to render them in.
        /// </summary>
        public UInt64 Layers
        {
            get { return native.layers; }
            set { native.layers = value; serializableData.layers = value; }
        }

        /// <summary>
        /// Returns the standard projection matrix that determines how are 3D points projected to two dimensions. The layout
        /// of this matrix depends on currently used render system.
        /// </summary>
        public Matrix4 ProjMatrix
        {
            get { return native.projMatrix; }
        }

        /// <summary>
        /// Returns the inverse of the standard projection matrix that determines how are 3D points projected to two 
        /// dimensions. The layout of this matrix depends on currently used render system.
        /// </summary>
        public Matrix4 ProjMatrixInverse
        {
            get { return native.projMatrixInv; }
        }

        /// <summary>
        /// Returns the view matrix that controls camera position and orientation.
        /// </summary>
        public Matrix4 ViewMatrix
        {
            get { return native.viewMatrix; }
        }

        /// <summary>
        /// Returns the inverse of the view matrix that controls camera position and orientation.
        /// </summary>
        public Matrix4 ViewMatrixInverse
        {
            get { return native.viewMatrixInv; }
        }

        /// <summary>
        /// Returns the width of the camera's viewport, in pixels. Only valid if render target is currently set.
        /// </summary>
        public int WidthPixels
        {
            get { return native.widthPixels; }
        }

        /// <summary>
        /// Returns the height of the camera's viewport, in pixels. Only valid if render target is currently set.
        /// </summary>
        public int HeightPixels
        {
            get { return native.heightPixels; }
        }

        /// <summary>
        /// Returns the render target that the camera will output all rendered pixels to.
        /// </summary>
        public RenderTarget Target
        {
            get { return native.target; }
            set { native.target = value; }
        }

        /// <summary>
        /// Determines if this is the main application camera. Main camera controls the final render surface that is 
        /// displayed to the user.
        /// </summary>
        public bool Main
        {
            get { return native.main; }
            set { native.main = value; serializableData.main = value; }
        }

        /// <summary>
        /// Converts a point in world space to screen coordinates.
        /// </summary>
        /// <param name="value">3D point in world space.</param>
        /// <returns>2D point on the render target attached to the camera, in pixels.</returns>
        public Vector2I WorldToScreen(Vector3 value) { return native.WorldToScreen(value); }

        /// <summary>
        /// Converts a point in world space to clip space coordinates.
        /// </summary>
        /// <param name="value">3D point in world space.</param>
        /// <returns>2D point in normalized coordinates ([0, 1] range), relative to the camera's viewport.</returns>
        public Vector2 WorldToClip(Vector3 value) { return native.WorldToClip(value); }

        /// <summary>
        /// Converts a point in world space to view space coordinates.
        /// </summary>
        /// <param name="value">3D point in world space.</param>
        /// <returns>3D point relative to the camera's coordinate system.</returns>
        public Vector3 WorldToView(Vector3 value) { return native.WorldToView(value); }

        /// <summary>
        /// Converts a point in screen space to a point in world space.
        /// </summary>
        /// <param name="value">2D point on the render target attached to the camera, in pixels.</param>
        /// <param name="depth">Depth at which place the world point at. The depth is applied to the vector going from 
        ///                     camera origin to the point on the near plane.</param>
        /// <returns>3D point in world space.</returns>
        public Vector3 ScreenToWorld(Vector2I value, float depth = 0.5f) { return native.ScreenToWorld(value, depth); }

        /// <summary>
        /// Converts a point in screen space to a point in view space.
        /// </summary>
        /// <param name="value">2D point on the render target attached to the camera, in pixels.</param>
        /// <param name="depth">Depth at which place the view point at. The depth is applied to the vector going from 
        ///                     camera origin to the point on the near plane.</param>
        /// <returns>3D point in view space.</returns>
        public Vector3 ScreenToView(Vector2I value, float depth = 0.5f) { return native.ScreenToView(value, depth); }

        /// <summary>
        /// Converts a point in screen space to a point in normalized clip space.
        /// </summary>
        /// <param name="value">2D point on the render target attached to the camera, in pixels.</param>
        /// <returns>2D point in normalized cliped space ([0, 1] range), relative to the camera's viewport.</returns>
        public Vector2 ScreenToClip(Vector2I value) { return native.ScreenToClip(value); }

        /// <summary>
        /// Converts a point relative to camera's coordinate system (view space) into a point in world space.
        /// </summary>
        /// <param name="value">3D point in view space.</param>
        /// <returns>3D point in world space.</returns>
        public Vector3 ViewToWorld(Vector3 value) { return native.ViewToWorld(value); }

        /// <summary>
        /// Converts a point relative to camera's coordinate system (view space) to screen coordinates.
        /// </summary>
        /// <param name="value">3D point in view space.</param>
        /// <returns>2D point on the render target attached to the camera, in pixels.</returns>
        public Vector2I ViewToScreen(Vector3 value) { return native.ViewToScreen(value); }

        /// <summary>
        /// Converts a point relative to camera's coordinate system (view space) to normalized clip space.
        /// </summary>
        /// <param name="value">3D point in view space.</param>
        /// <returns>2D point in normalized cliped space ([0, 1] range), relative to the camera's viewport.</returns>
        public Vector2 ViewToClip(Vector3 value) { return native.ViewToClip(value); }

        /// <summary>
        /// Converts a point relative to camera's viewport in normalized clip space ([0, 1] range) into a point in 
        /// world space.
        /// </summary>
        /// <param name="value">2D point in normalized clip space.</param>
        /// <param name="depth">Depth at which place the world point at. The depth is applied to the vector going from
        ///                     camera origin to the point on the near plane.</param>
        /// <returns>3D point in world space.</returns>
        public Vector3 ClipToWorld(Vector2 value, float depth = 0.5f) { return native.ClipToWorld(value, depth); }

        /// <summary>
        /// Converts a point relative to camera's viewport in normalized clip space ([0, 1] range) into a point in 
        /// view space.
        /// </summary>
        /// <param name="value">2D point in normalized clip space.</param>
        /// <param name="depth">Depth at which place the world point at. The depth is applied to the vector going from
        ///                     camera origin to the point on the near plane.</param>
        /// <returns>3D point in view space.</returns>
        public Vector3 ClipToView(Vector2 value, float depth = 0.5f) { return native.ClipToView(value, depth); }

        /// <summary>
        /// Converts a point relative to camera's viewport in normalized clip space ([0, 1] range) to screen space.
        /// </summary>
        /// <param name="value">2D point in normalized clip space.</param>
        /// <returns>2D point on the render target attached to the camera, in pixels.</returns>
        public Vector2I ClipToScreen(Vector2 value) { return native.ClipToScreen(value); }

        /// <summary>
        /// Converts a point in screen space in a ray in world space.
        /// </summary>
        /// <param name="value">2D point on the render target attached to the camera, in pixels.</param>
        /// <returns>A ray in world space with it's origin the selected point at the near frustum plane, pointing in the 
        ///          direction going from camera's origin towards a point on the near frustum plane.</returns>
        public Ray ScreenToWorldRay(Vector2I value) { return native.ScreenToWorldRay(value); }

        /// <summary>
        /// Projects a point in view space to a point in clip space. Similar to <see cref="ViewToClip"/> but preserves
        /// the depth component.
        /// </summary>
        /// <param name="value">3D point in view space.</param>
        /// <returns>3D point in normalized cliped space ([0, 1] range), relative to the camera's viewport. Z value
        ///          range depends on active render API.</returns>
        public Vector3 ProjectPoint(Vector3 value) { return native.ProjectPoint(value); }

        /// <summary>
        /// Un-rpojects a point in clip space to a point in view space.
        /// </summary>
        /// <param name="value">3D point in normalized cliped space ([0, 1] range), relative to the camera's viewport. 
        ///                     Z value range depends on active render API.</param>
        /// <returns>3D point in view space.</returns>
        public Vector3 UnprojectPoint(Vector3 value) { return native.UnprojectPoint(value); }

        private void OnReset()
        {
            if (native != null)
                native.OnDestroy();

            native = new NativeCamera(SceneObject);

            // Restore saved values after reset
            native.aspectRatio = serializableData.aspectRatio;
            native.nearClipPlane = serializableData.nearClipPlane;
            native.farClipPlane = serializableData.farClipPlane;
            native.fieldOfView = serializableData.fieldOfView;
            native.viewportRect = serializableData.viewportRect;
            native.projectionType = serializableData.projectionType;
            native.orthoHeight = serializableData.orthoHeight;
            native.clearColor = serializableData.clearColor;
            native.clearDepth = serializableData.clearDepth;
            native.clearStencil = serializableData.clearStencil;
            native.clearFlags = serializableData.clearFlags;
            native.priority = serializableData.priority;
            native.layers = serializableData.layers;
            native.main = serializableData.main;

            // TODO - Make RenderTexture a resource so I can save/restore it?
        }

        private void OnUpdate()
        {
            native.UpdateView(SceneObject);
        }

        private void OnDestroy()
        {
            native.OnDestroy();
        }

        /// <summary>
        /// Holds all data the camera component needs to persist through serialization.
        /// </summary>
        [SerializeObject]
        internal class SerializableData
        {
            public float aspectRatio = 1.333f;
            public float nearClipPlane = 1.0f;
            public float farClipPlane = 1000.0f;
            public Degree fieldOfView = new Degree(60);
            public Rect2 viewportRect = new Rect2(0, 0, 1, 1);
            public ProjectionType projectionType = ProjectionType.Perspective;
            public float orthoHeight;
            public Color clearColor = new Color(143.0f / 255.0f, 111.0f / 255.0f, 0);
            public float clearDepth = 1.0f;
            public UInt16 clearStencil;
            public ClearFlags clearFlags = ClearFlags.Color | ClearFlags.Depth | ClearFlags.Stencil;
            public int priority;
            public UInt64 layers = 0xFFFFFFFFFFFFFFFF;
            public bool main;
        }
    }
}
