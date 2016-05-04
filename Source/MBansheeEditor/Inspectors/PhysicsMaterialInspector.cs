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
    /// Renders an inspector for the <see cref="PhysicsMaterial"/> resource.
    /// </summary>
    [CustomInspector(typeof(PhysicsMaterial))]
    internal class PhysicsMaterialInspector : Inspector
    {
        private GUIFloatField staticFrictionField;
        private GUIFloatField dynamicFrictionField;
        private GUISliderField restitutionField;

        /// <inheritdoc/>
        protected internal override void Initialize()
        {
            BuildGUI();
        }

        /// <inheritdoc/>
        protected internal override InspectableState Refresh()
        {
            PhysicsMaterial material = InspectedObject as PhysicsMaterial;
            if (material == null)
                return InspectableState.NotModified;

            staticFrictionField.Value = material.StaticFriction;
            dynamicFrictionField.Value = material.DynamicFriction;
            restitutionField.Value = material.Restitution;

            return InspectableState.NotModified;
        }

        /// <summary>
        /// Recreates all the GUI elements used by this inspector.
        /// </summary>
        private void BuildGUI()
        {
            Layout.Clear();

            PhysicsMaterial material = InspectedObject as PhysicsMaterial;
            if (material == null)
                return;

            staticFrictionField = new GUIFloatField(new LocEdString("Static friction"));
            dynamicFrictionField = new GUIFloatField(new LocEdString("Dynamic friction"));
            restitutionField = new GUISliderField(0.0f, 1.0f, new LocEdString("Restitution"));

            staticFrictionField.OnChanged += x =>
            {
                material.StaticFriction = x;
                EditorApplication.SetDirty(material);
            };

            dynamicFrictionField.OnChanged += x =>
            {
                material.DynamicFriction = x;
                EditorApplication.SetDirty(material);
            };

            restitutionField.OnChanged += x =>
            {
                material.Restitution = x;
                EditorApplication.SetDirty(material);
            };
            
            Layout.AddElement(staticFrictionField);
            Layout.AddElement(dynamicFrictionField);
            Layout.AddElement(restitutionField);
        }
    }

    /** @} */
}