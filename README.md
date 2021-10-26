# Cable winding simulation
Implementation for cable winding simulation using our framework COND. 

## Prerequisite
Tested environment: Windows 10 64bit 19041.1165, Visual Studio 15, OpenGL 3.3.
1. Install Eigen (3.3.7): https://eigen.tuxfamily.org/index.php?title=Main_Page 
2. Install glew (2.1.0): http://glew.sourceforge.net/index.html
3. Install glm (0.9.9): https://github.com/g-truc/glm
4. Install glfw (3.3): https://www.glfw.org/download
\
Please download above prerequisites and add the header files and lib files in `CMakelists.txt` and `include/precompiled.h` properly.

## Execution
The source can be compiled using MSVC compiler.
```
git clone https://github.com/INRoL/inrol_sim_cablewinding.git
cd inrol_sim_cablewinding/build
cmake .. -G "Visual Studio 15 Win64" -DCMAKE_BUILD_TYPE=Release
inrol_sim_cablewinding.sln
```
Build and execute inrol_sim_cablewinding project.

## Set environment
The environment can be adjusted by changing variables in `src/main.cpp`.
```
int N_link = 160; // number of cable segment
double r = 0.002; // cable radius
double L = 0.4 / N_link; // length of each cable segment 
double rho = 1259; // cable density
double E = 2.954*1e6; // cable Young modulus
double nu = 0.49; // cable Poisson's ratio
double mu = 1.3; // friction coefficient
double g = 9.81; // gravitational acceleration
double cyl_radius = 0.02; // cylinder radius
double cyl_center_x = 0.4920; // cylinder position (x)
double cyl_center_y = -0.2025; // cylinder position (y)
DLL_CABLE::set(N_link, r, L, rho, E, nu, mu, g, cyl_radius, cyl_center_x, cyl_center_y);
```

## Set trajectory
Robot end-effector trajectory can be adjusted by changing `data/p_des.txt` (position) and `data/R_des.txt` (rotation).
`p_des.txt` is a sequence of three-dimensional positions in time, and the size should be 3 x max_simulation_step.
`R_des.txt` is a sequence of nine-dimensional (row wise SO3) rotations in time and the size shouldbe 9 x max_simulation_step.
Variable max_simulation_step is defined in `src/main.cpp`.
```
int max_simulation_step = 5500;
```

## Frame
The global frame is equivalent to the base frame of the panda robot.
Refer https://frankaemika.github.io/docs/control_parameters.html.