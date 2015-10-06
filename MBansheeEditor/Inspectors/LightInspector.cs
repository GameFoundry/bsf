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

                colorField.OnChanged += x => light.Color = x;
                rangeField.OnChanged += x => light.Range = x;
                intensityField.OnChanged += x => light.Intensity = x;
                spotAngleField.OnChanged += x => light.SpotAngle = x;
                spotFalloffAngleField.OnChanged += x => light.SpotFalloffAngle = x;
                castShadowField.OnChanged += x => light.CastsShadow = x;
                
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
        protected internal override void Refresh()
        {
            Light light = InspectedObject as Light;
            if (light == null)
                return;

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

    }
}
