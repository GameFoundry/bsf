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
            xAxis.Position = target.SceneObject.Position;
        }

        protected override void PostInput()
        {
            //target.sceneObject.Position = xAxis.NewPosition;
        }

        protected override void Draw()
        {
            Vector3 end = target.SceneObject.Position + Vector3.xAxis * 5;

            if (xAxis.State == HandleSlider.StateType.Active)
                HandleDrawing.SetColor(Color.White);
            else
                HandleDrawing.SetColor(Color.Green);

            HandleDrawing.DrawLine(target.SceneObject.Position, end);
        }
    }
}
