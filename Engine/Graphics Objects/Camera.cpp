/*
Camera.cpp
Written by Matthew Fisher

Camera represents a 3-D free floating camera.  The simplest 3D description of a camera
is an eye location and 3 perpindicular vectors (the look vector, the up vector, and the
cross product of these two, which would be the vector pointing to the left of the viewer.)
Although this could use quaternions, it is easier to keep everything as the 4 vectors
(eye, look, up, left) described above.  See Camera.h for a description of each function.
*/

Camera::Camera()
{
    _eye = Vec3f(0.0f,0.0f,0.0f);
    _up = Vec3f(0.0f,1.0f,0.0f);
    _right = Vec3f(1.0f,0.0f,0.0f);
    _look = Vec3f(0.0f,0.0f,1.0f);
    _worldUp = Vec3f(0.0f,1.0f,0.0f);
    Update();
}

Camera::Camera(const String &s)
{
    Vector<String> v = s.Partition(',');
    PersistentAssert(v.Length() == 15, "Incorrect number of components");
    _eye     = Vec3f(v[0 ].ConvertToFloat(), v[1 ].ConvertToFloat(), v[2 ].ConvertToFloat());
    _look    = Vec3f(v[3 ].ConvertToFloat(), v[4 ].ConvertToFloat(), v[5 ].ConvertToFloat());
    _up      = Vec3f(v[6 ].ConvertToFloat(), v[7 ].ConvertToFloat(), v[8 ].ConvertToFloat());
    _right   = Vec3f(v[9 ].ConvertToFloat(), v[10].ConvertToFloat(), v[11].ConvertToFloat());
    _worldUp = Vec3f(v[12].ConvertToFloat(), v[13].ConvertToFloat(), v[14].ConvertToFloat());
}

String Camera::ToString()
{
    return _eye.ToString(',') + "," +
           _look.ToString(',') + "," +
           _up.ToString(',') + "," +
           _right.ToString(',') + "," +
           _worldUp.ToString(',');
}

void Camera::Reset(const Vec3f &Eye, const Vec3f &Up, const Vec3f &VecLookAt)
{
    _eye = Eye;
    _worldUp = Up;
    _look = Vec3f::Normalize(_eye - VecLookAt);
    _right = Vec3f::Normalize(Vec3f::Cross(_worldUp, _look));
    _up = Vec3f::Normalize(Vec3f::Cross(_look, _right));

    Update();
}

void Camera::Update()
{
    _matrix = Matrix4::Camera(_eye, _look, _up, -_right);
}

void Camera::KeyboardWASD(InputManager &input, float factor)
{
    //Unreal Tournament style keyboard setup
    if(input.KeyCheck(KEY_W)) Move(-factor);
    if(input.KeyCheck(KEY_S)) Move(factor);

    if(input.KeyCheck(KEY_A)) Strafe(factor);
    if(input.KeyCheck(KEY_D)) Strafe(-factor);
}

void Camera::KeyboardNumpad(InputManager &input, float factor)
{
    //Use the numberpad to replace the mouse
    if(input.KeyCheck(KEY_NUMPAD4)) LookRight(-factor);
    if(input.KeyCheck(KEY_NUMPAD6)) LookRight(factor);

    if(input.KeyCheck(KEY_NUMPAD8)) LookUp(factor);
    if(input.KeyCheck(KEY_NUMPAD2)) LookUp(-factor);

    if(input.KeyCheck(KEY_NUMPAD7)) Roll(factor);
    if(input.KeyCheck(KEY_NUMPAD9)) Roll(-factor);
}

void Camera::LookUp(float theta)
{
    Matrix4 Rotation = Matrix4::Rotation(_right, theta);
    _up = Rotation.TransformPoint(_up);
    _right = Rotation.TransformPoint(_right);
    _look = Rotation.TransformPoint(_look);
}

void Camera::LookRight(float theta)
{
    Matrix4 Rotation = Matrix4::Rotation(_worldUp, theta);
    _up = Rotation.TransformPoint(_up);
    _right = Rotation.TransformPoint(_right);
    _look = Rotation.TransformPoint(_look);
}

void Camera::Roll(float theta)
{
    Matrix4 Rotation = Matrix4::Rotation(_look, theta);
    _up = Rotation.TransformPoint(_up);
    _right = Rotation.TransformPoint(_right);
    _look = Rotation.TransformPoint(_look);
}

void Camera::Move(float distance)
{
    _eye += _look * distance;
}

void Camera::Strafe(float distance)
{
    _eye += _right * distance;
}
