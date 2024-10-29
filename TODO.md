# TODO

## Changelog

- Removed old line of sight code
- Think, think, think; update TODO

## Planned


## Backlog

- Some bug with pathfinding (goes in the wrong direction at first)
- Simple line of sight
- Enemy AI; considering line of sight; etc.
- Simplify combat; get rid of everything; just each entity going through its own attack cycle; death
- Controlling any character on the screen
- Considering entities for collisions
- Transition to tiles instead of ASCII
- World gen same as it was in unity
- Integrate IMGUI
- Inventory and items
- Machines, processing, basic crafting
- UIs for inventory, for machines, for containers, etc.
- Pausing game
- Moving between levels and storing existing levels

### Consider for first pass

- Saving/loading (different now than it was in unity -- I could even just dump memory as first "save files")

# OLD

## SAV
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

## SURMONDE
- Use pathfinding in combat
  - Simple: just don't walk into walls
  - Complex: adjust position based on that data, e.g. if cornered, get out
- Line of sight
  - Limiting character field of vision
  - Limiting player field of vision
  - Map memory/fog of war
  - Lighting
- Non-combat character AI
- Multiple levels
  - Relation between level and entity store
  - Moving between levels in-game
- Overworld
- More work on world gen
  - Overworld level world gen
  - More level variants
- Machines
- Multiple controlled entities
