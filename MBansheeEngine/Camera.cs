using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Text;

namespace BansheeEngine
{
    public class Camera : Component
    {
        private CameraHandler handler;

        [SerializeField]
        private SerializableData serializableData = new SerializableData();

        internal CameraHandler Handler
        {
            get { return handler; }
        }

        public float AspectRatio
        {
            get { return handler.aspectRatio; }
            set { handler.aspectRatio = value; serializableData.aspectRatio = value; }
        }

        public float NearClipPlane
        {
            get { return handler.nearClipPlane; }
            set { handler.nearClipPlane = value; serializableData.nearClipPlane = value; }
        }

        public float FarClipPlane
        {
            get { return handler.farClipPlane; }
            set { handler.farClipPlane = value; serializableData.farClipPlane = value; }
        }

        public Degree FieldOfView
        {
            get { return handler.fieldOfView; }
            set { handler.fieldOfView = value; serializableData.fieldOfView = value; }
        }

        public Rect2 ViewportRect
        {
            get { return handler.viewportRect; }
            set { handler.viewportRect = value; serializableData.viewportRect = value; }
        }

        public ProjectionType ProjectionType
        {
            get { return handler.projectionType; }
            set { handler.projectionType = value; serializableData.projectionType = value; }
        }

        public float OrthoHeight
        {
            get { return handler.orthoHeight; }
            set { handler.orthoHeight = value; serializableData.orthoHeight = value; }
        }

        public float OrthoWidth
        {
            get { return handler.orthoWidth; }
        }

        public Color ClearColor
        {
            get { return handler.clearColor; }
            set { handler.clearColor = value; serializableData.clearColor = value; }
        }

        public float ClearDepth
        {
            get { return handler.clearDepth; }
            set { handler.clearDepth = value; serializableData.clearDepth = value; }
        }

        public UInt16 ClearStencil
        {
            get { return handler.clearStencil; }
            set { handler.clearStencil = value; serializableData.clearStencil = value; }
        }

        public ClearFlags ClearFlags
        {
            get { return handler.clearFlags; }
            set { handler.clearFlags = value; serializableData.clearFlags = value; }
        }

        public int Priority
        {
            get { return handler.priority; }
            set { handler.priority = value; serializableData.priority = value; }
        }

        public UInt64 Layers
        {
            get { return handler.layers; }
            set { handler.layers = value; serializableData.layers = value; }
        }

        public Matrix4 ProjMatrix
        {
            get { return handler.projMatrix; }
        }

        public Matrix4 ProjMatrixInverse
        {
            get { return handler.projMatrixInv; }
        }

        public Matrix4 ViewMatrix
        {
            get { return handler.viewMatrix; }
        }

        public Matrix4 ViewMatrixInverse
        {
            get { return handler.viewMatrixInv; }
        }

        public int WidthPixels
        {
            get { return handler.widthPixels; }
        }

        public int HeightPixels
        {
            get { return handler.heightPixels; }
        }

        public RenderTarget Target
        {
            get { return handler.target; }
            set { handler.target = value; }
        }

        public Vector2I WorldToScreen(Vector3 value) { return handler.WorldToScreen(value); }
        public Vector2 WorldToClip(Vector3 value) { return handler.WorldToClip(value); }
        public Vector3 WorldToView(Vector3 value) { return handler.WorldToView(value); }

        public Vector3 ScreenToWorld(Vector2I value) { return handler.ScreenToWorld(value); }
        public Vector3 ScreenToView(Vector2I value) { return handler.ScreenToView(value); }
        public Vector2 ScreenToClip(Vector2I value) { return handler.ScreenToClip(value); }

        public Vector3 ViewToWorld(Vector3 value) { return handler.ViewToWorld(value); }
        public Vector2I ViewToScreen(Vector3 value) { return handler.ViewToScreen(value); }
        public Vector2 ViewToClip(Vector3 value) { return handler.ViewToClip(value); }

        public Vector3 ClipToWorld(Vector2 value) { return handler.ClipToWorld(value); }
        public Vector3 ClipToView(Vector2 value) { return handler.ClipToView(value); }
        public Vector2I ClipToScreen(Vector2 value) { return handler.ClipToScreen(value); }

        public Ray ScreenToWorldRay(Vector2I value) { return handler.ScreenToWorldRay(value); }
        public Vector3 ProjectPoint(Vector3 value) { return handler.ProjectPoint(value); }
        public Vector3 UnprojectPoint(Vector3 value) { return handler.UnprojectPoint(value); }

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
            if (handler != null)
                handler.OnDestroy();

            handler = new CameraHandler(sceneObject);

            // Restore saved values after reset
            handler.aspectRatio = serializableData.aspectRatio;
            handler.nearClipPlane = serializableData.nearClipPlane;
            handler.farClipPlane = serializableData.farClipPlane;
            handler.fieldOfView = serializableData.fieldOfView;
            handler.viewportRect = serializableData.viewportRect;
            handler.projectionType = serializableData.projectionType;
            handler.orthoHeight = serializableData.orthoHeight;
            handler.clearColor = serializableData.clearColor;
            handler.clearDepth = serializableData.clearDepth;
            handler.clearStencil = serializableData.clearStencil;
            handler.clearFlags = serializableData.clearFlags;
            handler.priority = serializableData.priority;
            handler.layers = serializableData.layers;

            // TODO - Make RenderTexture a resource so I can save/restore it?
        }

        private void Update()
        {
            handler.UpdateView(sceneObject);
        }

        private void OnDestroy()
        {
            handler.OnDestroy();
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
