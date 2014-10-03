using System;
using BansheeEngine;

namespace BansheeEditor
{
    [CustomHandle(typeof(Component))]
    public class DebugCameraHandle : IHandle
    {
        private Component target;
        private HandleSliderLine xAxis;

        public DebugCameraHandle(Component target)
        {
            this.target = target;

            xAxis = new HandleSliderLine(Vector3.xAxis, 5.0f);
        }

        protected override void Update()
        {
            xAxis.Position = target.sceneObject.position;
        }

        protected override void Response()
        {
            target.sceneObject.position = xAxis.NewPosition;
        }

        protected override void Draw()
        {
            Vector3 end = target.sceneObject.position + Vector3.xAxis * 5;

            if (xAxis.State == HandleSlider.StateType.Active)
                HandleDrawing.SetColor(Color.white);
            else
                HandleDrawing.SetColor(Color.green);
            
            HandleDrawing.DrawLine(target.sceneObject.position, end);
        }
    }
}
