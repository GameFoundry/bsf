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

        internal CameraHandler Handler
        {
            get { return handler; }
        }

        public float aspectRatio
        {
            get { return handler.aspectRatio; }
            set { handler.aspectRatio = value; }
        }

        public float nearClipPlane
        {
            get { return handler.nearClipPlane; }
            set { handler.nearClipPlane = value; }
        }

        public float farClipPlane
        {
            get { return handler.farClipPlane; }
            set { handler.farClipPlane = value; }
        }

        public Degree fieldOfView
        {
            get { return handler.fieldOfView; }
            set { handler.fieldOfView = value; }
        }

        public Rect2 viewportRect
        {
            get { return handler.viewportRect; }
            set { handler.viewportRect = value; }
        }

        public ProjectionType projectionType
        {
            get { return handler.projectionType; }
            set { handler.projectionType = value; }
        }

        public float orthoHeight
        {
            get { return handler.orthoHeight; }
            set { handler.orthoHeight = value; }
        }

        public float orthoWidth
        {
            get { return handler.orthoWidth; }
        }

        public Color clearColor
        {
            get { return handler.clearColor; }
            set { handler.clearColor = value; }
        }

        public float clearDepth
        {
            get { return handler.clearDepth; }
            set { handler.clearDepth = value; }
        }

        public UInt16 clearStencil
        {
            get { return handler.clearStencil; }
            set { handler.clearStencil = value; }
        }

        public ClearFlags clearFlags
        {
            get { return handler.clearFlags; }
            set { handler.clearFlags = value; }
        }

        public int priority
        {
            get { return handler.priority; }
            set { handler.priority = value; }
        }

        public UInt64 layers
        {
            get { return handler.layers; }
            set { handler.layers = value; }
        }

        public Matrix4 projMatrix
        {
            get { return handler.projMatrix; }
        }

        public Matrix4 projMatrixInv
        {
            get { return handler.projMatrixInv; }
        }

        public Matrix4 viewMatrix
        {
            get { return handler.viewMatrix; }
        }

        public Matrix4 viewMatrixInv
        {
            get { return handler.viewMatrixInv; }
        }

        public int widthPixels
        {
            get { return handler.widthPixels; }
        }

        public int heightPixels
        {
            get { return handler.heightPixels; }
        }

        public RenderTarget target
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

        public Camera()
        {
            handler = new CameraHandler();
        }

        private void Update()
        {
            handler.UpdateView(sceneObject);
        }
    }
}
