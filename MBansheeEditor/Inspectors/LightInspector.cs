using System.Collections.Generic;
using BansheeEngine;

namespace BansheeEditor
{
    /// <summary>
    /// Renders an inspector for the <see cref="Light"/> component.
    /// </summary>
    [CustomInspector(typeof(Light))]
    public class LightInspector : Inspector
    {
        private GUIEnumField lightTypeField = new GUIEnumField(typeof(LightType), new LocEdString("Light type"));
        private GUIColorField colorField = new GUIColorField(new LocEdString("Color"));
        private GUIFloatField rangeField = new GUIFloatField(new LocEdString("Range"));
        private GUIFloatField intensityField = new GUIFloatField(new LocEdString("Intensity"));
        private GUISliderField spotAngleField = new GUISliderField(1, 180, new LocEdString("Spot angle"));
        private GUISliderField spotFalloffAngleField = new GUISliderField(1, 180, new LocEdString("Spot falloff angle"));
        private GUIToggleField castShadowField = new GUIToggleField(new LocEdString("Cast shadow"));

        private InspectableState modifyState;

        /// <inheritdoc/>
        protected internal override void Initialize()
        {
            if (InspectedObject != null)
            {
                Light light = (Light)InspectedObject;

                lightTypeField.OnSelectionChanged += x =>
                {
                    light.Type = (LightType)x;

                    ToggleTypeSpecificFields((LightType) x);
                };

                colorField.OnChanged += x =>
                {
                    light.Color = x;
                    MarkAsModified();
                    ConfirmModify();
                };

                rangeField.OnChanged += x => { light.Range = x; MarkAsModified(); };
                rangeField.OnConfirmed += ConfirmModify;
                rangeField.OnFocusLost += ConfirmModify;

                intensityField.OnChanged += x => { light.Intensity = x; MarkAsModified(); };
                intensityField.OnConfirmed += ConfirmModify;
                intensityField.OnFocusLost += ConfirmModify;

                spotAngleField.OnChanged += x => { light.SpotAngle = x; MarkAsModified(); };
                spotAngleField.OnFocusLost += ConfirmModify;

                spotFalloffAngleField.OnChanged += x => { light.SpotFalloffAngle = x; MarkAsModified(); };
                spotFalloffAngleField.OnFocusLost += ConfirmModify;

                castShadowField.OnChanged += x =>
                {
                    light.CastsShadow = x;
                    MarkAsModified();
                    ConfirmModify();
                };
                
                Layout.AddElement(lightTypeField);
                Layout.AddElement(colorField);
                Layout.AddElement(intensityField);
                Layout.AddElement(rangeField);
                Layout.AddElement(spotAngleField);
                Layout.AddElement(spotFalloffAngleField);
                Layout.AddElement(castShadowField);

                ToggleTypeSpecificFields(light.Type);
            }
        }

        /// <inheritdoc/>
        protected internal override InspectableState Refresh()
        {
            Light light = InspectedObject as Light;
            if (light == null)
                return InspectableState.NotModified;

            LightType lightType = light.Type;
            if (lightTypeField.Value != (ulong)lightType)
            {
                lightTypeField.Value = (ulong)lightType;
                ToggleTypeSpecificFields(lightType);
            }

            colorField.Value = light.Color;
            intensityField.Value = light.Intensity;
            rangeField.Value = light.Range;
            spotAngleField.Value = light.SpotAngle.Degrees;
            spotFalloffAngleField.Value = light.SpotFalloffAngle.Degrees;
            castShadowField.Value = light.CastsShadow;

            InspectableState oldState = modifyState;
            if (modifyState.HasFlag(InspectableState.Modified))
                modifyState = InspectableState.NotModified;

            return oldState;
        }

        /// <summary>
        /// Enables or disables different GUI elements depending on the light type.
        /// </summary>
        /// <param name="type">Light type to show GUI elements for.</param>
        private void ToggleTypeSpecificFields(LightType type)
        {
            if (type == LightType.Directional)
            {
                rangeField.Active = false;
                spotAngleField.Active = false;
                spotFalloffAngleField.Active = false;
            }
            else if (type == LightType.Point)
            {
                rangeField.Active = true;
                spotAngleField.Active = false;
                spotFalloffAngleField.Active = false;
            }
            else
            {
                rangeField.Active = true;
                spotAngleField.Active = true;
                spotFalloffAngleField.Active = true;
            }
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
