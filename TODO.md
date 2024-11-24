# TODO

## Changelog

- Saving/loading: file io prep

## Planned

- Saving level store
- Saving level tile content
- Saving entities
- Saving inventory blocks
- Loading level store
- Loading level tile content
- Loading entities
- Loading inventory blocks

-----------------------

## Game Backlog

### First pass

ALMOST DONE!

### Second pass

- Systematize crafting
- Lighting
- Piping
- Power
- Digital signal
- Universal chem synth idea
- Computer (shell ui controlling digital signal outputs)
- Computer player-created GUI

### Later pass

- Needs and desires :)
- Limb-based health system
- Fully continuous world; chunk-by-chunk background streaming
- Any-angle pathing
- Skills, attributes, abilities, etc.
- Friendly NPCs. Dialog, trading
- Cities, factions
- Remote entity processing (low resolution processing of entities that are not fully loaded in)
- Electrical circuit simulation. Optimize through a kind of mipmapping?
- Coherent world generation
- History generation
- Fluid simulation

### Misc

- Controlled Entity Vision. Should be possible to disable fully, not just visuals; meaning all tiles are actually considered visible
- If an entity (that is only controlled one for now) precalculates vision, and has visible tiles array, use that, instead of tracing line in IsInLineOfSight
- Checking line of sight of controlled entity if clicking in the world. Should not be clickable
- Some bug with pathfinding (goes in the wrong direction at first)
- When an actor gets attacked they should automaticall y go into combat
- Check path finding when setting move target before changing ai state
- If clicking on a non-walkable tile, don't even attempt to navigate there
- Symmterical A*
- Entity flags
- Hide entities that are out of line of sight
- Item stacks
- Transition tilemap drawing system to use central sprite and sprite atlas system
- Entity slot reuse
- Different sprites for different states of entities ("animation")
- ImGui input filtering - if a window is focused, it lets mouse events outside the window through, but it stops keyboard events until the window is unfocused. I only want to stop keyboard events if there's a keyboard shortcut that's possible in that window.
- Logic for cycling game log entries, if going over the count limit
- See about getting rid of tile px w and h in most of the places, except actually rendering
- Use ImGui::SetNextWindowSize, with ImGuiCond_FirstUseEver flag to set default sizes
- ImGui::Begin returns false if window is collapsed, so I can early out
- Use ImGui::MenuBar for something
- ImGui::TextLinkOpenURL
- ImGui::Separator
- Fix out of bounds navigation targets

### Misc 2

- Easy enum defintions + string values + get string helpers using X macros
- All imgui windows handled in a doubly linked list of windows, regardless of type of menu
- Also should be possible to close imgui windows using the x button, and that should update the window list accordingly
- Also internal state of each window (e.g. some setting that changes how to view this or that) should be stored in those linked list nodes, instead of static variables in function; also current selection of valueboxes
- EntityStore debug window
- Common components for links to certain objects: for example for entities -- in AI debug menu, there's followed entity field, don't just do custom format string for that entity, but have some helper function that takes entity, and displays a **link!** to an entity, clicking on it brings up that entity's debug menu, or brings it forward and focuses it, if already open. Use that common component for all instances of showing an entity!
- Think about keeping entity and inventory store pointers as global variables inside the inventory and entity modules, instead of passing around everywhere

------------------------

## SAV Backlog

- SAV managing its own memory for rendering (collect data in ram before subbing to gpu ram) + simplifying passing vertex data from user code
- My own printf/format string extensions. E.g. for v2, etc.
