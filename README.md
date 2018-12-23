# Neko Engine

Neko Engine is a 3D game engine currently being developed by two students from CITM-UPC Terrassa, Sandra Alvarez and Guillem Costa.


///////////////////////////*FORK FOR ASSIGNEMNT 3 ENGINE*////////////////////////////////////////////////////////////////

- Miquel Gironés
- Github profile: https://github.com/SenyoraCatolica/NekoEngine
- Github engine repository: https://github.com/SenyoraCatolica/NekoEngine/releases/tag/v.06
- WARNING! Assignment 3 developed in Assignment3 branch


Assignment 3 : PHYSICS PART

In this part of the project I have implemented a module of physics with Bullet.

The modules physiscs updates all the bodies and creates all the physics elements when play is pressed

Includes:
	- Components box collider and sphere collider as colliders
	- Component Rigidbody 3D
	- Component Car
	- Component Constraint point to point

Instructions:

- Create a physicalized object

	Add a rigidbody component:
		Edit the mass and set if is kinematic or not
	Add collider:
		Add Component box to create a cube collider
		Add a Component sphere to create a sphere collider
		Do not add any collider if not wanted a collider

- Create a car object

	Create an empty object and add a Component Car
		You can adjust the car options as you like

	Car movement: Every car is moved with the arrows of the keyboard

- Create a Constraint poit to point 

	Create two objects
	Add a component Collider, a rigidbody and JP2P components on each one
	On the JP2P component write the name of the other object you want to pair with on each object
		Note: the name can't be the same as the current object and the other object must have all the component to merge correctly
	Check for erros in the console if not working


- Others

	-The camera contains a sphere collider
	-Press 1 to shot a physical sphere to interact with physical objects








///////////////////////////*ORIGINAL ENGINE*////////////////////////////////////////////////////////////////

Neko Engine is a 3D game engine currently being developed by two students from CITM-UPC Terrassa, Sandra Alvarez and Guillem Costa.

- Sandra's GitHub account: [Sandruski](https://github.com/Sandruski)
- Guillem's GitHub account: [DatBeQuiet](https://github.com/DatBeQuiet)
- Our GitHub organization: [WickedNekomata](https://github.com/WickedNekomata)

## About the engine

The code is written in C++.

- GitHub repository: [NekoEngine](https://github.com/WickedNekomata/NekoEngine)

### Features

#### Game Objects
- Drag and drop game objects in the Hierarchy.
- Create/Delete game objects by right-clicking them at Hierarchy.
- Delete selected game object at Hierarchy by pressing the Delete key.

- Create/Delete components in Inspector.
- Swap/Reorder components in Inspector by drag and drop them through the 'Move' button.
- If more than one camera is set as main, play mode cannot be entered.

#### Resource Manager
Assets panel:
- The entire Assets folder is shown in real-time at the Assets panel.
- If any file (asset or meta) is removed/moved/renamed/changed from OS (offline mode), click the 'Refresh' button to update the affected resources.
- Dropped files will be imported.
- When importing a model, if it has a texture, the engine will search for it in the Assets folder. If it exists, it will be assigned to the model.

**IMPORTANT: after dropping any file into the engine or changing something in the Assets folder from outside the engine, push REFRESH button at Assets panel.
If the button is not clicked when one of this situations happens, the Assets panel will show the new files but resources will not be updated, so they will not work properly (invalid resource). 
Sorry for the inconvenience!**

- Drag and drop any texture to a material component.
- For models (fbx, dae and obj), open them to see its associated resources and drag and drop them to a mesh component.

- Select any texture or model and see its current import settings at Inspector. Change them and reimport the asset.

Library panel:
- Drag and drop any resource from the Library panel to an equivalent component.

- Select any resource and see its information at Inspector.

#### Scene Serialization
- Each new model in Assets generates a scene. To load the scene, drag and drop it from the Assets panel to the Hierarchy.
- Save the current scene or load a scene from Menu->File->Save Scene/Load Scene.

#### Options
- At Debug Draw panel, change the current debug draw options.
- At Edit panel, change between guizmo operations (shortcuts: W, E, R) and/or modes (shortcut: T).
- At Edit panel, enter Play mode. When entering Play mode, the scene is saved in memory.
- From Play mode, Pause or Tick the simulation.
- When leaving Play mode and entering Editor mode, the scene is loaded from memory.

#### Other
- At Settings panel, Scene section, check the quadtree structure. For debug purposes, random static game objects can be created (they will be automatically added to the quadtree).
- At Settings panel, Time Manager section, check the values of the game clock and the real time clock.


If something is not working as expected, please, revise console panel to understand what is happening.
If, after that, you assume the engine is running an error, revise issues or send a new one (we would be very grateful).

New features will be added soon in future releases. :)

### Controls

#### Camera
- Mouse Hold Right:
	- WASD: move forward, left, backward and right
	- QE: move up and down
	- Look around
- F: look at target
- Hold Alt:
	- Mouse Hold Left: look around target
- Mouse Wheel: zoom in/out
- Mouse Hold Shift: movement speed x2, zoom speed x0.5

#### Windows
- Hold Ctrl:
	- I: open/close Inspector panel
	- S: open/close Settings panel
	- C: open/close Console panel
	- H: open/close Hierarchy panel
	- A: open/close Assets panel
	- D: open/close DebugDraw panel

#### Other
- Change ImGuizmo operation/mode:
	- W: Translate operation
	- E: Rotate operation
	- R: Scale operation
	- T: World/Local mode

### Game exe

Executing Game.exe you will execute a build generated by the engine. 
The current scene for the game.exe is stored at Settings. Please, do not remove it.

### Tools used to develop the engine

- IDE: Microsoft Visual Studio 2017
- External libraries: 
	- SDL 2.0.8
	- OpenGL 3.1
	- Glew 2.1
	- Assimp 4.1
	- DevIL 1.8
	- ImGui 1.66 WIP
	- ImGuizmo 1.0
	- MathGeoLib 1.5
	- PCG Random Number Generator 0.94
	- Parson
	- PhysFS 3.0.1
	- MMGR

### Innovations

- UVs Matrix: you can modify UVs' transform for each texture from a game object.
- Docking and Viewports: the engine uses the ImGui docking & viewports beta.

## License

MIT License

Copyright (c) 2018 WickedNekomata (Sandra Alvarez & Guillem Costa)

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
