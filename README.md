# Scop
42School Project Scop

Obj render based on openGL and GLFW.
Texture can be applied on the format bmp and dds.

Some obj and textures are in the wavefront folder for example.

Compile the program with the command "Make"

The program usage is : ./Scop OBJ [TEXTURE]

ex :
```
./Scop wavefront/42.obj
./Scop wavefront/cube.obj wavefront/fire_camp.bmp
```

# Commands :
## Movements
Up/W &emsp; : Move up	\
Down/S&nbsp; : move down	\
Right/D &nbsp;: move right	\
Left/A&emsp; : move left	\
E &emsp;&emsp;&emsp; : move frontward	\
Q &emsp; &emsp; &nbsp; : move backward	\

## Camera orientation
M &emsp; &emsp;: Activate/Desactivate the camera control \
Mouse : Control the camera

## Modes
T &emsp;&emsp; : change mode from shade to texture and vice versa \
L &emsp;&emsp; : change mode to wireframe, press again to cancel it \
Space  : reset position and orientation

## Exit
Escape : exit program