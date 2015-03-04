namespace BansheeEngine
{
    public sealed class GpuProgram : Resource
    {
        // TODO - Dummy class
    }

    // Note: Must be equal to C++ enum GpuLanguage
    public enum GpuLanguage
    {
        HLSL,
        GLSL,
        Undefined
    }

    // Note: Must be equal to C++ enum GpuProgramType
    public enum GpuProgramType
    {
        Vertex,
        Fragment,
        Geometry,
        Domain,
        Hull,
        Compute
    };

    // Note: Must be equal to C++ enum GpuProgramProfile
    public enum GpuProgramProfile
    {
        None,
        Fragment_1_1,
        Fragment_1_2,
        Fragment_1_3,
        Fragment_1_4,
        Fragment_2_0,
        Fragment_2_x,
        Fragment_2_a,
        Fragment_2_b,
        Fragment_3_0,
        Fragment_3_x,
        Fragment_4_0,
        Fragment_4_1,
        Fragment_5_0,
        Vertex_1_1,
        Vertex_2_0,
        Vertex_2_x,
        Vertex_2_a,
        Vertex_3_0,
        Vertex_4_0,
        Vertex_4_1,
        Vertex_5_0,
        Geometry_4_0,
        Geometry_4_1,
        Geometry_5_0,
        Hull_5_0,
        Domain_5_0,
        Compute_5_0
    };
}
