using System;
using System.Collections.Generic;
using System.Runtime.CompilerServices;
using BansheeEngine;

namespace BansheeEditor
{
    public class ProjectDropTarget
    {
        private const int DragStartDistancePx = 3;

        public Action<Vector2I, string[]> OnDropResource;
        public Action<Vector2I, SceneObject[]> OnDropSceneObject;
        public Action<Vector2I> OnStart;
        public Action<Vector2I> OnEnter;
        public Action OnLeave;
        public Action<Vector2I> OnDrag;
        public Action<Vector2I> OnEnd;

        private readonly EditorWindow parentWindow;

        private OSDropTarget dropTargetOS;
        private Rect2I bounds;

        private bool isMouseDown;
        private bool isDragInProgress;
        private bool triggerStartDrag;
        private bool triggerEndDrag;
        private bool isDragInBounds;
        private bool isOSDragActive;
        private Vector2I mouseDownScreenPos;
        private Vector2I lastDragWindowPos;

        public Rect2I Bounds
        {
            get { return bounds; }
            set
            {
                bounds = value;
                dropTargetOS.Bounds = bounds;
            }
        }

        public ProjectDropTarget(EditorWindow window)
        {
            parentWindow = window;

            dropTargetOS = new OSDropTarget(window);
            dropTargetOS.OnDrag += DoOnOSDrag;
            dropTargetOS.OnDrop += DoOnOSDrop;
            dropTargetOS.OnEnter += DoOnOSDragEnter;
            dropTargetOS.OnLeave += DoOnOSDragLeave;

            Input.OnPointerPressed += Input_OnPointerPressed;
            Input.OnPointerReleased += Input_OnPointerReleased;
            Input.OnPointerMoved += Input_OnPointerMoved;
        }

        void Input_OnPointerMoved(PointerEvent ev)
        {
            Vector2I currentWindowPos = parentWindow.ScreenToWindowPos(ev.screenPos);

            if (isMouseDown && !isDragInProgress)
            {
                Vector2I startWindowPos = parentWindow.ScreenToWindowPos(mouseDownScreenPos);
                if (!Bounds.Contains(startWindowPos))
                    return;

                int distance = Vector2I.Distance(startWindowPos, currentWindowPos);
                if (distance >= DragStartDistancePx)
                    triggerStartDrag = true;
            }
        }

        void Input_OnPointerReleased(PointerEvent ev)
        {
            isDragInProgress = false;
            isMouseDown = false;
            isDragInBounds = false;
            triggerEndDrag = true;
        }

        void Input_OnPointerPressed(PointerEvent ev)
        {
            isMouseDown = true;
            mouseDownScreenPos = ev.screenPos;
        }

        public void Update()
        {
            Vector2I currentWindowPos = parentWindow.ScreenToWindowPos(Input.PointerPosition);

            if (triggerStartDrag)
            {
                isDragInProgress = true;
                triggerStartDrag = false;

                if (OnStart != null)
                    OnStart(currentWindowPos);
            }

            if (triggerEndDrag)
            {
                triggerEndDrag = false;

                if (OnEnd != null)
                    OnEnd(currentWindowPos);
            }

            if (isOSDragActive)
                return;

            if (isDragInProgress)
            {
                if (lastDragWindowPos != currentWindowPos)
                {
                    if (!isDragInBounds)
                    {
                        if (Bounds.Contains(currentWindowPos))
                        {
                            isDragInBounds = true;
                            if (OnEnter != null)
                                OnEnter(currentWindowPos);
                        }
                    }

                    if (OnDrag != null)
                        OnDrag(currentWindowPos);

                    if (isDragInBounds)
                    {
                        if (!Bounds.Contains(currentWindowPos))
                        {
                            isDragInBounds = false;
                            if (OnLeave != null)
                                OnLeave();
                        }
                    }

                    lastDragWindowPos = currentWindowPos;
                }
            }

            if (DragDrop.DropInProgress)
            {
                if (DragDrop.Type == DragDropType.Resource)
                {
                    if (OnDropResource != null)
                    {
                        ResourceDragDropData resourceDragDrop = (ResourceDragDropData) DragDrop.Data;
                        OnDropResource(currentWindowPos, resourceDragDrop.Paths);
                    }
                }
                else if (DragDrop.Type == DragDropType.SceneObject)
                {
                    if (OnDropSceneObject != null)
                    {
                        SceneObjectDragDropData sceneDragDrop = (SceneObjectDragDropData) DragDrop.Data;
                        OnDropSceneObject(currentWindowPos, sceneDragDrop.Objects);
                    }
                }
            }
        }

        public void Destroy()
        {
            dropTargetOS.Destroy();
            dropTargetOS = null;
        }

        private void DoOnOSDragEnter(int x, int y)
        {
            isOSDragActive = true;

            if (OnEnter != null)
                OnEnter(new Vector2I(x, y));
        }

        private void DoOnOSDragLeave()
        {
            isOSDragActive = false;

            if (OnLeave != null)
                OnLeave();
        }

        private void DoOnOSDrag(int x, int y)
        {
            if (OnDrag != null)
                OnDrag(new Vector2I(x, y));
        }

        private void DoOnOSDrop(int x, int y)
        {
            isOSDragActive = false;

            if (OnDropResource != null)
                OnDropResource(new Vector2I(x, y), dropTargetOS.FilePaths);
        }
    }
}
