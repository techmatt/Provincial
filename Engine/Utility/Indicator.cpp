/*
Indicator.cpp
Written by Matthew Fisher

the Indicator class rapidly renders spheres or cylinders between arbitrary points in three space
by using stored mesh data and loading a new Matrix4 each time a cylinder or sphere needs "indicating."
also can do basic viewing frustrum (camera) analysis or rendering.
*/

IndicatorShape IndicatorShape::Sphere(const Vec3f &Pos, float Radius, RGBColor Color)
{
    return IndicatorShape(IndicatorShapeSphere, Pos, Vec3f::Origin, Radius, Color);
}

IndicatorShape IndicatorShape::Cylinder(const Vec3f &P0, const Vec3f &P1, float Radius, RGBColor Color)
{
    return IndicatorShape(IndicatorShapeCylinder, P0, P1, Radius, Color);
}

Matrix4 IndicatorShape::TransformMatrix() const
{
    switch(Type)
    {
    case IndicatorShapeSphere:
        {
            Matrix4 Scale = Matrix4::Scaling(Radius);
            Matrix4 Translate = Matrix4::Translation(Pos[0]);
            return Scale * Translate;
        }
    case IndicatorShapeCylinder:
        {
            Vec3f Diff = Pos[1] - Pos[0];
            float Height = Diff.Length();

            Matrix4 Scale = Matrix4::Scaling(Vec3f(Radius, Radius, Height));
            Matrix4 Face = Matrix4::Face(Vec3f::eZ, Diff);
            Matrix4 Translate = Matrix4::Translation(Pos[0]);

            return Scale * Face * Translate;
        }
    default:
        SignalError("Invalid shape type");
        return Matrix4::Identity();
    }
}

void Indicator::Init(GraphicsDevice &GD, int SphereRefinement, int Stacks, int Slices)
{
    _Sphere.SetGD(GD);
    _Cylinder.SetGD(GD);
    _ArrowHead.SetGD(GD);
    _Box.SetGD(GD);

    if(SphereRefinement == -1)
    {
        _Sphere.CreateBox(2.0f, 2.0f, 2.0f);
    }
    else
    {
        _Sphere.CreateSphere(1.0f, SphereRefinement);
    }

    _Cylinder.CreateCylinder(1.0f, 1.0f, Slices, Stacks);        //create the sphere/cylinder as requested
    _ArrowHead.CreateCylinder(1.0f, 1.0f, Slices, Stacks);
    for(UINT VertexIndex = 0; VertexIndex < _ArrowHead.VertexCount(); VertexIndex++)
    {
        Vec3f &Pos = _ArrowHead.Vertices()[VertexIndex].Pos;
        float Radius = 1.0f - Pos.z;
        Pos.x *= Radius;
        Pos.y *= Radius;
    }

    _CylinderColor = RGBColor::White;
    _Box.CreateBox(1.0f, 1.0f, 1.0f);
    _Box.ApplyMatrix(Matrix4::Translation(Vec3f(0.5f, 0.5f, 0.5f)));
    //Lozenge.CreateLozenge(1.0f, Origin, eZ, Slices, Stacks);    //create the sphere/cylinder as requested
}

void Indicator::RenderArrow(GraphicsDevice &GD, MatrixController &MC, const Vec3f &P1, const Vec3f &P2, const RGBColor &Color, bool ColorNormals)
{
    Vec3f Diff = P2 - P1;
    Vec3f Dir = Vec3f::Normalize(Diff);
    float TotalHeight = Diff.Length();
    float CylinderRadius = TotalHeight * 0.1f;
    float ArrowRadius = CylinderRadius * 2.0f;
    float ArrowHeight = ArrowRadius;
    float CylinderHeight = TotalHeight - ArrowHeight;

    RenderCylinder(GD, MC, CylinderRadius, P1, P1 + Dir * CylinderHeight, Color, false, ColorNormals);
    RenderCylinder(GD, MC, ArrowRadius, P1 + Dir * CylinderHeight, P2, Color, true,  ColorNormals);
}

void Indicator::RenderBox(GraphicsDevice &GD, MatrixController &MC, float Radius, const Rectangle3f &Rect, RGBColor Color)
{
    RenderCylinder(GD, MC, Radius, Vec3f(Rect.Min.x, Rect.Min.y, Rect.Min.z), Vec3f(Rect.Max.x, Rect.Min.y, Rect.Min.z), Color);
    RenderCylinder(GD, MC, Radius, Vec3f(Rect.Max.x, Rect.Min.y, Rect.Min.z), Vec3f(Rect.Max.x, Rect.Max.y, Rect.Min.z), Color);
    RenderCylinder(GD, MC, Radius, Vec3f(Rect.Max.x, Rect.Max.y, Rect.Min.z), Vec3f(Rect.Min.x, Rect.Max.y, Rect.Min.z), Color);
    RenderCylinder(GD, MC, Radius, Vec3f(Rect.Min.x, Rect.Max.y, Rect.Min.z), Vec3f(Rect.Min.x, Rect.Min.y, Rect.Min.z), Color);

    RenderCylinder(GD, MC, Radius, Vec3f(Rect.Min.x, Rect.Min.y, Rect.Min.z), Vec3f(Rect.Min.x, Rect.Min.y, Rect.Max.z), Color);
    RenderCylinder(GD, MC, Radius, Vec3f(Rect.Max.x, Rect.Min.y, Rect.Min.z), Vec3f(Rect.Max.x, Rect.Min.y, Rect.Max.z), Color);
    RenderCylinder(GD, MC, Radius, Vec3f(Rect.Max.x, Rect.Max.y, Rect.Min.z), Vec3f(Rect.Max.x, Rect.Max.y, Rect.Max.z), Color);
    RenderCylinder(GD, MC, Radius, Vec3f(Rect.Min.x, Rect.Max.y, Rect.Min.z), Vec3f(Rect.Min.x, Rect.Max.y, Rect.Max.z), Color);

    RenderCylinder(GD, MC, Radius, Vec3f(Rect.Min.x, Rect.Min.y, Rect.Max.z), Vec3f(Rect.Max.x, Rect.Min.y, Rect.Max.z), Color);
    RenderCylinder(GD, MC, Radius, Vec3f(Rect.Max.x, Rect.Min.y, Rect.Max.z), Vec3f(Rect.Max.x, Rect.Max.y, Rect.Max.z), Color);
    RenderCylinder(GD, MC, Radius, Vec3f(Rect.Max.x, Rect.Max.y, Rect.Max.z), Vec3f(Rect.Min.x, Rect.Max.y, Rect.Max.z), Color);
    RenderCylinder(GD, MC, Radius, Vec3f(Rect.Min.x, Rect.Max.y, Rect.Max.z), Vec3f(Rect.Min.x, Rect.Min.y, Rect.Max.z), Color);

    Matrix4 Scale = Matrix4::Scaling(Rect.Dimensions());
    Matrix4 Translate = Matrix4::Translation(Rect.Min);

    MC.World = Scale * Translate;

    LPDIRECT3DDEVICE9 Device = GD.CastD3D9().GetDevice();
    
    D3DCOLOR D3DColor = RGBColor(90, 90, 90);
    Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_BLENDFACTOR);
    Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVBLENDFACTOR);
    Device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
    Device->SetRenderState(D3DRS_BLENDFACTOR, D3DColor);

    _Box.SetColor(Color);
    _Box.Render();

    Device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
}

void Indicator::RenderCylinder(GraphicsDevice &GD, MatrixController &MC, float Radius, const Vec3f &P1, const Vec3f &P2, const RGBColor &Color, bool RenderArrow, bool ColorNormals)
{
    Vec3f Diff = P2 - P1;
    float Height = Diff.Length();

    Matrix4 Scale = Matrix4::Scaling(Vec3f(Radius, Radius, Height)); //radius and height
    Matrix4 Face = Matrix4::Face(Vec3f::eZ, Diff);                   //direction
    Matrix4 Translate = Matrix4::Translation(P1);                    //position

    MC.World = Scale * Face * Translate;
	
    if(RenderArrow)
    {
        _ArrowHead.SetColor(Color);
        if(ColorNormals)
        {
            _ArrowHead.ColorNormalsGrayScale(Color);
        }
        _ArrowHead.Render();
    }
    else
    {
        if(Color != _CylinderColor)
        {
            _Cylinder.SetColor(Color);
            _CylinderColor = Color;
        }
        if(ColorNormals)
        {
            _Cylinder.ColorNormalsGrayScale(Color);
        }
        _Cylinder.Render();
    }
}

/*void Indicator::RenderLozenge(GraphicsDevice &GD, MatrixController &MC, float Radius, Vec3f &P1, Vec3f &P2, const RGBColor &Color)
{
    Matrix4 Scale, Face, Translate;    //scale fixes the cylinder radius and height, Face fixes the cylinder direction, Translate fixes the cylinder position
    Vec3f Diff = P2 - P1;
    float Height = Diff.Length();
    Diff.Normalize();

    Scale.Scaling(Vec3f(Radius,Radius,Height));    //radius and height
    Face.Face(eZ, Diff);                        //direction
    Translate.Translation(P1);                    //position

    MC.World = Scale * Face * Translate;

    GD.LoadMatrix(MC);                            //tell the graphics device to use this Matrix4,

    Lozenge.SetColor(Color);    //set the cylinder color
    Lozenge.Render();            //render the cylinder with the given Matrix4
}*/

void Indicator::RenderSphere(GraphicsDevice &GD, MatrixController &MC, float Radius, const Vec3f &P1, const RGBColor &Color)
{
    Matrix4 Scale = Matrix4::Scaling(Vec3f(Radius,Radius,Radius));
    Matrix4 Translate = Matrix4::Translation(P1);

    MC.World = Scale * Translate;

    _Sphere.SetColor(Color);
    _Sphere.Render();
}

void Indicator::GetCameraLine(const Matrix4 &Perspective, const Camera &C, float x, float y, Vec3f &P1Out, Vec3f &P2Out)
{
    Matrix4 Total = (C.Matrix() * Perspective).Inverse();    //the perspective -> view transform
    Vec3f P1(x * 2.0f - 1.0f, y * 2.0f - 1.0f, 0.0f), P2;
    P2 = Vec3f(P1.x, P1.y, 1.0f);                    //P1 and P2 are the segment corresponding to (x, y) on the perpsective cube

    P1Out = Total.TransformPoint(P1);
    P2Out = Total.TransformPoint(P2);
}

void Indicator::DrawCameraLine(GraphicsDevice &GD, MatrixController &MC, const Matrix4 &Perspective, float Radius, const RGBColor &Color, const Camera &C, float x, float y)
{
    Vec3f P1Out, P2Out;
    GetCameraLine(Perspective, C, x, y, P1Out, P2Out);        //get the camera line
    RenderCylinder(GD, MC, Radius, P1Out, P2Out, Color);    //draw it
}

void Indicator::DrawCamera(GraphicsDevice &GD, MatrixController &MC, const Matrix4 &Perspective, float Radius, float Length, const Camera &C)
{
    Vec3f Frustrum[8], Target;
    GetCameraLine(Perspective, C, 0.0f, 0.0f, Frustrum[0], Frustrum[4]);
    GetCameraLine(Perspective, C, 1.0f, 0.0f, Frustrum[1], Frustrum[5]);
    GetCameraLine(Perspective, C, 1.0f, 1.0f, Frustrum[2], Frustrum[6]);
    GetCameraLine(Perspective, C, 0.0f, 1.0f, Frustrum[3], Frustrum[7]);    //get the 8 vertices of the perspective cube in world space

    RenderCylinder(GD, MC, Radius, Frustrum[0], Frustrum[1], RGBColor::White);
    RenderCylinder(GD, MC, Radius, Frustrum[1], Frustrum[2], RGBColor::White);
    RenderCylinder(GD, MC, Radius, Frustrum[2], Frustrum[3], RGBColor::White);
    RenderCylinder(GD, MC, Radius, Frustrum[3], Frustrum[0], RGBColor::White);

    RenderCylinder(GD, MC, Radius, Frustrum[0], Frustrum[4], RGBColor::White);
    RenderCylinder(GD, MC, Radius, Frustrum[1], Frustrum[5], RGBColor::White);
    RenderCylinder(GD, MC, Radius, Frustrum[2], Frustrum[6], RGBColor::White);
    RenderCylinder(GD, MC, Radius, Frustrum[3], Frustrum[7], RGBColor::White);

    RenderCylinder(GD, MC, Radius, Frustrum[4], Frustrum[5], RGBColor::White);
    RenderCylinder(GD, MC, Radius, Frustrum[5], Frustrum[6], RGBColor::White);
    RenderCylinder(GD, MC, Radius, Frustrum[6], Frustrum[7], RGBColor::White);
    RenderCylinder(GD, MC, Radius, Frustrum[7], Frustrum[4], RGBColor::White);    //render the 12 line segments of the perspective cube in world space

    RenderSphere(GD, MC, Radius * 2.0f, C.VecEye(), RGBColor::Blue);            //render the camera eye vector

    Target = C.VecEye() - C.VecLookDir() * Length;
    RenderCylinder(GD, MC, Radius, C.VecEye(), Target, RGBColor::Red);

    Target = C.VecEye() + C.VecUp() * Length;
    RenderCylinder(GD, MC, Radius, C.VecEye(), Target, RGBColor::Green);

    Target = C.VecEye() + C.VecRight() * Length;
    RenderCylinder(GD, MC, Radius, C.VecEye(), Target, RGBColor::Yellow);        //render the C.VecLookDir, C.VecUp, and C.VecLeft vectors
}

void Indicator::CreateMesh(const Vector<IndicatorShape> &Shapes, BaseMesh &MOut) const
{
    Vector<const BaseMesh*> AllMeshes(Shapes.Length());
    for(UINT ShapeIndex = 0; ShapeIndex < Shapes.Length(); ShapeIndex++)
    {
        const IndicatorShape &CurShape = Shapes[ShapeIndex];
        Mesh *NewMesh = NULL;
        switch(CurShape.Type)
        {
        case IndicatorShapeSphere:
            {
                NewMesh = new Mesh(_Sphere);
            }
            break;
        case IndicatorShapeCylinder:
            {
                NewMesh = new Mesh(_Cylinder);
            }
            break;
        default:
            SignalError("Invalid shape type");
            return;
        }
        NewMesh->ApplyMatrix(CurShape.TransformMatrix());
        NewMesh->SetColor(CurShape.Color);
        AllMeshes[ShapeIndex] = NewMesh;
    }

    MOut.SetGD(_Sphere.GetGD());
    MOut.LoadMeshList(AllMeshes);

    for(UINT ShapeIndex = 0; ShapeIndex < Shapes.Length(); ShapeIndex++)
    {
        delete AllMeshes[ShapeIndex];
    }
}
