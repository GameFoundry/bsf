//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
using System;
using BansheeEngine;

namespace BansheeEditor
{
    /** @addtogroup AnimationEditor
     *  @{
     */

    internal enum GUITickStepType
    {
        Time,
        Generic
    }

    // TODO DOC
    internal class GUITicks
    {
        private int pixelRange = 100;
        private float valueRangeStart = 0.0f;
        private float valueRangeEnd = 1.0f;

        private int minTickSpacingPx = 5;
        private int maxTickSpacingPx = 30;

        private float[] validSteps = new [] { 1.0f };
        private float[] levelStrengths = new[] { 1.0f };
        private int numLevels = 1;
        private int maxLevel = 0;

        public int NumLevels { get { return numLevels; } }

        internal GUITicks(GUITickStepType stepType = GUITickStepType.Generic)
        {
            if(stepType == GUITickStepType.Generic)
                SetGenericSteps();
            else
                SetTimeSteps();

            Rebuild();
        }

        internal void SetRange(float valueRangeStart, float valueRangeEnd, int pixelRange)
        {
            this.valueRangeStart = valueRangeStart;
            this.valueRangeEnd = valueRangeEnd;
            this.pixelRange = pixelRange;

            Rebuild();
        }

        internal void SetTickSpacing(int minPx, int maxPx)
        {
            minTickSpacingPx = minPx;
            maxTickSpacingPx = maxPx;

            Rebuild();
        }

        internal float GetLevelStrength(int level)
        {
            if (level < 0 || level >= numLevels)
                return 0.0f;

            return levelStrengths[maxLevel + level];
        }

        internal float[] GetTicks(int level)
        {
            if (level < 0 || level >= numLevels)
                return new float[0];

            float step = validSteps[maxLevel + level];

            // Round up and down so we get one extra tick on either side (outside of value range)
            // (Useful when rendering text above the ticks, so the text doesn't just pop-in when the tick appears, instead
            // it is slowly clipped-in.)
            int startTick = MathEx.FloorToInt(valueRangeStart / step);
            int endTick = MathEx.CeilToInt(valueRangeEnd / step);

            int numTicks = endTick - startTick + 1;

            float[] ticks = new float[numTicks];
            for (int i = startTick; i <= endTick; i++)
                ticks[i - startTick] = i*step;

            return ticks;
        }

        private void Rebuild()
        {
            levelStrengths = new float[validSteps.Length];
            maxLevel = 0;

            float valueRange = valueRangeEnd - valueRangeStart;
            int tickSpacing = maxTickSpacingPx - minTickSpacingPx;
            int i = 0;
            for (; i < validSteps.Length; i++)
            {
                float tickSpacingPx = (validSteps[i]/valueRange) * pixelRange;
                levelStrengths[i] = (tickSpacingPx - minTickSpacingPx)/tickSpacing;

                if (levelStrengths[i] > 1.0f)
                    maxLevel = i;
                else if (levelStrengths[i] < 0.0f)
                    break;
            }

            if (i > 0)
                numLevels = i - maxLevel;
            else
                numLevels = 0;
        }

        private void SetTimeSteps()
        {
            validSteps = new float[]
            {
                3600.0f, 1800.0f, 600.0f, 300.0f,
                60.0f, 30.0f, 10.0f, 5.0f,
                1.0f, 0.5f, 0.25f, 0.1f, 0.05f, 0.01f
            };
        }

        private void SetGenericSteps()
        {
            float minStep = 0.0000001f;
            int numSteps = 15;

            validSteps = new float[15 * 2];
            for (int i = numSteps - 1; i >= 0; i--)
            {
                validSteps[i * 2 + 1] = minStep;
                validSteps[i * 2 + 0] = minStep * 5;

                minStep *= 10.0f;
            }
        }
    }

    /** @} */
}
