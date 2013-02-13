/*
Engine.cpp
Written by Matthew Fisher

See Engine.h for a defintion of all files included here.
*/

//All source files include Main.h
#include "Main.h"

#include "Engine\Core\Stdhdr.cpp"
#include "Engine\Core\UnicodeString.cpp"
#include "Engine\Core\String.cpp"
#include "Engine\Core\OutputDataStream.cpp"
#include "Engine\Core\InputDataStream.cpp"
#include "Engine\Core\Compression.cpp"

#include "Engine\Networking\Pipe.cpp"

#include "Engine\Math\RGBColor.cpp"
#include "Engine\Math\SpaceVector.cpp"
#include "Engine\Math\Line3D.cpp"
#include "Engine\Math\LineSegment2D.cpp"
#include "Engine\Math\LineSegment3D.cpp"
#include "Engine\Math\Plane.cpp"
#include "Engine\Math\Intersect.cpp"
#include "Engine\Math\TriangleIntersection.cpp"
#include "Engine\Math\Distance.cpp"
#include "Engine\Math\Matrix4.cpp"
#include "Engine\Math\BipartiteMatcher.cpp"
#include "Engine\Math\BayesNet.cpp"
#include "Engine\Math\GraphicalModel.cpp"

#include "Engine\Multithreading\Thread.cpp"
#include "Engine\Multithreading\WorkerThread.cpp"
#include "Engine\Multithreading\ThreadPool.cpp"

#include "Engine\Graphics Objects\Bitmap.cpp"
#include "Engine\Graphics Objects\MatrixController.cpp"
#include "Engine\Graphics Objects\Camera.cpp"
#include "Engine\Graphics Objects\PrimitiveRender.cpp"
#include "Engine\Graphics Objects\KDTree3.cpp"
#include "Engine\Graphics Objects\KDTreeN.cpp"

#include "Engine\Utility\AudioCapture.cpp"
#include "Engine\Utility\VideoCompressor.cpp"
#include "Engine\Utility\FileCollection.cpp"
#include "Engine\Utility\ParameterFile.cpp"
#include "Engine\Utility\FrameTimer.cpp"
#include "Engine\Utility\Profiler.cpp"
#include "Engine\Utility\Directory.cpp"
#include "Engine\Utility\MenuInterface.cpp"
#include "Engine\Utility\Console.cpp"
#include "Engine\Utility\ColorGenerator.cpp"
#include "Engine\Utility\Stemmer.cpp"
#include "Engine\Utility\Palette.cpp"

#include "Engine\Windows Controller\ApplicationWindow.cpp"
#include "Engine\Windows Controller\InputManager.cpp"
#include "Engine\Windows Controller\EventHandler.cpp"