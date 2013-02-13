
//
// VS 8+ includes some bounds checking even in debug mode.  This include disable this and
// enables full optimization.
//
//#ifndef _DEBUG
#define _SECURE_SCL 0
#define _HAS_ITERATOR_DEBUGGING 0
//#endif

//
// Enables all standard C-runtime functions without warnings
//
#define _CRT_SECURE_NO_DEPRECATE

#define _WIN32_WINNT _WIN32_WINNT_WIN7
#define WINVER       _WIN32_WINNT_WIN7

//
// commenting out the following lines will disable code involving the corresponding API.
// This can be useful if you lack some of the libraries involved with either API.
// Software rendering is always supported.  For now, the D3D10 headers require
// D3D9 headers.
//
//#define USE_D3D9
//#define USE_D3D11
//#define USE_OPENGL
//#define USE_GLUT
//#define USE_DIRECTX_AUDIO

#if defined(USE_D3D9) | defined(USE_D3D11)
#define USE_D3D
#endif

//
// When defined, asserts are thrown on out-of-bounds vector access
//
#ifdef _DEBUG
#define VECTOR_DEBUG
#endif

//
// Enables asserts in release mode
//
//#define ALWAYS_ASSERT

//
// Control over DirectX debug modes
//
//#define D3D_DEBUG_INFO
//#define DEBUG_PS
//#define DEBUG_VS
//#define USE_REF

//
// Compression and PNG support
//
//#define USE_ZLIB
//#define USE_PNG

//
// Simple DirectMedia Layer support (used for images)
//
//#define USE_SDL

//
// Matrix library support
//
//#define USE_TNT

//#define USE_ANN
//#define USE_KDTREE

//
// Windows Media Foundation
//
//#define USE_WMF

//#define SUPPRESS_CONSOLE