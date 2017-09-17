//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
using BansheeEngine;

namespace BansheeEditor
{
    /** @addtogroup Inspectors
     *  @{
     */

    /// <summary>
    /// Renders an inspector for the <see cref="DistanceJoint"/> component.
    /// </summary>
    [CustomInspector(typeof(DistanceJoint))]
    internal class DistanceJointInspector : JointInspector
    {
        private GUIToggleField enableMinLimitField = new GUIToggleField(new LocEdString("Enable minimum limit"));
        private GUIFloatField minLimitField = new GUIFloatField(new LocEdString("Minimum distance"));
        private GUIToggleField enableMaxLimitField = new GUIToggleField(new LocEdString("Enable maximum limit"));
        private GUIFloatField maxLimitField = new GUIFloatField(new LocEdString("Maximum distance"));
        private GUIFloatField toleranceField = new GUIFloatField(new LocEdString("Tolerance"));
        private GUIToggleField enableSpringField = new GUIToggleField(new LocEdString("Enable spring"));
        private SpringGUI springGUI;

        private GUILayoutX springLayout;

        /// <inheritdoc/>
        protected internal override void Initialize()
        {
            DistanceJoint joint = InspectedObject as DistanceJoint;

            if (joint != null)
                BuildGUI(joint);
        }

        /// <inheritdoc/>
        protected internal override InspectableState Refresh()
        {
            DistanceJoint joint = InspectedObject as DistanceJoint;
            if (joint == null)
                return InspectableState.NotModified;

            Refresh(joint);

            InspectableState oldState = modifyState;
            if (modifyState.HasFlag(InspectableState.Modified))
                modifyState = InspectableState.NotModified;

            return oldState;
        }

        /// <summary>
        /// Creates GUI elements for fields specific to the distance joint.
        /// </summary>
        protected void BuildGUI(DistanceJoint joint)
        {
            enableMinLimitField.OnChanged += x =>
            {
                joint.SetFlag(DistanceJointFlag.MinDistance, x);
                MarkAsModified();
                ConfirmModify();

                minLimitField.Active = x;
            };

            minLimitField.OnChanged += x => { joint.MinDistance = x; MarkAsModified(); };
            minLimitField.OnFocusLost += ConfirmModify;
            minLimitField.OnConfirmed += ConfirmModify;

            enableMaxLimitField.OnChanged += x =>
            {
                joint.SetFlag(DistanceJointFlag.MaxDistance, x);
                MarkAsModified();
                ConfirmModify();

                maxLimitField.Active = x;
            };

            maxLimitField.OnChanged += x => { joint.MaxDistance = x; MarkAsModified(); };
            maxLimitField.OnFocusLost += ConfirmModify;
            maxLimitField.OnConfirmed += ConfirmModify;

            toleranceField.OnChanged += x => { joint.Tolerance = x; MarkAsModified(); };
            toleranceField.OnFocusLost += ConfirmModify;
            toleranceField.OnConfirmed += ConfirmModify;

            enableSpringField.OnChanged += x =>
            {
                joint.SetFlag(DistanceJointFlag.Spring, x);
                MarkAsModified();
                ConfirmModify();

                springLayout.Active = x;
            };

            Layout.AddElement(enableMinLimitField);
            GUILayoutX minLimitLayout = Layout.AddLayoutX();
            {
                minLimitLayout.AddSpace(10);
                minLimitLayout.AddElement(minLimitField);
            }

            Layout.AddElement(enableMaxLimitField);
            GUILayoutX maxLimitLayout = Layout.AddLayoutX();
            {
                maxLimitLayout.AddSpace(10);
                maxLimitLayout.AddElement(maxLimitField);
            }

            Layout.AddElement(toleranceField);
            Layout.AddElement(enableSpringField);
            springLayout = Layout.AddLayoutX();
            {
                springLayout.AddSpace(10);
                springGUI = new SpringGUI(joint.Spring, springLayout);
                springGUI.OnChanged += x => { joint.Spring = x; MarkAsModified(); };
                springGUI.OnConfirmed += ConfirmModify;
            }

            minLimitField.Active = joint.HasFlag(DistanceJointFlag.MinDistance);
            maxLimitField.Active = joint.HasFlag(DistanceJointFlag.MaxDistance);
            springLayout.Active = joint.HasFlag(DistanceJointFlag.Spring);

            base.BuildGUI(joint, true);
        }

        /// <summary>
        /// Updates all GUI elements from current values in the joint.
        /// </summary>
        /// <param name="joint">Joint to update the GUI from.</param>
        protected void Refresh(DistanceJoint joint)
        {
            bool enableMinDistanceLimit = joint.HasFlag(DistanceJointFlag.MinDistance);
            if (enableMinLimitField.Value != enableMinDistanceLimit)
            {
                enableMinLimitField.Value = enableMinDistanceLimit;
                minLimitField.Active = enableMinDistanceLimit;
            }

            minLimitField.Value = joint.MinDistance;

            bool enableMaxDistanceLimit = joint.HasFlag(DistanceJointFlag.MaxDistance);
            if (enableMaxLimitField.Value != enableMaxDistanceLimit)
            {
                enableMaxLimitField.Value = enableMaxDistanceLimit;
                maxLimitField.Active = enableMaxDistanceLimit;
            }

            maxLimitField.Value = joint.MaxDistance;
            toleranceField.Value = joint.Tolerance;

            bool enableSpring = joint.HasFlag(DistanceJointFlag.Spring);
            if (enableSpringField.Value != enableSpring)
            {
                enableSpringField.Value = enableSpring;
                springLayout.Active = enableSpring;
            }

            springGUI.Spring = joint.Spring;

            base.Refresh(joint);
        }
    }

    /** @} */
}