The Zombie Engine

Zombie Engine History

    The Zombie Engine is the result of years of development of a game engine,
    framework and toolset, created as an in-house technology suite for creating
    PC games by Spanish developer
    <a href="http://www.tragnarion.com">Tragnarion Studios</a>. Its starting
    point is <a href="http://nebuladevice.cubik.org">nebula2</a>, an open source
    game engine originally developed by
    <a href="http://www.radonlabs.de">Radon Labs</a>, and
    extended by the open source community. Developers at Tragnarion Studios took
    the original nebula2 engine and extended it in a number of ways, and created
    a complete game framework and toolset, providing designers and artists with
    a powerful environment.

    Early stages of development were focused on extending and improving the
    features of base engine modules: rendering, culling, animation, terrain,
    physics, as well as the exporter tool for artists. These were used to put
    in motion the development process that would allow creating prototype levels
    and testing the engine capabilities, which would determine the constraints
    and requirements for the game design.

    Later on, came development in parallel of the fundamental entity framework
    that would be used as the building block for the game framework, as well as
    the first gameplay-related features and utilities, such as navigation meshes
    for pathfinding, finite state machines, and other basic gameplay structures
    such as paths, triggers or spawners. Along these, the Conjurer editor was
    built to provide an integrated editing environment for building maps,
    importing data assets, and tweaking a large number of configuration
    parameters for the different features and components that were being added.

    After all these were settled down, development moved on to create the first
    playable prototype, that would include and expose for edition all of
    high-level game features expected from a modern engine, including agent
    behavior, game actions, combat, and other such types of interactive
    features. Also at this point data replication was supported for all
    network-relevant entities and properties, for multiplayer gameplay. Several
    optimizations were being added to the graphics and culling systems, and
    other relevant modules such as sound and game GUI.

    By September 2006 the first playable, feature-complete prototype showing the
    possibilities of the Zombie Engine was completed. It allowed loading and
    playing in a multiplayer session through a whole level, allowing continuous
    gameplay through detailed interiors as well as open exteriors, combat and
    weapon system, enemy AI, etc. at interactive frame rates.

    Development of the Zombie Engine stopped in early 2007, having been used to
    release a successful playable demo which showed how complete and powerful
    it was. A year later, Tragnarion Studios was proud to announce the release
    of their stable code base for the Zombie Engine as open source,
    under a [...] license, which will allow developers worldwide to use it for
    their own project, both amateur and professional.

Differences With Nebula2

    Changed modules

    This is a summary of all changes introduced in the Zombie Engine to modules
    existing in nebula2, both in core nebula2 and contributed modules.

    - Kernel
      - Extended nArg, nEnv types
      - object cycle
      - loading and persistence of dependent objects
      - dynamically extending class with scripting
      - macros for easy scripting of commands ans signals
      - script class server, manages persistence of script code
      - dynamic creation and loading of classes
      - extended build system, generation of xmls schemas, support of libraries
        for vs71 and vs8

    - Graphics
      - Camera support for vertical field of view
      - Extended mesh interface: geometry transforms
      - Extended shader interface: parameter slots for collected array parameters
      - Extended texture interface: texture copy, rectangle locking
      - Extended ShaderParams: clearing parameters, matrix and vector arguments
      - Mesh optimization

    - DX9 Renderer
      - Implemented extended interfaces for meshes, shaders and textures
      - State manager for D3D9 Effects

    - Input
      - Input recording and playback

    - Audio
      - extended CSound, CStreamingSound classes
      - extended nSound3, nDSound3 interfaces
      - extended nSoundResource, nDSoundResource

    - Character
      - Extended character, Joint interfaces for direct skeleton manipulation
      - Extended Skeleton to support dynamic attachments

    - File system
      - RLE files
      - Directory Watchers

    - Resource
      - Hot resource reloading

    - Gui controls
      - Brush label
      - Fast selector
      - HorizontalSliderBoxed
      - HorizontalSliderBoxedFloat
      - TimedMessageWindow
      - ToggleButton

    - Utilities
      - Integrated support for Visual Leak Detector
      - Logging system with visual interface
      - Extended macros for asserts
      - extended classes, hash table, etc.
      - Hierarchical profiling

    - Math library
      - extended with polygon, oriented bounding boxes, cylinders, etc.

    - Python
      - Added signal binding

    Derived modules

    These are modules developed for the Zombie Engine but derived from existing
    modules in the nebula2 distribution, to which therefore we owe credit.

    - Spatial database
      - Support for Octree and Quadtree outdoor spaces
      - Support for Indoor spaces, Cells and Portals
      - Space persistence and loading
      - Occlusion Culling
      - Horizon Culling
      - Spatial Queries
      - Frustum Visibilty traversal, Plane Clipping
      - Light and shadow visibility traversal
      - Dynamic light regions and environments

    - Scene
      - Revised and optimized scene rendering system
      - optimized scene graph traversal and rendering
      - new types of geometry nodes and animators
      - batched and instanced rendering

    - Animation
      - Decoupled skeleton description from scene
      - Shared skeleton definition
      - Direct joint controllers
      - Rag-doll animators

    - Application
      - Extended application and state interfaces
      - Viewport system with support for multiple views

    New modules

    These are the modules entirely developed for the Zombie Engine.

    New engine modules

    - Entity
      - Framework for game entities
      - Component interface for entity composition
      - Entity Life cycle, scripting and persistence
      - Entity class management, persistence and extension

    - Physics
      - ODE-based physics system
      - Collision detection
      - Rigid-body simulation
      - Physics Spaces
      - Vehicles
      - Water and Buoyancy
      - Player control
      - Rag-dolls
      - Cloth

    - Material
      - Abstract shader description
      - Material trees and context-dependent shader selection
      - Automated shader generation through scripting

    - Geomipmap
      - Patch-based terrain definition
      - Optimized terrain rendering with cached dynamic geometry
      - Multiple level-of-detail systems, sorting and skirts
      - Terrain material blending
      - Terrain lightmap support

    - Vegetation
      - Grass layer distribution
      - Optimized grass rendering
      - Support for visual grass edition

    - Levels
      - Map persistence and loading
      - Support for editor layers
      - Loading areas
      - Game saving and restoring
      - Resource preloading

    - Assets
      - Game asset life cycle
      - Custom asset format definition
      - Handling of dependent objects

    - Dynamic effects
      - Support for decals and spawned effects
      - Management of effect pools

    - Network
      - Enet wrapper system
      - Server and client connections
      - Support for remote procedure calls
      - support for network replication

    - Class scripting
      - Dynamic class extension through scripting
      - Class command management

    New Graphic Features

    - Geometry transforms: billboards, swinging, waves
    - Surface maps: clip, bump, parallax, environment, emissive, blending
    - Vertex colors for blending and shading
    - Alpha surfaces: two-sided, alpha-lighting, fresnel
    - Light types: ambient, point, directional, spotlight
    - Light attenuation
    - Map fade-out
    - Ambient fog: linear, layered
    - Multi-layer terrain material
    - Terrain lightmap and shadowmap projection
    - Grass integration with terrain material
    - Billboard-rendering of impostor with dynamic lighting
    - Multiple light support
    - Dynamic shadow volumes
    - Shader constant-based geometry instancing
    - Multiple material profiles, shader quality selection

    New Exporter Module

    - Geometry exporter with levels of detail
    - Custom material edition and exporting
    - Texture exporter with normal map filtering
    - Texture animation exporter
    - Character and animation exporter with morphing and ragdolls
    - Indoor cells and portals exporter
    - Collision layer exporter with game materials

    New Editor Modules (Conjurer)

    - Editor framework
      - Editor camera control
      - Multiple viewports with dynamic layout
      - Realtime entity placement, inspection and edition
      - Realtime terrain geometry edition
      - High-resolution terrain material painting editor with streaming for
        large-terrain painting
      - Area and volume edition
      - Sound placement
      - Waypoint path edition
      - Navigation mesh builder
      - Graphical State Machine edition
      - Automated and custom geometry batch builder
      - Terrain lightmap generation and rendering
      - Editor views with debug visualizations
      - Distance measuring
      - Layer management
      - Shell scripting

    - Crash report
    - Debug
    - Undo

	New Game Framework Modules

    - Navigation
    - Pathfinding
    - Finite State Machines
    - Game materials
    - Triggers
    - Waypoints
    - Game actions
    - Sound and Music

	New Gameplay Modules (Renaissance)

    - Game actions
    - Player
    - Agents
    - Vehicles
    - Wheels and Doors
    - Spawners
    - Inventory and Weapons
    - Network replication

Zombie Features

    The Zombie engine is a game engine and framework based in nebula2, an open
    source engine from Radon Labs. nebula2 has been extended in a number of ways,
    and a full game framework has been created around it. It is mainly oriented
    towards the creation of a multiplayer PC action game, but it can be adapted to
    suit the needs of similar projects, and extended to support other genres and
    platforms.


	Components

    The Zombie Engine is not a standalone, out-of-the-box solution, but a suite
    of modules and tools:

    Core

    These are the low-level modules that provide the basic services used by the
    rest of the framework. The Zombie Engine makes use of a significant part of the
    core modules modules in nebula2, such as: kernel, file and resource management,
    graphics, particles, character, audio and video, lua and python scripting, and
    other utility modules. The source code for these modules can be found in the
    code/nebula2/ path in the zombie distribution.

    Along with these, a few other modules have been derived from existing
    modules in the open source nebula2, but their architecture has been changed or
    extended in significant ways, although they still make extensive use of the
    contribution from the original nebula2 modules. These are the application,
    scene, animation, and spatial modules.

    Finally, some basic modules have been created from scratch to extend the
    core system provided by nebula2, and to provide the remaining building blocks
    required by the exporter, editor and game frameworks. These modules are entity,
    material, terrain, physics, network, sound and music, and class scripting.

    Regardless of their origin, all of these modules will be called Zombie Core,
    for the sake of this documentation.

    Conjurer

    To this group belong the modules, classes and scripts that build up the
    editor framework, including the Conjurer and Summoner tools. Conjurer itself is
    built on top of the nebula2 application module, and is divided in two big
    pieces. One is the conjurer module itself, including the 3D interface for
    editing entities and terrain, and other important modules such a Debug
    visualizations and Undo commands.

    The other half is Summoner, a MDI-style application built in Python using
    the wxPython libraries for gui creation, that is fully integrated with the
    Conjurer module, for which it provides the toolbars, dialogs and inspectors
    required for editing operation. Summoner includes such powerful features as
    interactive log window, script edition and execution, class and entity browsing,
    and the ability to inspect, edit and save the game library and entities.

    Working together, Conjurer and Summoner are the main entry point for game
    designers, artists and programmers alike, to build maps, define behaviors and
    manage the game database as stored in a shared repository (see Working Copy,
    below).

    3DSPlugin

    The 3DStudio Max plugin is the main entry point for artists to work on art
    assets and export them into the working copy, thus integrating them into the
    game database. The plugin itself is a combination of a nebula2-based module, and
    a set of MaxScript files that present the artists with an interface for defining
    Zombie-specific materials, spatial structures (cells and portals), and other
    such constructs, that will be converted into Zombie assets at export time.

    Renaissance

    The high-level game framework defines all classes, states and behaviors that
    allow game designers and programmers to create a game. This is an extensive set
    of modules built on top of the core modules, most significantly the entity
    system. It plays 2 roles in developing a game: exposing the attributes and
    contents that will define the behavior of the game in human-readable and
    -editable form. And serving as a base for programmers to create the game
    specific classes that will implement design requirements.

    In order to play these two roles, the Renaissance framework includes such
    generic game-related modules as Navigation and Pathfinding, Finite State
    Machines, Triggers, Waypoints, Spawners, Special Effects or Game Actions. But
    more specific modules are provided that cope with the specifics of a multiplayer
    action game: Enemy AI, Combat and Weapons, Vehicles, Doors, and so on.

    These modules can be found in the Zombie code, broken down in the different
    areas they cover: entities, gameplay, actions, network, etc. most of which need
    to be customized for other, specific types of games, but represent a sample of
    the kinds of behaviors that it is possible to implement through the Zombie
    engine.

    Finally, Renaissance provides developers with an application framework,
    required to pack it all up, and run the game as stored in the working copy,
    stripping all tools and auxiliary code used by the editor. It also includes such
    things as the handling of game menus, interface for game connections, and other
    such user-oriented features.

    Zombie Core Features

    The modules included in the nebula2-based Core provide the following set
    of utilities and features. For more information on nebula2 modules, please
    refer to the nebula2 open source documentation
    (http://nebuladevice.cubik.org/documentation/nebula2/index.shtml).

    - Kernel
      - Object system
      - Hierarchical object namespace
      - Scripting
      - Object persistence
      - Signals
      - Entities
      - Resources
      - Variables
      - File system
      - Multithreading
      - Time management

    - Utilities
      - Console
      - Logging
      - Profiling
      - Crash report
      - Math library
      - Smart pointers
      - Resource and package management (builders, loaders, packaging)

    - Systems
      - DX9 graphics
      - Input
      - Particles
      - Skeletal animation
      - Scene renderer
      - Spatial database
      - 2D GUI
      - Application framework
      - Terrain renderer
      - Graphics materials
      - Collision
      - Physics
      - Audio and video
      - Network
      - Special FX

    - What it lacks or could be improved
      - Object property slots (not only commands)
      - Property types- vector, list, dictionary
      - Multiple platforms
      - Memory management
      - Multicore support
      - Multithreading

    Conjurer Features

    - Conjurer
      - Editor viewport- Editor grid with Snap to grid /
      - Viewport layout

      - Entity placing: single and multiple entity placing
      - Entity transform: translation, rotation, scaling
      - Entity picking and selecting

      - Game asset management
      - Game level management
      - Editor layer management
      - Save and restore

    - Terrain editor-
      - Terrain geometry editor: raise, flatten, noise, slope, smooth
      - High resolution terrain material painting editor with streaming for
        painting large terrains
      - Terrain holes
      - Grass layer

    - Distance measuring tool
    - Sound source editor
    - Trigger editor
    - Waypoint editor

    - Terrain lightmap builder
    - Navigation mesh builder
    
    - Superentity editor: grouped entities
    
    - Geometry batch builder
    - Geometry impostor builder
    - Geometry stream editor

    - Utilities
      - Viewport debug options
      - Viewport debug visualization
      - Camera bookmarks
      - Distance measuring
      - On-screen logging
      - Undo commands

    - Summoner
      - Grimoire: entity class library
      - Object browser
      - Entity browser
      - Object and class inspection

    - Lua console
    - Python console
    - Python script editor
    - Lua class script editor

    - User and level presets

    - Layer management

    - Terrain editor
      - Geometry tools
      - Terrain materials
      - Grass brushes

    - Music table
    - Sound scheduler

    - Particle system

    - Animation management
      - Entity attachments

    - Grass editor
    - FSM editor
    - Spawner editor
    - Event editor
    - Mission objectives

    3DSPlugin Features

    - Class and asset exporter
      - Export to temporary working copy for preview

    - Geometry exporter
      - Level-of-detail

    - Material exporter
      - Texture conversion
      - Texture animators

    - Animation exporter
      - Skeletons
      - Animations
      - Plugs

    - Collision exporter

    - Visibility exporter
      - Cells
      - Portals
      - Occluders

    Renaissance features

    - Navigation meshes
    - Triggers
    - Paths
    - Spawners, Populators
    - Game materials
    - Game camera
    - Mission handler

    - Inventory
      - Items
      - Weapons
      - Accesories

    - Sound layer

    - Effect manager
      - special fx
      - impact sounds

    - Network replication

    - Game AI
      - Basic actions- decision
      - Pathfinding
      - FSMs
      - Perception, Events
      - Movement engine
      - Memory
      - Enemy AI
      - Squad behavior

    - Game combat
      - Melee combat
      - Ranged combat
      - Group combat
      - Missiles
      - Vehicles

    - Damage
	