namespace BansheeEngine
{
    public struct Bounds
    {
        public Bounds(AABox box, Sphere sphere)
        {
            Box = box;
            Sphere = sphere;
        }

        public AABox Box;
        public Sphere Sphere;
    }
}
