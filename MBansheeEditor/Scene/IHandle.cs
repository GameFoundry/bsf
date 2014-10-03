using BansheeEngine;

namespace BansheeEditor
{
    public abstract class IHandle
    {
        protected abstract void Update();
        protected abstract void Response();
        protected abstract void Draw();
    }
}
