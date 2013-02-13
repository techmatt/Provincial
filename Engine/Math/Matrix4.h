/*
Matrix4.h
Written by Matthew Fisher

a 4x4 transformation matrix structure.
*/

class Matrix4
{
public:
    //
    // Initalization
    //
    Matrix4();
    Matrix4(const Matrix4 &M);
    Matrix4(const Vec3f &V0, const Vec3f &V1, const Vec3f &V2);

#ifdef USE_D3D
    Matrix4(const D3DXMATRIX &M);
#endif

    //
    // Assignment
    //
    Matrix4& operator = (const Matrix4 &M);

    //
    // Math properties
    //
    float Determinant() const;
    Matrix4 Transpose() const;
    Matrix4 Inverse() const;
	__forceinline float RotationAmount() const
	{
		// Valid only for rotation matrices
        float trace = _Entries[0][0] + _Entries[1][1] + _Entries[2][2];
		return acos(Utility::Bound((trace - 1.0f) * 0.5f, -1.0f, 1.0f));
	}

    //
    // Vector transforms
    //
    __forceinline Vec3f TransformPoint(const Vec3f &point) const
    {
        float w = point.x * _Entries[0][3] + point.y * _Entries[1][3] + point.z * _Entries[2][3] + _Entries[3][3];
        if(w)
        {
            const float invW = 1.0f / w;
            return Vec3f( (point.x * _Entries[0][0] + point.y * _Entries[1][0] + point.z * _Entries[2][0] + _Entries[3][0]) * invW,
                          (point.x * _Entries[0][1] + point.y * _Entries[1][1] + point.z * _Entries[2][1] + _Entries[3][1]) * invW,
                          (point.x * _Entries[0][2] + point.y * _Entries[1][2] + point.z * _Entries[2][2] + _Entries[3][2]) * invW);
        }
        else
        {
            return Vec3f::Origin;
        }
    }

    __forceinline Vec3f TransformPointNoProjection(const Vec3f &point) const
    {
        return Vec3f( (point.x * _Entries[0][0] + point.y * _Entries[1][0] + point.z * _Entries[2][0] + _Entries[3][0]),
                      (point.x * _Entries[0][1] + point.y * _Entries[1][1] + point.z * _Entries[2][1] + _Entries[3][1]),
                      (point.x * _Entries[0][2] + point.y * _Entries[1][2] + point.z * _Entries[2][2] + _Entries[3][2]));
    }
    
    __forceinline Vec3f TransformNormal(const Vec3f &normal) const
    {
        return Vec3f(normal.x * _Entries[0][0] + normal.y * _Entries[1][0] + normal.z * _Entries[2][0],
                     normal.x * _Entries[0][1] + normal.y * _Entries[1][1] + normal.z * _Entries[2][1],
                     normal.x * _Entries[0][2] + normal.y * _Entries[1][2] + normal.z * _Entries[2][2]);
    }

    Plane TransformPlane(const Plane &P) const;
    
    //
    // Formatting
    //
    String CommaSeparatedString() const;
    String CommaSeparatedStringSingleLine() const;
    String SpaceSeparatedStringSingleLine() const;
    String TabSeperatedString() const;

#ifdef USE_D3D
    operator D3DXMATRIX() const;
#endif
    
    //
    // Accessors
    //
    __forceinline float* operator [] (int Row)
    {
        return _Entries[Row];
    }
    __forceinline const float* operator [] (int Row) const
    {
        return _Entries[Row];
    }
    __forceinline void SetColumn(UINT Column, const Vec4f &Values)
    {
        _Entries[0][Column] = Values.x;
        _Entries[1][Column] = Values.y;
        _Entries[2][Column] = Values.z;
        _Entries[3][Column] = Values.w;
    }
    __forceinline void SetRow(UINT Row, const Vec4f &Values)
    {
        _Entries[Row][0] = Values.x;
        _Entries[Row][1] = Values.y;
        _Entries[Row][2] = Values.z;
        _Entries[Row][3] = Values.w;
    }
    __forceinline Vec4f GetColumn(UINT Column)
    {
        Vec4f Result;
        Result.x = _Entries[0][Column];
        Result.y = _Entries[1][Column];
        Result.z = _Entries[2][Column];
        Result.w = _Entries[3][Column];
        return Result;
    }
    __forceinline Vec4f GetRow(UINT Row)
    {
        Vec4f Result;
        Result.x = _Entries[Row][0];
        Result.y = _Entries[Row][1];
        Result.z = _Entries[Row][2];
        Result.w = _Entries[Row][3];
        return Result;
    }

    //
    // Transformation matrices
    //
    static Matrix4 Identity();
    static Matrix4 Scaling(const Vec3f &ScaleFactors);
    static Matrix4 Scaling(float ScaleFactor)
    {
        return Scaling(Vec3f(ScaleFactor, ScaleFactor, ScaleFactor));
    }
    static Matrix4 Translation(const Vec3f &Pos);
    static Matrix4 Rotation(const Vec3f &Axis, float Angle, const Vec3f &Center);
    static Matrix4 Rotation(const Vec3f &Axis, float Angle);
    static Matrix4 Rotation(float Yaw, float Pitch, float Roll);
    static Matrix4 Rotation(const Vec3f &Basis1, const Vec3f &Basis2, const Vec3f &Basis3);
    static Matrix4 RotationX(float Theta);
    static Matrix4 RotationY(float Theta);
    static Matrix4 RotationZ(float Theta);
    static Matrix4 Camera(const Vec3f &Eye, const Vec3f &Look, const Vec3f &Up, const Vec3f &Right);
    static Matrix4 LookAt(const Vec3f &Eye, const Vec3f &At, const Vec3f &Up);
    static Matrix4 Orthogonal(float Width, float Height, float ZNear, float ZFar);
    static Matrix4 Perspective(float Width, float Height, float ZNear, float ZFar);
    static Matrix4 PerspectiveFov(float FOV, float Aspect, float ZNear, float ZFar);
    static Matrix4 PerspectiveMultiFov(float FovX, float FovY, float ZNear, float ZFar);
    static Matrix4 Face(const Vec3f &V0, const Vec3f &V1);
    static Matrix4 Viewport(float Width, float Height);
    static Matrix4 ChangeOfBasis(const Vec3f &Source0, const Vec3f &Source1, const Vec3f &Source2, const Vec3f &SourceOrigin, 
                                 const Vec3f &Target0, const Vec3f &Target1, const Vec3f &Target2, const Vec3f &TargetOrigin);
    static float CompareMatrices(const Matrix4 &Left, const Matrix4 &Right);

    friend Matrix4 operator * (const Matrix4 &Left, const Matrix4 &Right);
    friend Matrix4 operator * (const Matrix4 &Left, float &Right);
    friend Matrix4 operator * (float &Left, const Matrix4 &Right);
    friend Matrix4 operator + (const Matrix4 &Left, const Matrix4 &Right);
    friend Matrix4 operator - (const Matrix4 &Left, const Matrix4 &Right);

private:
    float _Entries[4][4];
};

//
// Overloaded operators
//
ostream& operator << (ostream &os, const Matrix4 &m);
istream& operator >> (istream &os, Matrix4 &m);

__forceinline Vec4f operator * (const Vec4f &Right, const Matrix4 &Left)
{
    return Vec4f(Right.x * Left[0][0] + Right.y * Left[1][0] + Right.z * Left[2][0] + Right.w * Left[3][0],
                Right.x * Left[0][1] + Right.y * Left[1][1] + Right.z * Left[2][1] + Right.w * Left[3][1],
                Right.x * Left[0][2] + Right.y * Left[1][2] + Right.z * Left[2][2] + Right.w * Left[3][2],
                Right.x * Left[0][3] + Right.y * Left[1][3] + Right.z * Left[2][3] + Right.w * Left[3][3]);
}
