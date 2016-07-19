//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
using System;
using BansheeEngine;

namespace BansheeEditor
{
    /** @addtogroup AnimationEditor
     *  @{
     */

    // TODO DOC
    internal class GUIGraphValues
    {
        private static readonly Color COLOR_TRANSPARENT_LIGHT_GRAY = 
            new Color(200.0f / 255.0f, 200.0f / 255.0f, 200.0f / 255.0f, 0.5f);

        private GUITicks tickHandler;
        private GUICanvas canvas;

        private int width = 20;
        private int height = 20;
        private float rangeStart = -1.0f;
        private float rangeEnd = 1.0f;

        public GUIGraphValues(GUILayout layout, int width, int height)
        {
            canvas = new GUICanvas();
            layout.AddElement(canvas);

            tickHandler = new GUITicks();

            SetSize(width, height);
        }

        public void SetSize(int width, int height)
        {
            this.width = width;
            this.height = height;

            canvas.SetWidth(width);
            canvas.SetHeight(height);

            tickHandler.SetRange(rangeStart, rangeEnd, height);

            Rebuild();
        }

        public void SetRange(float start, float end)
        {
            if (start > end)
            {
                float temp = start;
                start = end;
                end = temp;
            }

            rangeStart = start;
            rangeEnd = end;

            tickHandler.SetRange(rangeStart, rangeEnd, height);

            Rebuild();
        }

        private void DrawTime(int yPos, float seconds, bool minutes, bool above)
        {
            TimeSpan timeSpan = TimeSpan.FromSeconds(seconds);

            string timeString;
            if (minutes)
                timeString = timeSpan.TotalMinutes.ToString("#0") + ":" + timeSpan.Seconds.ToString("D2");
            else
                timeString = timeSpan.TotalSeconds.ToString("#0.00");

            Vector2I textBounds = GUIUtility.CalculateTextBounds(timeString, EditorBuiltin.DefaultFont,
                EditorStyles.DefaultFontSize);

            Vector2I textPosition = new Vector2I();
            textPosition.x = width - textBounds.x;

            if (above)
                textPosition.y = yPos - textBounds.y;
            else // Below
            {
                const int PADDING = 3; // So the text doesn't touch the tick
                textPosition.y = yPos + PADDING;
            }
            canvas.DrawText(timeString, textPosition, EditorBuiltin.DefaultFont, COLOR_TRANSPARENT_LIGHT_GRAY,
                EditorStyles.DefaultFontSize);
        }

        private void Rebuild()
        {
            canvas.Clear();

            int heightOffset = height/2;
            float pixelsPerHeight;

            if (rangeEnd != rangeStart)
                pixelsPerHeight = height/(rangeEnd - rangeStart);
            else
                pixelsPerHeight = 0;

            int numTickLevels = tickHandler.NumLevels;
            for (int i = numTickLevels - 1; i >= 0; i--)
            {
                float[] ticks = tickHandler.GetTicks(i);
                float strength = tickHandler.GetLevelStrength(i);
               
                if (ticks.Length > 0)
                {
                    float valuePerTick = (rangeEnd - rangeStart)/ticks.Length;
                    bool displayAsMinutes = TimeSpan.FromSeconds(valuePerTick).Minutes > 0;

                    for (int j = 0; j < ticks.Length; j++)
                    {
                        int yPos = (int) (ticks[j]*pixelsPerHeight);
                        yPos = heightOffset - yPos; // Offset and flip height (canvas Y goes down)

                        Vector2I start = new Vector2I(0, yPos);
                        Vector2I end = new Vector2I((int) (width*strength), yPos);

                        Color color = COLOR_TRANSPARENT_LIGHT_GRAY;
                        color.a *= MathEx.Clamp01(strength);

                        canvas.DrawLine(start, end, color);

                        // Draw text for the highest level ticks
                        if (i == 0)
                            DrawTime(yPos, ticks[j], displayAsMinutes, ticks[j] <= 0.0f);
                    }
                }
            }
        }
    }

    /** @} */
}
