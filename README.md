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
Up/W &nbsp; &nbsp;&nbsp; : Move up	\
Down/S&nbsp; : move down	\
Right/D &nbsp;: move right	\
Left/A  &nbsp; &nbsp; : move left	\
E &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; : move frontward	\
Q &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp;&nbsp; : move backward	\
Space &nbsp; &nbsp;&nbsp; : reset position

## Modes
T		: change mode from shade to texture and vice versa
L		: change mode to wireframe, press again to cancel it

## Exit
Escape : exit program