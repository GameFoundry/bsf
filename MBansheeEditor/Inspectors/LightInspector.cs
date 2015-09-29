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
        private bool isInitialized;
        private GUIEnumField lightTypeField = new GUIEnumField(typeof(LightType), new LocEdString("Light type"));
        private GUIColorField colorField = new GUIColorField(new LocEdString("Color"));
        private GUIFloatField rangeField = new GUIFloatField(new LocEdString("Range"));
        private GUIFloatField intensityField = new GUIFloatField(new LocEdString("Intensity"));
        private GUISliderField spotAngleField = new GUISliderField(1, 180, new LocEdString("Spot angle"));
        private GUISliderField spotFalloffAngleField = new GUISliderField(1, 180, new LocEdString("Spot falloff angle"));
        private GUIToggleField castShadowField = new GUIToggleField(new LocEdString("Cast shadow"));

        /// <summary>
        /// Initializes required data the first time <see cref="Refresh"/> is called.
        /// </summary>
        private void Initialize()
        {
            if (referencedObject != null)
            {
                Light light = (Light)referencedObject;

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
                
                layout.AddElement(lightTypeField);
                layout.AddElement(colorField);
                layout.AddElement(intensityField);
                layout.AddElement(rangeField);
                layout.AddElement(spotAngleField);
                layout.AddElement(spotFalloffAngleField);
                layout.AddElement(castShadowField);

                ToggleTypeSpecificFields(light.Type);
            }

            isInitialized = true;
        }

        /// <summary>
        /// Enables or disables different GUI elements depending on the light type.
        /// </summary>
        /// <param name="type">Light type to show GUI elements for.</param>
        private void ToggleTypeSpecificFields(LightType type)
        {
            if (type == LightType.Directional)
            {
                rangeField.Enabled = false;
                spotAngleField.Enabled = false;
                spotFalloffAngleField.Enabled = false;
            }
            else if (type == LightType.Point)
            {
                rangeField.Enabled = true;
                spotAngleField.Enabled = false;
                spotFalloffAngleField.Enabled = false;
            }
            else
            {
                rangeField.Enabled = true;
                spotAngleField.Enabled = true;
                spotFalloffAngleField.Enabled = true;
            }
        }

        /// <inheritdoc/>
        internal override bool Refresh()
        {
            if (!isInitialized)
                Initialize();

            Light light = referencedObject as Light;
            if (light == null)
                return false;

            bool anythingModified = false;

            LightType lightType = light.Type;
            if (lightTypeField.Value != (ulong)lightType)
            {
                lightTypeField.Value = (ulong)lightType;
                ToggleTypeSpecificFields(lightType);
                anythingModified = true;
            }

            if (colorField.Value != light.Color)
            {
                colorField.Value = light.Color;
                anythingModified = true;
            }

            if (intensityField.Value != light.Intensity)
            {
                intensityField.Value = light.Intensity;
                anythingModified = true;
            }

            if (rangeField.Value != light.Range)
            {
                rangeField.Value = light.Range;
                anythingModified = true;
            }

            if (spotAngleField.Value != light.SpotAngle.Degrees)
            {
                spotAngleField.Value = light.SpotAngle.Degrees;
                anythingModified = true;
            }

            if (spotFalloffAngleField.Value != light.SpotFalloffAngle.Degrees)
            {
                spotFalloffAngleField.Value = light.SpotFalloffAngle.Degrees;
                anythingModified = true;
            }

            if (castShadowField.Value != light.CastsShadow)
            {
                castShadowField.Value = light.CastsShadow;
                anythingModified = true;
            }

            return anythingModified;
        }
    }
}
