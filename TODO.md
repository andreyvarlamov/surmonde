# TODO

## Changelog

- Item pickup entity (WIP)

## Planned

- Item pickup entity logic; picking up and dropping items
- Inventory UI; drop functionality
- Container entities
- Inventory UI drag and drop between different inventories

-----------------------

## Game Backlog

### First pass

- Machines, processing, basic crafting
- Game log
- Moving between levels and storing existing levels; relation between world, level and entity store
- Saving/loading

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
