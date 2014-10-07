using BansheeEngine;

namespace BansheeEditor
{
    public abstract class DefaultHandle : Handle
    {
        protected Vector3 position;
        protected Quaternion rotation;

        public Vector3 Position
        {
            get { return position; }
            set { position = value; }
        }

        public Quaternion Rotation
        {
            get { return rotation; }
            set { rotation = value; }
        }

        internal void DoPreInput()
        {
            PreInput();
        }

        internal void DoPostInput()
        {
            PostInput();
        }

        internal void DoDraw()
        {
            Draw();
        }
    }
}
