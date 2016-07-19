//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
using System;
using BansheeEngine;

namespace BansheeEditor
{
    /** @addtogroup AnimationEditor
     *  @{
     */

    internal class GUIGraphValues
    {
        private static readonly Color COLOR_DARK_GRAY = new Color(40.0f / 255.0f, 40.0f / 255.0f, 40.0f / 255.0f, 1.0f);

        private GUITicks tickHandler;
        private GUICanvas canvas;

        private int width = 20;
        private int height = 20;
        private float rangeStart = -1.0f;
        private float rangeEnd = 1.0f;

        private float maxTextHeight;

        public GUIGraphValues(GUILayout layout, int width, int height)
        {
            canvas = new GUICanvas();
            layout.AddElement(canvas);

            tickHandler = new GUITicks();

            maxTextHeight = GUIUtility.CalculateTextBounds("99:999", EditorBuiltin.DefaultFont,
               EditorStyles.DefaultFontSize).y;

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

        private void DrawTime(int yPos, float seconds, bool minutes)
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
            textPosition.y = yPos - textBounds.y;

            canvas.DrawText(timeString, textPosition, EditorBuiltin.DefaultFont, Color.LightGray,
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

                        Color color = Color.LightGray;
                        color.a *= strength;

                        canvas.DrawLine(start, end, color);

                        // Draw text for the highest level ticks
                        if (i == 0)
                            DrawTime(yPos, ticks[j], displayAsMinutes);
                    }
                }
            }
        }
    }

    /** @} */
}
