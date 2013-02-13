/*
Camera.h
Written by Matthew Fisher

Camera represents a 3-D free floating camera.  The simplest 3D description of a camera
is an eye location and 3 perpindicular vectors (the look vector, the up vector, and the
cross product of these two, which would be the vector pointing to the left of the viewer.)
Although this could use quaternions, it is easier to keep everything as the 4 vectors
(eye, look, up, left) described above.
*/

class Camera
{
public:
    Camera();
    Camera(const String &s);

    void Reset(const Vec3f &eye, const Vec3f &worldUp, const Vec3f &lookAt);
    String ToString();
    
    void Update();
    
    //
    // Input
    //
    void KeyboardWASD(InputManager &input, float factor);
    void KeyboardNumpad(InputManager &input, float factor);

    //
    // Movement
    //
    void LookUp(float Theta);
    void LookRight(float Theta);
    void Roll(float Theta);
    void Move(float Distance);
    void Jump(float Distance);
    void Strafe(float Distance);

    //
    // Accessors
    //
    __forceinline Plane ViewPlane()
    {
        return Plane::ConstructFromPointNormal(_eye, _look);
    }

    __forceinline Matrix4& Matrix()
    {
        return _matrix;
    }
    __forceinline const Matrix4& Matrix() const
    {
        return _matrix;
    }
    __forceinline Vec3f& VecEye()
    {
        return _eye;
    }
    __forceinline const Vec3f& VecEye() const
    {
        return _eye;
    }
    __forceinline Vec3f& VecLookDir()
    {
        return _look;
    }
    __forceinline const Vec3f& VecLookDir() const
    {
        return _look;
    }
    __forceinline Vec3f& VecUp()
    {
        return _up;
    }
    __forceinline const Vec3f& VecUp() const
    {
        return _up;
    }
    __forceinline Vec3f& VecRight()
    {
        return _right;
    }
    __forceinline const Vec3f& VecRight() const
    {
        return _right;
    }
    __forceinline Vec3f& AbsoluteUp()
    {
        return _worldUp;
    }
    __forceinline const Vec3f& AbsoluteUp() const
    {
        return _worldUp;
    }

private:
    Vec3f _eye, _look, _up, _right, _worldUp;
    Matrix4 _matrix;
};
