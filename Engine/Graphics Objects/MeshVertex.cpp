/*
MeshVertex.cpp
Written by Matthew Fisher

MeshVertex Represents a single vertex in a mesh structure.
*/

void Interpolate(const MeshVertex &sv, const MeshVertex &ev, MeshVertex &out, float s)
{
    int i;
    unsigned char *color1,*color2,*color;
    color = (unsigned char *) &out.Color;
    color1 = (unsigned char *) &sv.Color;
    color2 = (unsigned char *) &ev.Color;
    for(i=0;i<3;i++)
        color[i] = unsigned char(color1[i] + int(s * (color2[i] - color1[i])));    //interpolate each component of the color seperately

    out.TexCoord.x = sv.TexCoord.x + s * (ev.TexCoord.x - sv.TexCoord.x);
    out.TexCoord.y = sv.TexCoord.y + s * (ev.TexCoord.y - sv.TexCoord.y);    //interpolate the texture coordinates

    out.Normal = sv.Normal + s * (ev.Normal - sv.Normal);    //interpolate the normal vector
    float Len = out.Normal.Length();
    if(Len == 0.0f)
    {
        out.Normal = sv.Normal;                                    //renormalize the normal vector
    }
    else
    {
        out.Normal /= Len;
    }

    out.Pos = sv.Pos + s * (ev.Pos - sv.Pos);    //interpolate the position
}
