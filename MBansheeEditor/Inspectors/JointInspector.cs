//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
using BansheeEngine;

namespace BansheeEditor
{
    /// <summary>
    /// Renders common inspector elements for all <see cref="Joint"/> components.
    /// </summary>
    public abstract class JointInspector : Inspector
    {
        private GUIGameObjectField bodyAField = new GUIGameObjectField(typeof(Rigidbody), new LocEdString("Body A"));
        private GUIVector3Field bodyAOffsetField = new GUIVector3Field(new LocEdString("Body A offset"));

        private GUIGameObjectField bodyBField = new GUIGameObjectField(typeof(Rigidbody), new LocEdString("Body B"));
        private GUIVector3Field bodyBOffsetField = new GUIVector3Field(new LocEdString("Body B offset"));

        private GUIFloatField breakForceField = new GUIFloatField(new LocEdString("Break force"));
        private GUIFloatField breakTorqueField = new GUIFloatField(new LocEdString("Break torque"));
        private GUIToggleField collisionField = new GUIToggleField(new LocEdString("Enable collision"));
        
        protected InspectableState modifyState;

        /// <summary>
        /// Updates all GUI elements from current values in the joint.
        /// </summary>
        /// <param name="joint">Joint to update the GUI from.</param>
        protected virtual void Refresh(Joint joint)
        {
            bodyAField.Value = joint.GetRigidbody(JointBody.A);
            bodyAOffsetField.Value = joint.GetPosition(JointBody.A);

            bodyBField.Value = joint.GetRigidbody(JointBody.B);
            bodyBOffsetField.Value = joint.GetPosition(JointBody.B);

            breakForceField.Value = joint.BreakForce;
            breakTorqueField.Value = joint.BreakTorque;
            collisionField.Value = joint.EnableCollision;
        }

        /// <summary>
        /// Creates GUI elements for fields common to all joints.
        /// </summary>
        protected virtual void BuildGUI(Joint joint)
        {
            bodyAField.OnChanged += x => { joint.SetRigidbody(JointBody.A, (Rigidbody)x); MarkAsModified(); ConfirmModify(); };
            bodyAOffsetField.OnChanged += x => { joint.SetPosition(JointBody.A, x); MarkAsModified(); };
            bodyAOffsetField.OnFocusLost += ConfirmModify;
            bodyAOffsetField.OnConfirmed += ConfirmModify;

            bodyBField.OnChanged += x => { joint.SetRigidbody(JointBody.B, (Rigidbody)x); MarkAsModified(); ConfirmModify(); };
            bodyBOffsetField.OnChanged += x => { joint.SetPosition(JointBody.B, x); MarkAsModified(); };
            bodyBOffsetField.OnFocusLost += ConfirmModify;
            bodyBOffsetField.OnConfirmed += ConfirmModify;

            breakForceField.OnChanged += x => { joint.BreakForce = x; MarkAsModified(); };
            breakForceField.OnFocusLost += ConfirmModify;
            breakForceField.OnConfirmed += ConfirmModify;

            breakTorqueField.OnChanged += x => { joint.BreakTorque = x; MarkAsModified(); };
            breakTorqueField.OnFocusLost += ConfirmModify;
            breakTorqueField.OnConfirmed += ConfirmModify;

            collisionField.OnChanged += x => { joint.EnableCollision = x; MarkAsModified(); ConfirmModify(); };
            
            Layout.AddElement(bodyAField);
            Layout.AddElement(bodyAOffsetField);
            Layout.AddElement(bodyBField);
            Layout.AddElement(bodyBOffsetField);
            Layout.AddElement(breakForceField);
            Layout.AddElement(breakTorqueField);
            Layout.AddElement(collisionField);
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
