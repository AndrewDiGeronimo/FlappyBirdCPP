# FlappyBirdCPP

### SOURCE.CPP is my original coding work. All other files were supplied as part of the resources necessary to build the game.

Contains all the files needed to run the program. DirectX Toolkit installation

DirectX Toolkit is required to run the game.
The game was compiled using DirectXTK_Desktop_2019_Win10.vcxproj.

To use DirectX components, program’s object file must be linked with the DirectX libraries. 
Add DirectX Tool Kit include path to Project/Properties/VC++ Directories.
Add d3d11.lib library to program by the following code
	#include <d3d11.h>
	#pragma comment (lib, "d3d11.lib");
   using namespace DirectX;


#### Features:
* Endless gameplay
* Highscore system
* Press start to play Gamestate
* Randomized bird colors
* Randomized pipe layout
* Bird Gravity
* Animated bird wings
