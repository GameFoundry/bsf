//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
using BansheeEngine;

namespace BansheeEditor
{
    /// <summary>
    /// Renders an inspector for the <see cref="GUIWidget"/> component.
    /// </summary>
    [CustomInspector(typeof(GUIWidget))]
    internal class GUIWidgetInspector : Inspector
    {
        private GUIResourceField skinField;
        private GUIGameObjectField cameraField;

        private InspectableState modifyState;

        /// <inheritdoc/>
        protected internal override void Initialize()
        {
            BuildGUI();
        }

        /// <inheritdoc/>
        protected internal override InspectableState Refresh()
        {
            GUIWidget guiWidget = InspectedObject as GUIWidget;
            if (guiWidget == null)
                return InspectableState.NotModified;

            skinField.Value = guiWidget.Skin;
            cameraField.Value = guiWidget.Camera;
            
            InspectableState oldState = modifyState;
            if (modifyState.HasFlag(InspectableState.Modified))
                modifyState = InspectableState.NotModified;

            return oldState;
        }

        /// <summary>
        /// Recreates all the GUI elements used by this inspector.
        /// </summary>
        private void BuildGUI()
        {
            Layout.Clear();

            GUIWidget guiWidget = InspectedObject as GUIWidget;
            if (guiWidget == null)
                return;

            skinField = new GUIResourceField(typeof(GUISkin), new LocEdString("Skin"));
            cameraField = new GUIGameObjectField(typeof (Camera), new LocEdString("Camera"));

            skinField.OnChanged += x =>
            {
                GUISkin skin = Resources.Load<GUISkin>(x);
                guiWidget.Skin = skin;

                MarkAsModified();
                ConfirmModify();
            };

            cameraField.OnChanged += x =>
            {
                guiWidget.Camera = x as Camera;

                MarkAsModified();
                ConfirmModify();
            };

            Layout.AddElement(skinField);
            Layout.AddElement(cameraField);
        }

        /// <summary>
        /// Marks the contents of the inspector as modified.
        /// </summary>
        protected void MarkAsModified()
        {
            modifyState |= InspectableState.ModifyInProgress;
        }

        /// <summary>
        /// Confirms any queued modifications.
        /// </summary>
        protected void ConfirmModify()
        {
            if (modifyState.HasFlag(InspectableState.ModifyInProgress))
                modifyState |= InspectableState.Modified;
        }
    }
}