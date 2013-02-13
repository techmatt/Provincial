/*
MeshVertex.h
Written by Matthew Fisher

MeshVertex Represents a single vertex in a mesh structure.
*/

struct MeshVertex
{
    MeshVertex() {}
    explicit MeshVertex(const Vec3f &_Pos)
    {
        Pos = _Pos;
        Normal = Vec3f::Origin;
        Color = RGBColor::White;
        TexCoord = Vec2f::Origin;
    }
    MeshVertex(const Vec3f &_Pos, const Vec3f &_Normal, RGBColor _Color, const Vec2f &_TexCoord)
    {
        Pos = _Pos;
        Normal = _Normal;
        Color = _Color;
        TexCoord = _TexCoord;
    }

    //
    // Static helper functions
    //
    static void Interpolate(const MeshVertex &sv, const MeshVertex &ev, MeshVertex &out, float s);

    //
    // The order of these elements matters for D3D
    //
    Vec3f Pos;           // position of the vertex in 3-space
    Vec3f Normal;        // vector normal to the surface passing through this vertex
    RGBColor Color;      // color of this vertex
    Vec2f TexCoord;      // x and y texture coordinates of this vertex
};
