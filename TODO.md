# TODO

## Changelog

- Integrate IMGUI
- Play around with IMGUI

## Planned

- Line of sight immediate check function
- Enemy AI states: Idle, Follow, Attack
- Simplify attack AI
- Player attacks
- Death -- player; other entity
- Controlling any character on the screen
- Considering entities for collisions
- Considering entities for line of sight

-----------------------

## Game Backlog

### Definitely for first pass

- Some bug with pathfinding (goes in the wrong direction at first)
- Inventory and items
- Machines, processing, basic crafting
- UIs for inventory, for machines, for containers, etc.
- Pausing game
- Moving between levels and storing existing levels; relation between world, level and entity store

### Consider for first pass

- Saving/loading (different now than it was in unity -- I could even just dump memory as first "save files")

### Unsure

- How can I smoothly transition between a map "thing" -- tile -- and an entity. E.g. breaking wall. Controlling a tree. Tree-growing logic. Etc. Do those just become entities when they need to be?

------------------------

## SAV Backlog

- String manangement (pool/bucket array)
- SAV managing its own memory for rendering (collect data in ram before subbing to gpu ram) + simplifying passing vertex data from user code
- Better string drawing/extending into a GUI library
- My own printf/format string extensions. E.g. for v2, etc.
- Memory Arena: dynamic arrays.
  - Reserve size on a mother arena to make a "dynamic array" memory arena
  - Push elements onto dynamic array one by one, keep track of count internally
  - GetElement() -- checks bounds
  - Iterator
  - Potential problem: a lot of the times I want to deal with counts explicitly in user code
