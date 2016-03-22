//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
using System;
using System.Collections.Generic;
using System.Runtime.CompilerServices;
using BansheeEngine;

namespace BansheeEditor
{
    /// <summary>
    /// Handles various types of drag and drop operations used by the library window.
    /// </summary>
    public class LibraryDropTarget
    {
        private const int DragStartDistancePx = 3;

        /// <summary>
        /// Triggered when a resource drag and drop operation occured over the drop target.
        /// </summary>
        public Action<Vector2I, string[]> OnDropResource;

        /// <summary>
        /// Triggered when a scene object drag and drop operation occured over the drop target.
        /// </summary>
        public Action<Vector2I, SceneObject[]> OnDropSceneObject;

        /// <summary>
        /// Triggered when a local drag and drop operation is starting (pointer has been pressed and is being dragged while
        /// within the drop target).
        /// </summary>
        public Action<Vector2I> OnStart;

        /// <summary>
        /// Triggered when a pointer enters the drop target area while a drag operation is in progress.
        /// </summary>
        public Action<Vector2I> OnEnter;

        /// <summary>
        /// Triggered when a pointer leaves the drop target area while a drag operation is in progress.
        /// </summary>
        public Action OnLeave;

        /// <summary>
        /// Triggered when a pointer moves within the drop target area while a drag operation is in progress.
        /// </summary>
        public Action<Vector2I> OnDrag;

        /// <summary>
        /// Triggered when a locally initiated (started within the drop target) drag and drop operation ends.
        /// </summary>
        public Action<Vector2I> OnEnd;

        private readonly EditorWindow parentWindow;

        private OSDropTarget dropTargetOS;
        private Rect2I bounds;

        private bool isMouseDown;
        private bool isLocalDragInProgress;
        private bool triggerStartLocalDrag;
        private bool triggerEndLocalDrag;
        private bool isDragInBounds;
        private bool isOSDragActive;
        private Vector2I mouseDownScreenPos;
        private Vector2I lastDragWindowPos;

        /// <summary>
        /// Area inside which drag and drop operations are allowed to occurr. Relative to the parent library window.
        /// </summary>
        public Rect2I Bounds
        {
            get { return bounds; }
            set
            {
                bounds = value;
                dropTargetOS.Bounds = bounds;
            }
        }

        /// <summary>
        /// Creates a new library drop target.
        /// </summary>
        /// <param name="window">Window the drop target is part of.</param>
        public LibraryDropTarget(EditorWindow window)
        {
            parentWindow = window;

            dropTargetOS = new OSDropTarget(window);
            dropTargetOS.OnDrag += DoOnOSDrag;
            dropTargetOS.OnDrop += DoOnOSDrop;
            dropTargetOS.OnEnter += DoOnOSDragEnter;
            dropTargetOS.OnLeave += DoOnOSDragLeave;

            EditorInput.OnPointerPressed += Input_OnPointerPressed;
            EditorInput.OnPointerReleased += Input_OnPointerReleased;
            EditorInput.OnPointerMoved += Input_OnPointerMoved;
        }

        /// <summary>
        /// Triggered when the pointer moved.
        /// </summary>
        /// <param name="ev">Data about the pointer move event.</param>
        void Input_OnPointerMoved(PointerEvent ev)
        {
            Vector2I currentWindowPos = parentWindow.ScreenToWindowPos(ev.ScreenPos);

            if (isMouseDown && !isLocalDragInProgress)
            {
                Vector2I startWindowPos = parentWindow.ScreenToWindowPos(mouseDownScreenPos);
                if (!Bounds.Contains(startWindowPos))
                    return;

                int distance = Vector2I.Distance(startWindowPos, currentWindowPos);
                if (distance >= DragStartDistancePx)
                    triggerStartLocalDrag = true;
            }
        }

        /// <summary>
        /// Triggered when the pointer is released.
        /// </summary>
        /// <param name="ev">Data about the pointer release event.</param>
        void Input_OnPointerReleased(PointerEvent ev)
        {
            if (isLocalDragInProgress)
                triggerEndLocalDrag = true;

            isLocalDragInProgress = false;
            isMouseDown = false;
            isDragInBounds = false;
            triggerStartLocalDrag = false;
        }

        /// <summary>
        /// Triggered when the pointer is pressed.
        /// </summary>
        /// <param name="ev">Data about the pointer press event.</param>
        void Input_OnPointerPressed(PointerEvent ev)
        {
            Vector2I currentWindowPos = parentWindow.ScreenToWindowPos(ev.ScreenPos);

            if (Bounds.Contains(currentWindowPos))
            {
                isMouseDown = true;
                mouseDownScreenPos = ev.ScreenPos;
            }
        }

        /// <summary>
        /// Triggers events and queries for changes in drag and drop operations. Should be called every frame.
        /// </summary>
        public void Update()
        {
            Vector2I currentWindowPos = parentWindow.ScreenToWindowPos(Input.PointerScreenPosition);

            if (triggerStartLocalDrag)
            {
                isLocalDragInProgress = true;
                triggerStartLocalDrag = false;

                if (OnStart != null)
                    OnStart(currentWindowPos);
            }

            if (triggerEndLocalDrag)
            {
                triggerEndLocalDrag = false;

                if (OnEnd != null)
                    OnEnd(currentWindowPos);
            }

            if (isOSDragActive)
                return;

            bool externalDragInProgress = DragDrop.DragInProgress && DragDrop.Type == DragDropType.SceneObject;
            if (isLocalDragInProgress || externalDragInProgress)
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

            if (DragDrop.DropInProgress && Bounds.Contains(currentWindowPos))
            {
                if (DragDrop.Type == DragDropType.Resource)
                {
                    if (OnDropResource != null)
                    {
                        ResourceDragDropData resourceDragDrop = (ResourceDragDropData)DragDrop.Data;
                        OnDropResource(currentWindowPos, resourceDragDrop.Paths);
                    }
                }
                else if (DragDrop.Type == DragDropType.SceneObject)
                {
                    if (OnDropSceneObject != null)
                    {
                        SceneObjectDragDropData sceneDragDrop = (SceneObjectDragDropData)DragDrop.Data;
                        OnDropSceneObject(currentWindowPos, sceneDragDrop.Objects);
                    }
                }

                isDragInBounds = false;
            }
        }

        /// <summary>
        /// Destroy the drop target. You should call this when done with the drop target.
        /// </summary>
        public void Destroy()
        {
            dropTargetOS.Destroy();
            dropTargetOS = null;
        }

        /// <summary>
        /// Triggered when an OS drag event has entered the drop target area.
        /// </summary>
        /// <param name="x">X coordinate of the pointer relative to the parent window.</param>
        /// <param name="y">Y coordinate of the pointer relative to the parent window.</param>
        private void DoOnOSDragEnter(int x, int y)
        {
            isOSDragActive = true;

            if (OnEnter != null)
                OnEnter(new Vector2I(x, y));
        }

        /// <summary>
        /// Triggered when an OS drag event has left the parent window.
        /// </summary>
        private void DoOnOSDragLeave()
        {
            isOSDragActive = false;

            if (OnLeave != null)
                OnLeave();
        }

        /// <summary>
        /// Triggered when a pointer moves while an OS drag event is occuring over the drop target area.
        /// </summary>
        /// <param name="x">X coordinate of the pointer relative to the parent window.</param>
        /// <param name="y">Y coordinate of the pointer relative to the parent window.</param>
        private void DoOnOSDrag(int x, int y)
        {
            if (OnDrag != null)
                OnDrag(new Vector2I(x, y));
        }

        /// <summary>
        /// Triggered when the pointer was released during an OS drag and drop operation while over the drop target area.
        /// </summary>
        /// <param name="x">X coordinate of the pointer relative to the parent window.</param>
        /// <param name="y">Y coordinate of the pointer relative to the parent window.</param>
        private void DoOnOSDrop(int x, int y)
        {
            isOSDragActive = false;

            if (OnDropResource != null)
                OnDropResource(new Vector2I(x, y), dropTargetOS.FilePaths);
        }
    }
}
