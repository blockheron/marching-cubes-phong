This source code was originally written for an assignment. It uses OpenGL and C++ to generating a scalar field with marching cubes, writes its vertices and normals into a PLY file, and then renders it with a Phong shader. Additionally, it also has a first-person camera oriented around spherical coordinates. To run it, ensure MarchingCubes.cpp is in the same folder as TriTable.hpp. Run the executable 'test' on the command line.

**Controls:** Use the up/down arrow to move towards or away from the origin. Click and drag with a mouse to rotate the rendered objects. There are 2 scalar fields: fWave and fCone. To change which renders in the program, go to line 515 and change the first argument of the method. You can also change the isovalue and stepsize there.

**Acknowledgements:** TriTable.hpp was supplied by my class. I also really wanted to name this repository what-the-phong, but sometimes, you must sacrifice bad jokes for berevity.

![fWave render](https://raw.githubusercontent.com/blockheron/marching-cubes-phong/refs/heads/main/fWave.png) ![fCone render](https://raw.githubusercontent.com/blockheron/marching-cubes-phong/refs/heads/main/fCone.png)
