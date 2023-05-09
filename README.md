# Scop
42School Project Scop

Obj render based on openGL and GLFW.
Texture can be applied on the format bmp and dds.

Some obj are in the wavefront folder and some texture are in the texture folder.

Compile the program with the command "Make"

The program usage is : ./Scop OBJ [TEXTURE]

ex :
```
./Scop wavefront/42.obj
./Scop wavefront/cube.obj texture/fire_camp.bmp
```

*NOTE*: The git repository use git lfs to stock and load large memory textures.\
        If the textures don't work, install git-lfs and reclone the repository

# Commands :
## Movements
Up/W &emsp; : Move up	\
Down/S&nbsp; : move down	\
Right/D &nbsp;: move right	\
Left/A&emsp; : move left	\
E &emsp;&emsp;&emsp; : move frontward	\
Q &emsp; &emsp; &nbsp; : move backward	\

*NOTE* : In azerty keyboard W => Z, A => Q and Q => A

## Camera orientation
M &emsp; &emsp;: Activate/Desactivate the camera control \
Mouse : Control the camera

*NOTE* : in azerty keyboad, M => ','
## Modes
T &emsp;&emsp; : change mode from shade to texture and vice versa \
L &emsp;&emsp; : change mode to wireframe, press again to cancel it \
Space  : reset position and orientation

## Exit
Escape : exit program