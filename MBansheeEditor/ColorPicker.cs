using System;
using BansheeEngine;

namespace BansheeEditor
{
    public class ColorPicker : ModalWindow
    {
        private const int SliderIndividualWidth = 150;
        private const int SliderIndividualHeight = 20;

        private const int ColorBoxWidth = 150;
        private const int ColorBoxHeight = 150;

        private const int SliderSideWidth = 40;
        private const int SliderSideHeight = 150;

        private float colRed, colGreen, colBlue;
        private float colHue, colSaturation, colValue;
        private float colAlpha = 1.0f;

        private ColorSlider1DHorz sliderR, sliderG, sliderB, sliderA;
        private ColorSlider2D colorBox;
        private ColorSlider1DVert sideSlider;

        private ColorBoxMode colorBoxMode;
        private SliderMode sliderMode;

        private GUIColorField guiColor;
        private GUITexture guiSlider2DTex;
        private GUITexture guiSliderVertTex;
        private GUITexture guiSliderRHorzTex;
        private GUITexture guiSliderGHorzTex;
        private GUITexture guiSliderBHorzTex;
        private GUITexture guiSliderAHorzTex;

        private GUIButton guiColorBoxBtn;
        private GUIButton guiColorModeBtn;

        private GUISliderV guiSliderVert;
        private GUISliderH guiSliderRHorz;
        private GUISliderH guiSliderGHorz;
        private GUISliderH guiSliderBHorz;
        private GUISliderH guiSliderAHorz;
        private GUITexture guiSlider2DHandle;

        private GUILabel guiLabelR;
        private GUILabel guiLabelG;
        private GUILabel guiLabelB;
        private GUILabel guiLabelA;

        private GUIIntField guiInputR;
        private GUIIntField guiInputG;
        private GUIIntField guiInputB;
        private GUIIntField guiInputA;

        public enum ColorBoxMode
        {
            BG_R,
            BR_G,
            RG_B,
            SV_H,
            HV_S,
            HS_V
        }

        public enum SliderMode
        {
            RGB,
            HSV
        }

        public Color SelectedColor
        {
            get
            {
                return new Color(colRed, colGreen, colBlue, colAlpha);
            }
        }

        public static ColorPicker Show()
        {
            return new ColorPicker();
        }

        protected ColorPicker()
            : base(false)
        {
            Title = "Color Picker";
            Width = 400;
            Height = 800;
        }

        private void OnInitialize()
        {
            guiColor = new GUIColorField();
            guiSlider2DTex = new GUITexture(null);
            guiSliderVertTex = new GUITexture(null);
            guiSliderRHorzTex = new GUITexture(null);
            guiSliderGHorzTex = new GUITexture(null);
            guiSliderBHorzTex = new GUITexture(null);
            guiSliderAHorzTex = new GUITexture(null);

            guiColorBoxBtn = new GUIButton(colorBoxMode.ToString());
            guiColorModeBtn = new GUIButton(sliderMode.ToString());

            guiSliderVert = new GUISliderV(EditorStyles.ColorSliderVert);
            guiSliderRHorz = new GUISliderH(EditorStyles.ColorSliderHorz);
            guiSliderGHorz = new GUISliderH(EditorStyles.ColorSliderHorz);
            guiSliderBHorz = new GUISliderH(EditorStyles.ColorSliderHorz);
            guiSliderAHorz = new GUISliderH(EditorStyles.ColorSliderHorz);
            guiSlider2DHandle = new GUITexture(null);

            guiLabelR = new GUILabel("R");
            guiLabelG = new GUILabel("G");
            guiLabelB = new GUILabel("B");
            guiLabelA = new GUILabel("A");

            guiInputR = new GUIIntField();
            guiInputG = new GUIIntField();
            guiInputB = new GUIIntField();
            guiInputA = new GUIIntField();

            guiColorBoxBtn.OnClick += OnColorBoxModeChanged;
            guiColorModeBtn.OnClick += OnSliderModeChanged;

            guiSliderVert.OnChanged += OnSliderVertChanged;
            guiSliderRHorz.OnChanged += OnSliderRHorzChanged;
            guiSliderGHorz.OnChanged += OnSliderGHorzChanged;
            guiSliderBHorz.OnChanged += OnSliderBHorzChanged;
            guiSliderAHorz.OnChanged += OnSliderAHorzChanged;

            guiInputR.OnChanged += OnInputRChanged;
            guiInputG.OnChanged += OnInputGChanged;
            guiInputB.OnChanged += OnInputBChanged;
            guiInputA.OnChanged += OnInputAChanged;

            GUILayout v0 = GUI.layout.AddLayoutY();

            GUILayout h0 = v0.AddLayoutX();
            h0.AddElement(guiColor);
            h0.AddFlexibleSpace();
            h0.AddElement(guiColorBoxBtn);
            h0.AddElement(guiColorModeBtn);

            v0.AddSpace(20);

            GUILayout h1 = v0.AddLayoutX();
            h1.AddElement(guiSlider2DTex);
            h1.AddFlexibleSpace();
            h1.AddElement(guiSliderVertTex);
            h1.AddSpace(10);

            v0.AddSpace(30);

            GUILayout h2 = v0.AddLayoutX();
            h2.AddElement(guiLabelR);
            h2.AddFlexibleSpace();
            h2.AddElement(guiSliderRHorzTex);
            h2.AddFlexibleSpace();
            h2.AddElement(guiInputR);

            v0.AddSpace(15);

            GUILayout h3 = v0.AddLayoutX();
            h3.AddElement(guiLabelG);
            h3.AddFlexibleSpace();
            h3.AddElement(guiSliderGHorzTex);
            h3.AddFlexibleSpace();
            h3.AddElement(guiInputG);

            v0.AddSpace(15);

            GUILayout h4 = v0.AddLayoutX();
            h4.AddElement(guiLabelB);
            h4.AddFlexibleSpace();
            h4.AddElement(guiSliderBHorzTex);
            h4.AddFlexibleSpace();
            h4.AddElement(guiInputB);

            v0.AddSpace(15);

            GUILayout h5 = v0.AddLayoutX();
            h5.AddElement(guiLabelA);
            h5.AddFlexibleSpace();
            h5.AddElement(guiSliderAHorzTex);
            h5.AddFlexibleSpace();
            h5.AddElement(guiInputA);

            GUIArea overlay = GUI.AddArea(0, 0, Width, Height, -1, GUILayoutType.Explicit);
            overlay.layout.AddElement(guiSliderVert);
            overlay.layout.AddElement(guiSliderRHorz);
            overlay.layout.AddElement(guiSliderGHorz);
            overlay.layout.AddElement(guiSliderBHorz);
            overlay.layout.AddElement(guiSliderAHorz);
            overlay.layout.AddElement(guiSlider2DHandle);

            colorBox = new ColorSlider2D(guiSlider2DTex, guiSlider2DHandle, ColorBoxWidth, ColorBoxHeight);
            sideSlider = new ColorSlider1DVert(guiSliderVertTex, guiSliderVert, SliderSideWidth, SliderSideHeight);

            sliderR = new ColorSlider1DHorz(guiSliderRHorzTex, guiSliderRHorz, SliderIndividualWidth, SliderIndividualHeight);
            sliderG = new ColorSlider1DHorz(guiSliderGHorzTex, guiSliderGHorz, SliderIndividualWidth, SliderIndividualHeight);
            sliderB = new ColorSlider1DHorz(guiSliderBHorzTex, guiSliderBHorz, SliderIndividualWidth, SliderIndividualHeight);
            sliderA = new ColorSlider1DHorz(guiSliderAHorzTex, guiSliderAHorz, SliderIndividualWidth, SliderIndividualHeight);

            colorBox.OnValueChanged += OnColorBoxValueChanged;

            guiColor.Value = SelectedColor;
            UpdateInputBoxValues();
            Update2DSliderTextures();
            Update2DSliderValues();
            Update1DSliderTextures();
            Update1DSliderValues();

            Color startA = new Color(0, 0, 0, 1);
            Color stepA = new Color(1, 1, 1, 0);
            sliderA.UpdateTexture(startA, stepA, false);
            guiInputA.SetRange(0, 255);
        }

        private void OnEditorUpdate()
        {
            Vector2I windowPos = ScreenToWindowPos(Input.PointerPosition);

            colorBox.UpdateInput(windowPos);

            Debug.Log(Width + " - " + Height + " - " + GUI.childAreas[0].mCachedPtr);
        }

        private static void FillArea(int width, int height, Color[] colors, Color start, Color rightGradient, Color downGradient)
        {
            Color rightDelta = new Color(0, 0, 0, 0);
            if (width > 1)
                rightDelta = rightGradient / (width - 1);

            Color downDelta = new Color(0, 0, 0, 0);
            if (height > 1)
                downDelta = downGradient / (height - 1);

            Color verticalColor = start;
            int idx = 0;
            for (int y = 0; y < height; y++)
            {
                Color currentColor = verticalColor;
                for (int x = 0; x < width; x++)
                {
                    colors[idx++] = currentColor;
                    currentColor += rightDelta;
                }
                verticalColor += downDelta;
            }
        }

        void HSVToRGB()
        {
            Color hsv = new Color(colHue, colSaturation, colValue);
            Color rgb = Color.HSV2RGB(hsv);

            colRed = rgb.r;
            colGreen = rgb.g;
            colBlue = rgb.b;
        }

        void RGBToHSV()
        {
            Color rgb = new Color(colRed, colGreen, colBlue);
            Color hsv = Color.RGB2HSV(rgb);

            colHue = hsv.r;
            colSaturation = hsv.g;
            colValue = hsv.b;
        }

        void OnColorBoxModeChanged()
        {
            int maxModes = Enum.GetNames(colorBoxMode.GetType()).Length;
            colorBoxMode = (ColorBoxMode)(((int)colorBoxMode + 1) % maxModes);

            guiColorBoxBtn.SetContent(colorBoxMode.ToString());
            Update2DSliderTextures();
            Update2DSliderValues();
        }

        void OnSliderModeChanged()
        {
            int maxModes = Enum.GetNames(sliderMode.GetType()).Length;
            sliderMode = (SliderMode)(((int)sliderMode + 1) % maxModes);

            if (sliderMode == SliderMode.RGB)
            {
                guiLabelR.SetContent("R");
                guiLabelG.SetContent("G");
                guiLabelB.SetContent("B");

                guiInputR.SetRange(0, 255);
                guiInputG.SetRange(0, 255);
                guiInputB.SetRange(0, 255);
            }
            else
            {
                guiLabelR.SetContent("H");
                guiLabelG.SetContent("S");
                guiLabelB.SetContent("V");

                guiInputR.SetRange(0, 359);
                guiInputG.SetRange(0, 255);
                guiInputB.SetRange(0, 255);
            }

            guiColorModeBtn.SetContent(sliderMode.ToString());
            UpdateInputBoxValues();
            Update1DSliderTextures();
            Update1DSliderValues();
        }

        void OnColorBoxValueChanged(Vector2 value)
        {
            switch (colorBoxMode)
            {
                case ColorBoxMode.BG_R:
                    colGreen = value.x;
                    colBlue = value.y;
                    RGBToHSV();
                    break;
                case ColorBoxMode.BR_G:
                    colRed = value.x;
                    colBlue = value.y;
                    RGBToHSV();
                    break;
                case ColorBoxMode.RG_B:
                    colRed = value.x;
                    colGreen = value.y;
                    RGBToHSV();
                    break;
                case ColorBoxMode.SV_H:
                    colSaturation = value.x;
                    colValue = value.y;
                    HSVToRGB();
                    break;
                case ColorBoxMode.HV_S:
                    colHue = value.x;
                    colValue = value.y;
                    HSVToRGB();
                    break;
                case ColorBoxMode.HS_V:
                    colHue = value.x;
                    colSaturation = value.y;
                    HSVToRGB();
                    break;
            }

            guiColor.Value = SelectedColor;
            UpdateInputBoxValues();
            Update1DSliderTextures();
            Update1DSliderValues();
        }

        void OnSliderVertChanged(float percent)
        {
            switch (colorBoxMode)
            {
                case ColorBoxMode.BG_R:
                    colRed = percent;
                    RGBToHSV();
                    break;
                case ColorBoxMode.BR_G:
                    colGreen = percent;
                    RGBToHSV();
                    break;
                case ColorBoxMode.RG_B:
                    colBlue = percent;
                    RGBToHSV();
                    break;
                case ColorBoxMode.SV_H:
                    colHue = percent;
                    HSVToRGB();
                    break;
                case ColorBoxMode.HV_S:
                    colSaturation = percent;
                    HSVToRGB();
                    break;
                case ColorBoxMode.HS_V:
                    colValue = percent;
                    HSVToRGB();
                    break;
            }

            guiColor.Value = SelectedColor;
            UpdateInputBoxValues();
            Update1DSliderTextures();
            Update1DSliderValues();
        }

        void OnSliderRHorzChanged(float percent)
        {
            bool isHSV = sliderMode == SliderMode.HSV;
            if (isHSV)
            {
                colHue = percent;
                HSVToRGB();
            }
            else
            {
                colRed = percent;
                RGBToHSV();
            }

            guiColor.Value = SelectedColor;
            UpdateInputBoxValues();
            Update2DSliderTextures();
            Update2DSliderValues();
        }

        void OnSliderGHorzChanged(float percent)
        {
            bool isHSV = sliderMode == SliderMode.HSV;
            if (isHSV)
            {
                colSaturation = percent;
                HSVToRGB();
            }
            else
            {
                colGreen = percent;
                RGBToHSV();
            }

            guiColor.Value = SelectedColor;
            UpdateInputBoxValues();
            Update2DSliderTextures();
            Update2DSliderValues();
        }

        void OnSliderBHorzChanged(float percent)
        {
            bool isHSV = sliderMode == SliderMode.HSV;
            if (isHSV)
            {
                colValue = percent;
                HSVToRGB();
            }
            else
            {
                colBlue = percent;
                RGBToHSV();
            }

            guiColor.Value = SelectedColor;
            UpdateInputBoxValues();
            Update2DSliderTextures();
            Update2DSliderValues();
        }

        void OnSliderAHorzChanged(float percent)
        {
            colAlpha = percent;

            guiColor.Value = SelectedColor;
            guiInputA.Value = MathEx.RoundToInt(colValue * 255.0f);
        }

        void OnInputRChanged(int value)
        {
            bool isHSV = sliderMode == SliderMode.HSV;
            if (isHSV)
            {
                colHue = value/359.0f;
                HSVToRGB();
            }
            else
            {
                colRed = value/255.0f;
                RGBToHSV();
            }

            guiColor.Value = SelectedColor;
            Update1DSliderValues();
            Update2DSliderTextures();
            Update2DSliderValues();
        }

        void OnInputGChanged(int value)
        {
            bool isHSV = sliderMode == SliderMode.HSV;
            if (isHSV)
            {
                colSaturation = value / 255.0f;
                HSVToRGB();
            }
            else
            {
                colGreen = value / 255.0f;
                RGBToHSV();
            }

            guiColor.Value = SelectedColor;
            Update1DSliderValues();
            Update2DSliderTextures();
            Update2DSliderValues();
        }

        void OnInputBChanged(int value)
        {
            bool isHSV = sliderMode == SliderMode.HSV;
            if (isHSV)
            {
                colValue = value / 255.0f;
                HSVToRGB();
            }
            else
            {
                colBlue = value / 255.0f;
                RGBToHSV();
            }

            guiColor.Value = SelectedColor;
            Update1DSliderValues();
            Update2DSliderTextures();
            Update2DSliderValues();
        }

        void OnInputAChanged(int value)
        {
            colAlpha = value/255.0f;

            guiColor.Value = SelectedColor;
            guiSliderAHorz.Percent = colAlpha;
        }

        void UpdateInputBoxValues()
        {
            bool isHSV = sliderMode == SliderMode.HSV;
            if (isHSV)
            {
                guiInputR.Value = MathEx.RoundToInt(colHue * 359.0f);
                guiInputG.Value = MathEx.RoundToInt(colSaturation * 255.0f);
                guiInputB.Value = MathEx.RoundToInt(colValue * 255.0f);
            }
            else
            {
                guiInputR.Value = MathEx.RoundToInt(colRed * 255.0f);
                guiInputG.Value = MathEx.RoundToInt(colGreen * 255.0f);
                guiInputB.Value = MathEx.RoundToInt(colBlue * 255.0f);
            }
        }

        void Update1DSliderValues()
        {
            bool isHSV = sliderMode == SliderMode.HSV;
            if (isHSV)
            {
                guiSliderRHorz.Percent = colHue;
                guiSliderGHorz.Percent = colSaturation;
                guiSliderBHorz.Percent = colValue;
            }
            else
            {
                guiSliderRHorz.Percent = colRed;
                guiSliderGHorz.Percent = colGreen;
                guiSliderBHorz.Percent = colBlue;
            }
        }

        void Update2DSliderValues()
        {
            Vector2 xy = Vector2.zero;
            float z = 0.0f;

            switch (colorBoxMode)
            {
                case ColorBoxMode.BG_R:
                    xy.x = colBlue;
                    xy.y = colGreen;
                    z = colRed;
                    break;
                case ColorBoxMode.BR_G:
                    xy.x = colRed;
                    xy.y = colBlue;
                    z = colGreen;
                    break;
                case ColorBoxMode.RG_B:
                    xy.x = colRed;
                    xy.y = colGreen;
                    z = colBlue;
                    break;
                case ColorBoxMode.SV_H:
                    xy.x = colSaturation;
                    xy.y = colValue;
                    z = colHue;
                    break;
                case ColorBoxMode.HV_S:
                    xy.x = colHue;
                    xy.y = colValue;
                    z = colSaturation;
                    break;
                case ColorBoxMode.HS_V:
                    xy.x = colHue;
                    xy.y = colSaturation;
                    z = colValue;
                    break;
            }

            colorBox.SetValue(xy);
            guiSliderVert.Percent = z;
        }

        void Update1DSliderTextures()
        {
            bool isHSV = sliderMode == SliderMode.HSV;

            if (isHSV)
            {
                Color startH = new Color(0, 1, 1);
                Color stepH = new Color(1, 0, 0, 0);

                sliderR.UpdateTexture(startH, stepH, false);

                Color startS = new Color(colHue, 0, MathEx.Max(colValue, 0.2f));
                Color stepS = new Color(0, 1, 0, 0);

                sliderG.UpdateTexture(startS, stepS, false);

                Color startV = new Color(colHue, colSaturation, 0);
                Color stepV = new Color(0, 0, 1, 0);

                sliderB.UpdateTexture(startV, stepV, false);
            }
            else
            {
                Color startR = new Color(0, colGreen, colBlue);
                Color stepR = new Color(1, 0, 0, 0);

                sliderR.UpdateTexture(startR, stepR, false);

                Color startG = new Color(colRed, 0, colBlue);
                Color stepG = new Color(0, 1, 0, 0);

                sliderG.UpdateTexture(startG, stepG, false);

                Color startB = new Color(colRed, colGreen, 0);
                Color stepB = new Color(0, 0, 1, 0);

                sliderB.UpdateTexture(startB, stepB, false);
            }
        }

        void Update2DSliderTextures()
        {
            switch (colorBoxMode)
            {
                case ColorBoxMode.BG_R:
                    sideSlider.UpdateTexture(new Color(0, colGreen, colBlue, 1), new Color(1, 0, 0, 0), false);
                    break;
                case ColorBoxMode.BR_G:
                    sideSlider.UpdateTexture(new Color(colRed, 0, colBlue, 1), new Color(0, 1, 0, 0), false);
                    break;
                case ColorBoxMode.RG_B:
                    sideSlider.UpdateTexture(new Color(colRed, colGreen, 0, 1), new Color(0, 0, 1, 0), false);
                    break;
                case ColorBoxMode.SV_H:
                    sideSlider.UpdateTexture(new Color(0, 1, 1, 1), new Color(1, 0, 0, 0), true);
                    break;
                case ColorBoxMode.HV_S:
                    sideSlider.UpdateTexture(new Color(colHue, 0, MathEx.Max(colValue, 0.2f), 1), new Color(0, 1, 0, 0), true);
                    break;
                case ColorBoxMode.HS_V:
                    sideSlider.UpdateTexture(new Color(colHue, colSaturation, 0, 1), new Color(0, 0, 1, 0), true);
                    break;
            }

            float[] valueLookup = new float[] { colRed, colGreen, colBlue, colHue, colSaturation, colValue };
            colorBox.UpdateTexture(colorBoxMode, valueLookup[(int)colorBoxMode]);
        }

        public class ColorSlider1DHorz
        {
            private const int SLIDER_HEIGHT = 8;

            private int width, height;
            private Texture2D texture;
            private SpriteTexture spriteTexture;

            private GUITexture guiTexture;
            private GUISliderH guiSlider;

            public ColorSlider1DHorz(GUITexture guiTexture, GUISliderH guiSlider, int width, int height)
            {
                this.width = width;
                this.height = height;
                this.guiTexture = guiTexture;
                this.guiSlider = guiSlider;

                texture = new Texture2D(width, height);
                spriteTexture = new SpriteTexture(texture);

                Rect2I sliderBounds = guiTexture.Bounds;
                sliderBounds.y -= SLIDER_HEIGHT;
                sliderBounds.height += SLIDER_HEIGHT;

                guiSlider.Bounds = sliderBounds;
            }

            public void UpdateTexture(Color start, Color step, bool isHSV)
            {
                Color[] colors = new Color[width * height];
                FillArea(width, height, colors, start, step, new Color(0, 0, 0, 0));

                if (isHSV)
                {
                    for (int i = 0; i < colors.Length; i++)
                        colors[i] = Color.HSV2RGB(colors[i]);
                }

                texture.SetPixels(colors);
                guiTexture.SetTexture(spriteTexture);
            }
        }

        public class ColorSlider1DVert
        {
            private const int SLIDER_WIDTH = 7;

            private int width, height;
            private Texture2D texture;
            private SpriteTexture spriteTexture;

            private GUITexture guiTexture;
            private GUISliderV guiSlider;

            public ColorSlider1DVert(GUITexture guiTexture, GUISliderV guiSlider, int width, int height)
            {
                this.width = width;
                this.height = height;
                this.guiTexture = guiTexture;
                this.guiSlider = guiSlider;

                texture = new Texture2D(width, height);
                spriteTexture = new SpriteTexture(texture);

                Rect2I sliderBounds = guiTexture.Bounds;
                sliderBounds.x -= SLIDER_WIDTH;
                sliderBounds.width += SLIDER_WIDTH;

                guiSlider.Bounds = sliderBounds;
            }

            public void UpdateTexture(Color start, Color step, bool isHSV)
            {
                Color[] colors = new Color[width * height];
                FillArea(width, height, colors, start, new Color(0, 0, 0, 0), step);

                if (isHSV)
                {
                    for (int i = 0; i < colors.Length; i++)
                        colors[i] = Color.HSV2RGB(colors[i]);
                }

                texture.SetPixels(colors);
                guiTexture.SetTexture(spriteTexture);
            }
        }

        public class ColorSlider2D
        {
            private int width, height;
            private Texture2D texture;
            private SpriteTexture spriteTexture;

            private GUITexture guiTexture;
            private GUITexture guiSliderHandle;

            private Vector2 oldValue;

            public delegate void OnValueChangedDelegate(Vector2 value);
            public event OnValueChangedDelegate OnValueChanged;

            public ColorSlider2D(GUITexture guiTexture, GUITexture guiSliderHandle, int width, int height)
            {
                this.width = width;
                this.height = height;

                this.guiTexture = guiTexture;
                this.guiSliderHandle = guiSliderHandle;

                texture = new Texture2D(width, height);
                spriteTexture = new SpriteTexture(texture);
            }

            public void UpdateTexture(ColorBoxMode mode, float value)
            {
                Color[] colors = new Color[width * height];

                switch (mode)
                {
                    case ColorBoxMode.BG_R:
                        FillArea(width, height, colors, new Color(value, 0, 0, 1), new Color(0, 0, 1, 0), new Color(0, 1, 0, 0));
                        break;
                    case ColorBoxMode.BR_G:
                        FillArea(width, height, colors, new Color(0, value, 0, 1), new Color(0, 0, 1, 0), new Color(1, 0, 0, 0));
                        break;
                    case ColorBoxMode.RG_B:
                        FillArea(width, height, colors, new Color(0, 0, value, 1), new Color(1, 0, 0, 0), new Color(0, 1, 0, 0));
                        break;
                    case ColorBoxMode.SV_H:
                        FillArea(width, height, colors, new Color(value, 0, 0, 1), new Color(0, 1, 0, 0), new Color(0, 0, 1, 0));
                        for (int i = 0; i < colors.Length; i++)
                            colors[i] = Color.HSV2RGB(colors[i]);
                        break;
                    case ColorBoxMode.HV_S:
                        FillArea(width, height, colors, new Color(0, value, 0, 1), new Color(1, 0, 0, 0), new Color(0, 0, 1, 0));
                        for (int i = 0; i < colors.Length; i++)
                            colors[i] = Color.HSV2RGB(colors[i]);
                        break;
                    case ColorBoxMode.HS_V:
                        FillArea(width, height, colors, new Color(0, 0, value, 1), new Color(1, 0, 0, 0), new Color(0, 1, 0, 0));
                        for (int i = 0; i < colors.Length; i++)
                            colors[i] = Color.HSV2RGB(colors[i]);
                        break;
                }

                texture.SetPixels(colors);
                guiTexture.SetTexture(spriteTexture);
            }

            public void UpdateInput(Vector2I windowPos)
            {
                if (Input.IsPointerButtonHeld(PointerButton.Left))
                {
                    Rect2I bounds = guiTexture.Bounds;

                    if (bounds.Contains(windowPos))
                    {
                        Vector2 newValue = Vector2.zero;
                        newValue.x = (windowPos.x - bounds.x) / (float)bounds.width;
                        newValue.y = (windowPos.y - bounds.y) / (float)bounds.height;

                        SetValue(newValue);
                    }
                }
            }

            public void SetValue(Vector2 value)
            {
                if (oldValue == value)
                    return;

                Rect2I handleBounds = guiSliderHandle.Bounds;
                Rect2I boxBounds = guiTexture.Bounds;

                handleBounds.x = boxBounds.x + MathEx.RoundToInt(value.x * boxBounds.width) - handleBounds.width / 2;
                handleBounds.y = boxBounds.y + MathEx.RoundToInt(value.y * boxBounds.height) - handleBounds.height / 2;

                guiSliderHandle.Bounds = handleBounds;

                oldValue = value;

                if (OnValueChanged != null)
                    OnValueChanged(value);
            }
        }
    }
}
