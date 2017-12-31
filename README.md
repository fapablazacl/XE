
Low level C++ multimedia engine

Planned features for Version 1.0

Supported Platforms / Toolchains:
	- Windows / Visual C++ 2015
	- Windows / MinGW-w64
	- Linux / GCC
	- Linux / clang
	- Android / GCC
	- Web / emscripten

- Test-Driven development 
- Support for Windows, Linux, Web and Android

Features:

Low Level Layer 

Core
	- Buffer Abstraction
	
	- Stream Abstration
		- Memory Stream.
		- Memory Reference Stream.
		- Buffer Stream
		- File Stream.
		
	- Archive Abstraction
		- Folder Archive
		- Zipped Archive
		
Math 
	- n-dimensional Vector operations.
		- Addition and Subtraction.
		- Multiplication and Division by Scalar.
		- Multiplication and Division component wise.
		- Dot Product.
		- Cross Product.
		- Length.
		- Normalize
		- Clamp.
		- Triple Dot Product.
		- Triple Vector Product.
		- Projection
		
	- Quaternion operations.
		- Addition and Subtraction.
		- Multiplication and Division.
		- Multiplication and Division by Scalar.
		- Length.
		- Inverse. 
		- Normalize.
		- Conjugate.
		
	- nxm- dimensional Matrix operations.
		- Addition and Subtraction.
		- Multiplication and Division.
		- Multiplication and Division by Scalar.
		- Transpose
		- Determinant
		- Inverse
	
Geometry 
	- Plane 
	- Triangle 
	- Ray
	- Segment
	- Bounding Volumes
		- Sphere
		- Ellipsoid
		- Axis-Aligned Bouding Box
		
	- Operations
		 - Plane - Ray Intersection
		 - Sphere - Ray Intersection
		 - Ellipsoid - Ray Intersection
		 - AABBox - Ray Intersection
		 
		 - Plane - Segment Intersection
		 - Sphere - Segment Intersection
		 - Ellipsoid - Segment Intersection
		 - AABBox - Segment Intersection
		 
Input
	- Support for Mouse
		- Mouse Button Press and Release
		- Mouse Move
		- Mouse Bidirectional Wheel
	
	- Support for Keyboard
		- Key Press
		- Key Release
		
	- Suppport for Joypad 
		- Pad Press and Release
		- Joypad Button Press and Release
		- Joypad Handle Move.
	
	- Support for Touch (investigate more)
	
Graphics
	- Images
	- Textures
	- 
	- Modern Device Interface
		- Custom Vertex Attributes
		- Programmable Pipeline (Language depends on backend renderer)
		
	- Legacy Device Interface
		
Audio
	- Simple 2D Play
		- Volume Control
		- Pitch Control 
		
	- Listener Model 3D 
		- Volume Control
		- Pitch Control
		
	- Audio Buffers
	- Audio Buffer Queues
	- Extensible interface for loading Audio Samples 
	- Extensible inteface for sound effects

	
High Level Layer 

	
Scene Graph 
	- Scene - Based rendering 
	- Extensible Scene Node Object Interface
	
	