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

        private ColorBoxMode colorBoxMode = ColorBoxMode.BG_R;
        private SliderMode sliderMode = SliderMode.HSV;

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

        private GUIButton guiOK;
        private GUIButton guiCancel;

        private Action<bool, Color> closedCallback;

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

        public static ColorPicker Show(Action<bool, Color> closedCallback = null)
        {
            ColorPicker picker = new ColorPicker();
            picker.closedCallback = closedCallback;

            return picker;
        }

        public static ColorPicker Show(Color color, Action<bool, Color> closedCallback = null)
        {
            ColorPicker picker = new ColorPicker();
            picker.colRed = color.r;
            picker.colGreen = color.g;
            picker.colBlue = color.b; 
            picker.colAlpha = color.a;
            picker.closedCallback = closedCallback;

            return picker;
        }

        protected ColorPicker()
            : base(false)
        { }

        private void OnInitialize()
        {
            Title = new LocEdString("Color Picker");
            Width = 270;
            Height = 400;

            guiColor = new GUIColorField("", GUIOption.FixedWidth(100));
            guiSlider2DTex = new GUITexture(null, GUIOption.FixedHeight(200), GUIOption.FixedWidth(200));
            guiSliderVertTex = new GUITexture(null, GUIOption.FixedHeight(200), GUIOption.FixedWidth(40));
            guiSliderRHorzTex = new GUITexture(null, GUIOption.FixedHeight(15));
            guiSliderGHorzTex = new GUITexture(null, GUIOption.FixedHeight(15));
            guiSliderBHorzTex = new GUITexture(null, GUIOption.FixedHeight(15));
            guiSliderAHorzTex = new GUITexture(null, GUIOption.FixedHeight(15));

            guiColorBoxBtn = new GUIButton(colorBoxMode.ToString());
            guiColorModeBtn = new GUIButton(sliderMode.ToString());

            guiSliderVert = new GUISliderV(EditorStyles.ColorSliderVert);
            guiSliderRHorz = new GUISliderH(EditorStyles.ColorSliderHorz);
            guiSliderGHorz = new GUISliderH(EditorStyles.ColorSliderHorz);
            guiSliderBHorz = new GUISliderH(EditorStyles.ColorSliderHorz);
            guiSliderAHorz = new GUISliderH(EditorStyles.ColorSliderHorz);
            guiSlider2DHandle = new GUITexture(null, EditorStyles.ColorSlider2DHandle);

            guiLabelR = new GUILabel(new LocEdString("R"));
            guiLabelG = new GUILabel(new LocEdString("G"));
            guiLabelB = new GUILabel(new LocEdString("B"));
            guiLabelA = new GUILabel(new LocEdString("A"));

            guiInputR = new GUIIntField();
            guiInputG = new GUIIntField();
            guiInputB = new GUIIntField();
            guiInputA = new GUIIntField();

            guiOK = new GUIButton(new LocEdString("OK"));
            guiCancel = new GUIButton(new LocEdString("Cancel"));

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

            guiOK.OnClick += OnOK;
            guiCancel.OnClick += OnCancel;

            GUIPanel mainPanel = GUI.AddPanel(0);
            GUILayout v0 = mainPanel.AddLayoutY();

            v0.AddSpace(5);

            GUILayout h0 = v0.AddLayoutX();
            h0.AddSpace(10);
            h0.AddElement(guiColor);
            h0.AddFlexibleSpace();
            h0.AddElement(guiColorBoxBtn);
            h0.AddElement(guiColorModeBtn);
            h0.AddSpace(10);

            v0.AddSpace(10);

            GUILayout h1 = v0.AddLayoutX();
            h1.AddSpace(10);
            h1.AddElement(guiSlider2DTex);
            h1.AddFlexibleSpace();
            h1.AddElement(guiSliderVertTex);
            h1.AddSpace(10);

            v0.AddSpace(10);

            GUILayout h2 = v0.AddLayoutX();
            h2.AddSpace(10);
            h2.AddElement(guiLabelR);
            h2.AddFlexibleSpace();
            h2.AddElement(guiSliderRHorzTex);
            h2.AddFlexibleSpace();
            h2.AddElement(guiInputR);
            h2.AddSpace(10);

            v0.AddSpace(5);

            GUILayout h3 = v0.AddLayoutX();
            h3.AddSpace(10);
            h3.AddElement(guiLabelG);
            h3.AddFlexibleSpace();
            h3.AddElement(guiSliderGHorzTex);
            h3.AddFlexibleSpace();
            h3.AddElement(guiInputG);
            h3.AddSpace(10);

            v0.AddSpace(5);

            GUILayout h4 = v0.AddLayoutX();
            h4.AddSpace(10);
            h4.AddElement(guiLabelB);
            h4.AddFlexibleSpace();
            h4.AddElement(guiSliderBHorzTex);
            h4.AddFlexibleSpace();
            h4.AddElement(guiInputB);
            h4.AddSpace(10);

            v0.AddSpace(5);

            GUILayout h5 = v0.AddLayoutX();
            h5.AddSpace(10);
            h5.AddElement(guiLabelA);
            h5.AddFlexibleSpace();
            h5.AddElement(guiSliderAHorzTex);
            h5.AddFlexibleSpace();
            h5.AddElement(guiInputA);
            h5.AddSpace(10);

            v0.AddSpace(10);

            GUILayout h6 = v0.AddLayoutX();
            h6.AddFlexibleSpace();
            h6.AddElement(guiOK);
            h6.AddSpace(10);
            h6.AddElement(guiCancel);
            h6.AddFlexibleSpace();

            v0.AddSpace(5);

            GUIPanel overlay = GUI.AddPanel(-1);
            overlay.SetWidth(Width);
            overlay.SetHeight(Height);

            overlay.AddElement(guiSliderVert);
            overlay.AddElement(guiSliderRHorz);
            overlay.AddElement(guiSliderGHorz);
            overlay.AddElement(guiSliderBHorz);
            overlay.AddElement(guiSliderAHorz);
            overlay.AddElement(guiSlider2DHandle);

            colorBox = new ColorSlider2D(guiSlider2DTex, guiSlider2DHandle, ColorBoxWidth, ColorBoxHeight);
            sideSlider = new ColorSlider1DVert(guiSliderVertTex, guiSliderVert, SliderSideWidth, SliderSideHeight);

            sliderR = new ColorSlider1DHorz(guiSliderRHorzTex, guiSliderRHorz, SliderIndividualWidth, SliderIndividualHeight);
            sliderG = new ColorSlider1DHorz(guiSliderGHorzTex, guiSliderGHorz, SliderIndividualWidth, SliderIndividualHeight);
            sliderB = new ColorSlider1DHorz(guiSliderBHorzTex, guiSliderBHorz, SliderIndividualWidth, SliderIndividualHeight);
            sliderA = new ColorSlider1DHorz(guiSliderAHorzTex, guiSliderAHorz, SliderIndividualWidth, SliderIndividualHeight);

            colorBox.OnValueChanged += OnColorBoxValueChanged;

            Color startA = new Color(0, 0, 0, 1);
            Color stepA = new Color(1, 1, 1, 0);
            sliderA.UpdateTexture(startA, stepA, false);
            guiInputA.SetRange(0, 255);
            guiInputA.Value = 255;
            guiSliderAHorz.Percent = 1.0f;

            guiColor.Value = SelectedColor;
            UpdateInputBoxValues();
            Update2DSliderTextures();
            Update2DSliderValues();
            Update1DSliderTextures();
            Update1DSliderValues();
            UpdateSliderMode();
        }

        private void OnEditorUpdate()
        {
            Vector2I windowPos = ScreenToWindowPos(Input.PointerPosition);

            colorBox.UpdateInput(windowPos);
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
            for (int y = 0; y < height; y++)
            {
                int rowIdx = (height - y - 1) * width;

                Color currentColor = verticalColor;
                for (int x = 0; x < width; x++)
                {
                    colors[rowIdx + x] = currentColor;
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

            UpdateSliderMode();

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
                    colBlue = value.x;
                    colGreen = value.y;
                    RGBToHSV();
                    break;
                case ColorBoxMode.BR_G:
                    colBlue = value.x;
                    colRed = value.y;
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
            UpdateSideSliderTexture();

            Vector2 xy;
            float z;

            GetColorBoxValues(out xy, out z);
            guiSliderVert.Percent = 1.0f - z;
        }

        void OnSliderVertChanged(float percent)
        {
            percent = 1.0f - percent;

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
            guiInputA.Value = MathEx.RoundToInt(colAlpha * 255.0f);
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

        void OnOK()
        {
            if (closedCallback != null)
                closedCallback(true, SelectedColor);

            Close();
        }

        void OnCancel()
        {
            if (closedCallback != null)
                closedCallback(false, SelectedColor);

            Close();
        }

        void UpdateSliderMode()
        {
            if (sliderMode == SliderMode.RGB)
            {
                guiLabelR.SetContent(new LocEdString("R"));
                guiLabelG.SetContent(new LocEdString("G"));
                guiLabelB.SetContent(new LocEdString("B"));

                guiInputR.SetRange(0, 255);
                guiInputG.SetRange(0, 255);
                guiInputB.SetRange(0, 255);
            }
            else
            {
                guiLabelR.SetContent(new LocEdString("H"));
                guiLabelG.SetContent(new LocEdString("S"));
                guiLabelB.SetContent(new LocEdString("V"));

                guiInputR.SetRange(0, 359);
                guiInputG.SetRange(0, 255);
                guiInputB.SetRange(0, 255);
            }
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

        void GetColorBoxValues(out Vector2 xy, out float z)
        {
            xy = Vector2.Zero;
            z = 0.0f;

            switch (colorBoxMode)
            {
                case ColorBoxMode.BG_R:
                    xy.x = colBlue;
                    xy.y = colGreen;
                    z = colRed;
                    break;
                case ColorBoxMode.BR_G:
                    xy.x = colBlue;
                    xy.y = colRed;
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
        }

        void Update2DSliderValues()
        {
            Vector2 xy = Vector2.Zero;
            float z = 0.0f;

            GetColorBoxValues(out xy, out z);

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

                sliderR.UpdateTexture(startH, stepH, true);

                Color startS = new Color(colHue, 0, MathEx.Max(colValue, 0.2f));
                Color stepS = new Color(0, 1, 0, 0);

                sliderG.UpdateTexture(startS, stepS, true);

                Color startV = new Color(colHue, colSaturation, 0);
                Color stepV = new Color(0, 0, 1, 0);

                sliderB.UpdateTexture(startV, stepV, true);
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

        void UpdateSideSliderTexture()
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
        }

        void Update2DSliderTextures()
        {
            UpdateSideSliderTexture();

            float[] valueLookup = new float[] { colRed, colGreen, colBlue, colHue, colSaturation, colValue };
            colorBox.UpdateTexture(colorBoxMode, valueLookup[(int)colorBoxMode]);
        }

        public class ColorSlider1DHorz
        {
            private const int SLIDER_X_OFFSET = 3;
            private const int SLIDER_Y_OFFSET = 5;

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

                Rect2I sliderBounds = guiTexture.Bounds;
                sliderBounds.x -= SLIDER_X_OFFSET;
                sliderBounds.width += SLIDER_X_OFFSET*2;
                sliderBounds.y -= SLIDER_Y_OFFSET;
                sliderBounds.height += SLIDER_Y_OFFSET;

                guiSlider.Bounds = sliderBounds;
            }
        }

        public class ColorSlider1DVert
        {
            private const int SLIDER_X_OFFSET = 5;
            private const int SLIDER_Y_OFFSET = 3;

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

                Rect2I sliderBounds = guiTexture.Bounds;
                sliderBounds.x -= SLIDER_X_OFFSET;
                sliderBounds.width += SLIDER_X_OFFSET;
                sliderBounds.y -= SLIDER_Y_OFFSET;
                sliderBounds.height += SLIDER_Y_OFFSET * 2;

                guiSlider.Bounds = sliderBounds;
            }
        }

        public class ColorSlider2D
        {
            private int width, height;
            private Texture2D texture;
            private SpriteTexture spriteTexture;

            private GUITexture guiTexture;
            private GUITexture guiSliderHandle;

            private Vector2 oldValue = new Vector2(-1, -1);

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
                        Vector2 newValue = Vector2.Zero;
                        newValue.x = (windowPos.x - bounds.x) / (float)bounds.width;
                        newValue.y = 1.0f - (windowPos.y - bounds.y) / (float)bounds.height;

                        SetValue(newValue);
                    }
                }
            }

            public void SetValue(Vector2 value)
            {
                Vector2 pos = value;
                pos.y = 1.0f - pos.y;

                if (oldValue == value)
                    return;

                Rect2I handleBounds = guiSliderHandle.Bounds;
                Rect2I boxBounds = guiTexture.Bounds;

                handleBounds.x = boxBounds.x + MathEx.RoundToInt(pos.x * boxBounds.width) - handleBounds.width / 2;
                handleBounds.y = boxBounds.y + MathEx.RoundToInt(pos.y * boxBounds.height) - handleBounds.height / 2;

                guiSliderHandle.Bounds = handleBounds;
                oldValue = value;

                if (OnValueChanged != null)
                    OnValueChanged(value);
            }
        }
    }
}
