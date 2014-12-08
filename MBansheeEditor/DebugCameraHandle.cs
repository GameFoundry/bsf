using System;
using BansheeEngine;

namespace BansheeEditor
{
    [CustomHandle(typeof(Component))]
    public class DebugCameraHandle : Handle
    {
        private Component target;
        private HandleSliderLine xAxis;

        public DebugCameraHandle(Component target)
        {
            this.target = target;

            xAxis = new HandleSliderLine(this, Vector3.xAxis, 5.0f);
        }

        protected override void PreInput()
        {
            xAxis.Position = target.sceneObject.Position;
        }

        protected override void PostInput()
        {
            //target.sceneObject.Position = xAxis.NewPosition;
        }

        protected override void Draw()
        {
            Vector3 end = target.sceneObject.Position + Vector3.xAxis * 5;

            if (xAxis.State == HandleSlider.StateType.Active)
                HandleDrawing.SetColor(Color.white);
            else
                HandleDrawing.SetColor(Color.green);

            HandleDrawing.DrawLine(target.sceneObject.Position, end);
        }
    }
}
