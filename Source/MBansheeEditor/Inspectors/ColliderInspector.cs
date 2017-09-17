//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
using BansheeEngine;

namespace BansheeEditor
{
    /** @addtogroup Inspectors
     *  @{
     */

    /// <summary>
    /// Renders common inspector elements for all <see cref="Collider"/> components.
    /// </summary>
    internal abstract class ColliderInspector : Inspector
    {
        private GUIToggleField isTriggerField = new GUIToggleField(new LocEdString("Is trigger"));
        private GUIFloatField massField = new GUIFloatField(new LocEdString("Mass"));
        private GUIResourceField materialField = new GUIResourceField(typeof (PhysicsMaterial),
            new LocEdString("Material"));
        private GUIFloatField contactOffsetField = new GUIFloatField(new LocEdString("Contact offset"));
        private GUIFloatField restOffsetField = new GUIFloatField(new LocEdString("Rest offset"));

        private GUIEnumField collisionReportModeField = new GUIEnumField(typeof (CollisionReportMode),
            new LocEdString("Collision report mode"));
        private GUIListBoxField layerField = new GUIListBoxField(Layers.Names, false, new LocEdString("Layer"));

        private ulong layersValue = 0;
        protected InspectableState modifyState;

        /// <summary>
        /// Updates all GUI elements from current values in the collider.
        /// </summary>
        /// <param name="collider">Collider to update the GUI from.</param>
        protected virtual void Refresh(Collider collider)
        {
            isTriggerField.Value = collider.Trigger;
            massField.Value = collider.Mass;
            materialField.Value = collider.Material;
            contactOffsetField.Value = collider.ContactOffset;
            restOffsetField.Value = collider.RestOffset;
            collisionReportModeField.Value = (ulong)collider.CollisionReportMode;

            if (layersValue != collider.Layer)
            {
                bool[] states = new bool[64];
                for (int i = 0; i < states.Length; i++)
                    states[i] = (collider.Layer & Layers.Values[i]) == Layers.Values[i];

                layerField.States = states;
                layersValue = collider.Layer;
            }
        }

        /// <summary>
        /// Creates GUI elements for fields common to all colliders.
        /// </summary>
        protected virtual void BuildGUI(Collider collider)
        {
            isTriggerField.OnChanged += x => { collider.Trigger = x; MarkAsModified(); ConfirmModify(); };

            massField.OnChanged += x => { collider.Mass = x; MarkAsModified(); };
            massField.OnConfirmed += ConfirmModify;
            massField.OnFocusLost += ConfirmModify;

            materialField.OnChanged += x =>
            {
                PhysicsMaterial mesh = Resources.Load<PhysicsMaterial>(x);
                collider.Material = mesh;

                MarkAsModified();
                ConfirmModify();
            };

            contactOffsetField.OnChanged += x => { collider.ContactOffset = x; MarkAsModified(); };
            contactOffsetField.OnConfirmed += ConfirmModify;
            contactOffsetField.OnFocusLost += ConfirmModify;

            restOffsetField.OnChanged += x => { collider.RestOffset = x; MarkAsModified(); };
            restOffsetField.OnConfirmed += ConfirmModify;
            restOffsetField.OnFocusLost += ConfirmModify;

            collisionReportModeField.OnSelectionChanged += x =>
            {
                collider.CollisionReportMode = (CollisionReportMode)x;

                MarkAsModified();
                ConfirmModify();
            };

            layerField.OnSelectionChanged += x =>
            {
                ulong layer = 0;
                bool[] states = layerField.States;
                for (int i = 0; i < states.Length; i++)
                    layer |= states[i] ? Layers.Values[i] : 0;

                layersValue = layer;
                collider.Layer = layer;

                MarkAsModified();
                ConfirmModify();
            };

            Layout.AddElement(isTriggerField);
            Layout.AddElement(massField);
            Layout.AddElement(materialField);
            Layout.AddElement(contactOffsetField);
            Layout.AddElement(restOffsetField);
            Layout.AddElement(collisionReportModeField);
            Layout.AddElement(layerField);
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
