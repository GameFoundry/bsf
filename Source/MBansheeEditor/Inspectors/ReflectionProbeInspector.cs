//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
using System.Collections.Generic;
using BansheeEngine;

namespace BansheeEditor
{
    /** @addtogroup Inspectors
     *  @{
     */

    /// <summary>
    /// Renders an inspector for the <see cref="ReflectionProbe"/> component.
    /// </summary>
    [CustomInspector(typeof(ReflectionProbe))]
    internal class ReflectionProbeInspector : Inspector
    {
        private GUIEnumField probeTypeField = new GUIEnumField(typeof(ReflectionProbeType), new LocEdString("Probe type"));
        private GUIVector3Field extentsField = new GUIVector3Field(new LocEdString("Extents"));
        private GUIFloatField radiusField = new GUIFloatField(new LocEdString("Radius"));
        private GUITextureField customTextureField = new GUITextureField(new LocEdString("Custom texture"));
        private GUIButton captureButton = new GUIButton(new LocEdString("Capture"));

        private InspectableState modifyState;

        /// <inheritdoc/>
        protected internal override void Initialize()
        {
            if (InspectedObject != null)
            {
                ReflectionProbe probe = (ReflectionProbe)InspectedObject;

                probeTypeField.OnSelectionChanged += x =>
                {
                    probe.Type = (ReflectionProbeType)x;

                    ToggleTypeSpecificFields((ReflectionProbeType) x);
                };

                radiusField.OnChanged += x => { probe.Radius = x; MarkAsModified(); };
                radiusField.OnConfirmed += ConfirmModify;
                radiusField.OnFocusLost += ConfirmModify;

                extentsField.OnChanged += x => { probe.Extents = x; MarkAsModified(); };
                extentsField.OnConfirmed += ConfirmModify;
                extentsField.OnFocusLost += ConfirmModify;

                customTextureField.OnChanged += x =>
                {
                    probe.CustomTexture = Resources.Load<Texture>(x);
                    MarkAsModified();
                    ConfirmModify();
                };

                captureButton.OnClick += () => probe.Capture();

                Layout.AddElement(probeTypeField);
                Layout.AddElement(radiusField);
                Layout.AddElement(extentsField);
                Layout.AddElement(customTextureField);
                Layout.AddSpace(10);
                Layout.AddElement(captureButton);

                ToggleTypeSpecificFields(probe.Type);
            }
        }

        /// <inheritdoc/>
        protected internal override InspectableState Refresh()
        {
            ReflectionProbe probe = InspectedObject as ReflectionProbe;
            if (probe == null)
                return InspectableState.NotModified;

            ReflectionProbeType probeType = probe.Type;
            if (probeTypeField.Value != (ulong)probeType)
                ToggleTypeSpecificFields(probeType);

            probeTypeField.Value = (ulong)probeType;
            radiusField.Value = probe.Radius;
            extentsField.Value = probe.Extents;
            customTextureField.Value = probe.CustomTexture;

            InspectableState oldState = modifyState;
            if (modifyState.HasFlag(InspectableState.Modified))
                modifyState = InspectableState.NotModified;

            return oldState;
        }

        /// <summary>
        /// Enables or disables different GUI elements depending on the probe type.
        /// </summary>
        /// <param name="type">Probe type to show GUI elements for.</param>
        private void ToggleTypeSpecificFields(ReflectionProbeType type)
        {
            bool isBox = type == ReflectionProbeType.Box;

            radiusField.Active = !isBox;
            extentsField.Active = isBox;
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

    /** @} */
}
