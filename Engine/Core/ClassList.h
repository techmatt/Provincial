/*
ClassList.h
Written by Matthew Fisher

Forward declarations for classes/structures
*/

//Graphics objects
struct RGBColor;
class PointSet;
class KDTree3;

//Math objects
struct Vec3f;
struct Vec3f;
struct Vec4f;
struct Plane;
struct Line3D;

//Mesh objects
struct MeshVertex;
class BaseMesh;
class D3D9Mesh;
class D3D10Mesh;
class Mesh;

//D3D objects
class D3D9Font;

//Subdivision objects
class HalfEdge;
class FullEdge;
class Triangle;
class Vertex;
class ComplexMesh;

//Application/Windows objects
class InputManager;
class FrameTimer;
class ApplicationWindow;

//Graphics devices/API objects
class GraphicsDevice;
class OpenGLGraphicsDevice;
class D3D9GraphicsDevice;
class D3D10GraphicsDevice;
class SoftwareGraphicsDevice;

//Other objects
class String;
class UnicodeString;
//template <class type> class Vector;

//Texture objects
class D3D10Texture;
