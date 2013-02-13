/*
Indicator.h
Written by Matthew Fisher

the Indicator class rapidly renders spheres or cylinders between arbitrary points in three space
by using stored mesh data and loading a new Matrix4 each time a cylinder or sphere needs "indicating."
also can do basic viewing frustrum (camera) analysis or rendering.
*/

enum IndicatorShapeType
{
    IndicatorShapeSphere,
    IndicatorShapeCylinder,
};

struct IndicatorShape
{
    IndicatorShape() {}
    IndicatorShape(IndicatorShapeType _Type, const Vec3f &Pos0, const Vec3f &Pos1, float _Radius, RGBColor _Color)
    {
        Type = _Type;
        Pos[0] = Pos0;
        Pos[1] = Pos1;
        Color = _Color;
        Radius = _Radius;
    }

    static IndicatorShape Sphere(const Vec3f &Pos, float Radius, RGBColor Color);
    static IndicatorShape Cylinder(const Vec3f &P0, const Vec3f &P1, float Radius, RGBColor Color);

    Matrix4 TransformMatrix() const;

    IndicatorShapeType Type;
    Vec3f Pos[2];
    RGBColor Color;
    float Radius;
};

class Indicator
{
public:
    void Init(GraphicsDevice &GD, int SphereRefinement, int Stacks, int Slices);    //creates a new sphere with the given number of refinements,
                                                                                    //and a new cylinder with the given number of stacks/slices

    void RenderCylinder(GraphicsDevice &GD, MatrixController &MC, float Radius, const Vec3f &P1, const Vec3f &P2,
                        const RGBColor &Color, bool RenderArrow = false, bool ColorNormals = false);

    void RenderBox(GraphicsDevice &GD, MatrixController &MC, float Radius, const Rectangle3f &Rect, RGBColor Color);

    void RenderArrow(GraphicsDevice &GD, MatrixController &MC, const Vec3f &P1, const Vec3f &P2, const RGBColor &Color, bool ColorNormals = false);

    void RenderSphere(GraphicsDevice &GD, MatrixController &MC,            //renders the existing sphere in the given color such that its center is
                float Radius, const Vec3f &P1, const RGBColor &Color);    //P1 and it has the given radius.

    void GetCameraLine(const Matrix4 &Projection, const Camera &C,            //given x and y both on the perspective cube (the interval -1 to 1), returns the same line
                float x, float y, Vec3f &P1Out, Vec3f &P2Out);                //segment in world coordinates given the provided perspective Matrix4 and camera's view Matrix4.

    void DrawCameraLine(GraphicsDevice &GD, MatrixController &MC, const Matrix4 &Projection,    //calls GetCameraLine and then renders the given line as a cylinder
                    float Radius, const RGBColor &Color, const Camera &C, float x, float y);    //having the given radius and color

    void DrawCamera(GraphicsDevice &GD, MatrixController &MC,                //renders the entire 8 lines of the perspective cube (x, y) from (-1 to 1) and z from (0 to 1)
        const Matrix4 &Perspective, float Radius, float Length, const Camera &C);    //as cylinders of the given radius.  Also renders the camera's eye vector as a sphere,
                                                                            //and the 3 orthogonal vectors (VecEye, VecLookDir, VecLeft) that represent that camera's basis
                                                                            //as cylinders of the given Length centered on VecEye.

    void CreateMesh(const Vector<IndicatorShape> &Shapes, BaseMesh &MOut) const;

private:
    RGBColor _CylinderColor;
    Mesh _Cylinder, _ArrowHead, _Sphere, _Lozenge, _Box;
};
