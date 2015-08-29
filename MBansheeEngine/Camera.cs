using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Text;

namespace BansheeEngine
{
    public class Camera : Component
    {
        private NativeCamera native;

        [SerializeField]
        private SerializableData serializableData = new SerializableData();

        internal NativeCamera Native
        {
            get { return native; }
        }

        public float AspectRatio
        {
            get { return native.aspectRatio; }
            set { native.aspectRatio = value; serializableData.aspectRatio = value; }
        }

        public float NearClipPlane
        {
            get { return native.nearClipPlane; }
            set { native.nearClipPlane = value; serializableData.nearClipPlane = value; }
        }

        public float FarClipPlane
        {
            get { return native.farClipPlane; }
            set { native.farClipPlane = value; serializableData.farClipPlane = value; }
        }

        public Degree FieldOfView
        {
            get { return native.fieldOfView; }
            set { native.fieldOfView = value; serializableData.fieldOfView = value; }
        }

        public Rect2 ViewportRect
        {
            get { return native.viewportRect; }
            set { native.viewportRect = value; serializableData.viewportRect = value; }
        }

        public ProjectionType ProjectionType
        {
            get { return native.projectionType; }
            set { native.projectionType = value; serializableData.projectionType = value; }
        }

        public float OrthoHeight
        {
            get { return native.orthoHeight; }
            set { native.orthoHeight = value; serializableData.orthoHeight = value; }
        }

        public float OrthoWidth
        {
            get { return native.orthoWidth; }
        }

        public Color ClearColor
        {
            get { return native.clearColor; }
            set { native.clearColor = value; serializableData.clearColor = value; }
        }

        public float ClearDepth
        {
            get { return native.clearDepth; }
            set { native.clearDepth = value; serializableData.clearDepth = value; }
        }

        public UInt16 ClearStencil
        {
            get { return native.clearStencil; }
            set { native.clearStencil = value; serializableData.clearStencil = value; }
        }

        public ClearFlags ClearFlags
        {
            get { return native.clearFlags; }
            set { native.clearFlags = value; serializableData.clearFlags = value; }
        }

        public int Priority
        {
            get { return native.priority; }
            set { native.priority = value; serializableData.priority = value; }
        }

        public UInt64 Layers
        {
            get { return native.layers; }
            set { native.layers = value; serializableData.layers = value; }
        }

        public Matrix4 ProjMatrix
        {
            get { return native.projMatrix; }
        }

        public Matrix4 ProjMatrixInverse
        {
            get { return native.projMatrixInv; }
        }

        public Matrix4 ViewMatrix
        {
            get { return native.viewMatrix; }
        }

        public Matrix4 ViewMatrixInverse
        {
            get { return native.viewMatrixInv; }
        }

        public int WidthPixels
        {
            get { return native.widthPixels; }
        }

        public int HeightPixels
        {
            get { return native.heightPixels; }
        }

        public RenderTarget Target
        {
            get { return native.target; }
            set { native.target = value; }
        }

        public Vector2I WorldToScreen(Vector3 value) { return native.WorldToScreen(value); }
        public Vector2 WorldToClip(Vector3 value) { return native.WorldToClip(value); }
        public Vector3 WorldToView(Vector3 value) { return native.WorldToView(value); }

        public Vector3 ScreenToWorld(Vector2I value, float depth = 0.5f) { return native.ScreenToWorld(value, depth); }
        public Vector3 ScreenToView(Vector2I value, float depth = 0.5f) { return native.ScreenToView(value, depth); }
        public Vector2 ScreenToClip(Vector2I value) { return native.ScreenToClip(value); }

        public Vector3 ViewToWorld(Vector3 value) { return native.ViewToWorld(value); }
        public Vector2I ViewToScreen(Vector3 value) { return native.ViewToScreen(value); }
        public Vector2 ViewToClip(Vector3 value) { return native.ViewToClip(value); }

        public Vector3 ClipToWorld(Vector2 value, float depth = 0.5f) { return native.ClipToWorld(value, depth); }
        public Vector3 ClipToView(Vector2 value, float depth = 0.5f) { return native.ClipToView(value, depth); }
        public Vector2I ClipToScreen(Vector2 value) { return native.ClipToScreen(value); }

        public Ray ScreenToWorldRay(Vector2I value) { return native.ScreenToWorldRay(value); }
        public Vector3 ProjectPoint(Vector3 value) { return native.ProjectPoint(value); }
        public Vector3 UnprojectPoint(Vector3 value) { return native.UnprojectPoint(value); }

        private void OnInitialize()
        {
            serializableData.aspectRatio = 1.333f;
            serializableData.nearClipPlane = 1.0f;
            serializableData.farClipPlane = 1000.0f;
            serializableData.fieldOfView = new Degree(60);
            serializableData.viewportRect = new Rect2(0, 0, 1, 1);
            serializableData.projectionType = ProjectionType.Perspective;
            serializableData.layers = 0xFFFFFFFFFFFFFFFF;
            serializableData.clearColor = new Color(143.0f / 255.0f, 111.0f / 255.0f, 0);
            serializableData.clearDepth = 1.0f;
            serializableData.clearFlags = ClearFlags.Color | ClearFlags.Depth | ClearFlags.Stencil;
        }

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

            // TODO - Make RenderTexture a resource so I can save/restore it?
        }

        private void Update()
        {
            native.UpdateView(SceneObject);
        }

        private void OnDestroy()
        {
            native.OnDestroy();
        }

        [SerializeObject]
        private struct SerializableData
        {
            public float aspectRatio;
            public float nearClipPlane;
            public float farClipPlane;
            public Degree fieldOfView;
            public Rect2 viewportRect;
            public ProjectionType projectionType;
            public float orthoHeight;
            public Color clearColor;
            public float clearDepth;
            public UInt16 clearStencil;
            public ClearFlags clearFlags;
            public int priority;
            public UInt64 layers;
        }
    }
}
