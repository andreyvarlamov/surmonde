# TODO

## Changelog



## Planned

- Moving between levels and storing existing levels; relation between world, level and entity store

-----------------------

## Game Backlog

### First pass

- Saving/loading

### Second pass

- Systematize crafting
- Lighting
- Piping
- Power
- Digital signal
- Universal chem synth idea
- Computer (shell ui controlling digital signal outputs)
- Computer player-created GUI

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

------------------------

## SAV Backlog

- SAV managing its own memory for rendering (collect data in ram before subbing to gpu ram) + simplifying passing vertex data from user code
- Better string drawing/extending into a GUI library
- My own printf/format string extensions. E.g. for v2, etc.
- Memory Arena: dynamic arrays.
  - Reserve size on a mother arena to make a "dynamic array" memory arena
  - Push elements onto dynamic array one by one, keep track of count internally
  - GetElement() -- checks bounds
  - Iterator
  - Potential problem: a lot of the times I want to deal with counts explicitly in user code
