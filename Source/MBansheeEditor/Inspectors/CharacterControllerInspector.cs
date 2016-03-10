//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
using BansheeEngine;

namespace BansheeEditor
{
    /// <summary>
    /// Renders an inspector for the <see cref="CharacterController"/> component.
    /// </summary>
    [CustomInspector(typeof(CharacterController))]
    internal class CharacterControllerInspector : Inspector
    {
        private GUIFloatField radiusField = new GUIFloatField(new LocEdString("Radius"));
        private GUIFloatField heightField = new GUIFloatField(new LocEdString("Height"));
        private GUIVector3Field orientationField = new GUIVector3Field(new LocEdString("Orientation"));
        private GUIFloatField contactOffsetField = new GUIFloatField(new LocEdString("Contact offset"));
        private GUIFloatField minMoveDistanceField = new GUIFloatField(new LocEdString("Min. move distance"));

        private GUIEnumField climbingModeField = new GUIEnumField(typeof (CharacterClimbingMode),
            new LocEdString("Climbing mode"));
        private GUIFloatField stepOffsetField = new GUIFloatField(new LocEdString("Step offset"));
        
        private GUIEnumField nonWalkableModeField = new GUIEnumField(typeof (CharacterNonWalkableMode),
            new LocEdString("Non-walkable mode"));
        private GUISliderField slopeLimitField = new GUISliderField(0, 180, new LocEdString("Slope limit"));

        private GUIListBoxField layerField = new GUIListBoxField(Layers.Names, false, new LocEdString("Layer"));

        private ulong layersValue = 0;
        private InspectableState modifyState;

        private Vector3 orientation;

        /// <inheritdoc/>
        protected internal override void Initialize()
        {
            BuildGUI();
        }

        /// <inheritdoc/>
        protected internal override InspectableState Refresh()
        {
            CharacterController controller = InspectedObject as CharacterController;
            if (controller == null)
                return InspectableState.NotModified;

            radiusField.Value = controller.Radius;
            heightField.Value = controller.Height;
            orientationField.Value = orientation;
            contactOffsetField.Value = controller.ContactOffset;
            minMoveDistanceField.Value = controller.MinMoveDistance;
            climbingModeField.Value = (ulong)controller.ClimbingMode;
            stepOffsetField.Value = controller.StepOffset;
            nonWalkableModeField.Value = (ulong)controller.NonWalkableMode;
            slopeLimitField.Value = controller.SlopeLimit.Degrees;

            if (layersValue != controller.Layer)
            {
                bool[] states = new bool[64];
                for (int i = 0; i < states.Length; i++)
                    states[i] = (controller.Layer & Layers.Values[i]) == Layers.Values[i];

                layerField.States = states;
                layersValue = controller.Layer;
            }

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
            CharacterController controller = InspectedObject as CharacterController;
            if (controller == null)
                return;

            radiusField.OnChanged += x => { controller.Radius = x; MarkAsModified(); };
            radiusField.OnConfirmed += ConfirmModify;
            radiusField.OnFocusLost += ConfirmModify;

            heightField.OnChanged += x => { controller.Height = x; MarkAsModified(); };
            heightField.OnConfirmed += ConfirmModify;
            heightField.OnFocusLost += ConfirmModify;

            orientationField.OnChanged += x =>
            {
                orientation = x;
                Quaternion rotation = Quaternion.FromEuler(x);

                controller.Up = rotation.Rotate(Vector3.YAxis);
                MarkAsModified();
            };
            orientationField.OnConfirmed += ConfirmModify;
            orientationField.OnFocusLost += ConfirmModify;

            contactOffsetField.OnChanged += x => { controller.ContactOffset = x; MarkAsModified(); };
            contactOffsetField.OnConfirmed += ConfirmModify;
            contactOffsetField.OnFocusLost += ConfirmModify;

            minMoveDistanceField.OnChanged += x => { controller.MinMoveDistance = x; MarkAsModified(); };
            minMoveDistanceField.OnConfirmed += ConfirmModify;
            minMoveDistanceField.OnFocusLost += ConfirmModify;

            climbingModeField.OnSelectionChanged += x =>
            {
                controller.ClimbingMode = (CharacterClimbingMode)x;

                MarkAsModified();
                ConfirmModify();
            };

            stepOffsetField.OnChanged += x => { controller.StepOffset = x; MarkAsModified(); };
            stepOffsetField.OnConfirmed += ConfirmModify;
            stepOffsetField.OnFocusLost += ConfirmModify;

            nonWalkableModeField.OnSelectionChanged += x =>
            {
                controller.NonWalkableMode = (CharacterNonWalkableMode)x;

                MarkAsModified();
                ConfirmModify();
            };

            slopeLimitField.OnChanged += x => { controller.SlopeLimit = new Degree(x); MarkAsModified(); };
            slopeLimitField.OnFocusLost += ConfirmModify;

            layerField.OnSelectionChanged += x =>
            {
                ulong layer = 0;
                bool[] states = layerField.States;
                for (int i = 0; i < states.Length; i++)
                    layer |= states[i] ? Layers.Values[i] : 0;

                layersValue = layer;
                controller.Layer = layer;

                MarkAsModified();
                ConfirmModify();
            };

            Layout.AddElement(radiusField);
            Layout.AddElement(heightField);
            Layout.AddElement(orientationField);
            Layout.AddElement(contactOffsetField);
            Layout.AddElement(minMoveDistanceField);
            Layout.AddElement(climbingModeField);
            Layout.AddElement(stepOffsetField);
            Layout.AddElement(nonWalkableModeField);
            Layout.AddElement(slopeLimitField);
            Layout.AddElement(layerField);

            orientation = Quaternion.FromToRotation(Vector3.YAxis, controller.Up).ToEuler();
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