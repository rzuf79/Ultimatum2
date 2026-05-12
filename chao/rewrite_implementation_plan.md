# Chrono-Splat Rewrite Implementation Plan

This document turns [design.md](./design.md) into an implementation roadmap for the current C codebase.

The goal is to rewrite the world, plot, and content while preserving as much of the working engine as possible:

1. Keep the current renderer, input, save/load, combat, inventory, spells, time gates, and procedural dungeon foundation.
2. Replace the setting, progression, NPCs, locations, and quest structure with the new Chrono-Splat design.
3. Do the rewrite in vertical slices so the game stays playable during the transition.

## Current Foundation We Should Reuse

The current code already gives us a strong base:

1. Looped overworld maps and smooth camera.
2. Scene transitions between overworld and interior maps.
3. Time gates with turn-based schedules and visual effects.
4. Turn-based bump combat and roaming monsters.
5. Player sheet, XP, level-ups, items, equipment, and spells.
6. Persistent dungeons/towers with fog of war, treasure, and save/load.
7. Data-driven NPC spawns, services, dialogue, and shop stock.

The rewrite should mostly happen in `world_data.h`, `session.h`, `items.h`, `game.h`, and eventually `generated_map_data.h` plus `tools/export_map_data.js`.

## Working Rewrite Strategy

1. Re-theme existing map slots first instead of redrawing every map immediately.
2. Move story progression into explicit quest flags and objective state.
3. Build the plot in order, starting with Act I in Neon Middling.
4. Replace placeholder locations and dialogue in batches, not all at once.
5. Treat save compatibility as disposable during the rewrite.

## Map Slot Strategy

The existing map inventory is already close to the new design:

1. The 5 current Earth-era overworld maps can become the 5 designed time periods.
2. The 9 current offworld maps can become the 9 designed planets/moons.
3. The current interior map pool is close, but we are one content map short for the full set of redesigned enterable places.

Recommended era mapping:

| Current map id | New identity |
| --- | --- |
| `pangea` | `Crustacea Prima` |
| `bc1423` | `Brass Afternoon` |
| `ad1990` | `Neon Middling` |
| `aftermath` | `The Apology War` |
| `legends` | `Minute Before Creation` |

Recommended offworld mapping:

| Current map id | New identity |
| --- | --- |
| `mercury` | `Mercury: The Toasted Coin` |
| `venus` | `Venus: The Perfume Swamp` |
| `mars` | `Mars: The Red Bureau` |
| `jupiter` | `Jupiter: The Floating Schoolyard` |
| `saturn` | `Saturn: The Ring Road` |
| `uranus` | `Uranus: The Sideways Carnival` |
| `neptune` | `Neptune: The Computer Aquarium` |
| `pluto` | `Pluto: The Forgotten Suburb` |
| `planetX` | `Planet Blivv: The Extra Planet Nobody Admits Exists` |

Important note:

1. We should plan to add one new interior map during the rewrite.
2. Until then, one location can stay temporarily merged or use placeholder geometry.

## Milestone Order

## Milestone 1: Quest State and Objective Framework

Purpose: give the rewritten plot a proper backbone before we rewrite content.

Implementation tasks:

1. Add story progression state to `U2GameSession`.
2. Add quest flags for major gates in the plot.
3. Add an objective string or objective enum shown in the HUD or message flow.
4. Add conditional dialogue support so NPCs can react to story state.
5. Add support for key-item checks on interactions, travel, and boss gates.

Suggested initial flags:

1. Visited `Castle Kindly-Bent`
2. Visited `New Snackramento`
3. Visited `Port Folio`
4. Acquired `Blue Ribbon`
5. Can command ship
6. Has aircraft permit
7. Has `Ignition Crouton`
8. Can launch rocket
9. Has `Blessing of Nonstick`
10. Has `Mirror Skillet`
11. Has `Half-Omelet Compass`
12. Rescued `Princess Semicolon`
13. Opened route to `Minute Before Creation`

Done when:

1. The game can always tell the player the next major objective.
2. NPCs can gate content and change dialogue based on flags.

## Milestone 2: Act I Vertical Slice

Purpose: replace the current opening with the designed opening and make the plot readable immediately.

Implementation tasks:

1. Change the starting era to `Neon Middling`.
2. Spawn the player outside `New Snackramento`.
3. Re-theme the first three critical Act I locations:
   `Castle Kindly-Bent`, `New Snackramento`, and `Port Folio`.
4. Replace their NPC rosters, dialogue, services, and signs.
5. Add the opening Panclock message.
6. Add the first practical goals:
   ship access, rocket access, and Brass Eggshell hunt.

Recommended first location remaps:

1. `castleBritish` -> `Castle Kindly-Bent`
2. `newSanAntonio` -> `New Snackramento`
3. `portBonifice` -> `Port Folio`

Done when:

1. A new game starts in `Neon Middling`.
2. Queen Kettle, Mayor Coupon, and Captain Ledger each provide the intended first-act information.
3. The player can understand the main quest without reading external notes.

## Milestone 3: World Rename and Time-Gate Rewrite

Purpose: make the whole world read as Chrono-Splat instead of old Ultima carryover.

Implementation tasks:

1. Rename all era maps and offworld maps in displayed game text.
2. Rewrite the `u2_time_gate_defs` network to match the design document’s gate logic and gate flavor.
3. Replace placeholder gate hints with era-specific clues.
4. Re-theme the existing dungeon and tower presets into design-appropriate locations.
5. Add the hidden `Minute Before Creation` unlock logic for later use by the `Half-Omelet Compass`.

Done when:

1. Every time gate destination reads as a design-era destination.
2. The world names, gate messages, and map names no longer expose the old setting.

## Milestone 4: Vehicles and Travel Unlocks

Purpose: support the designed movement progression without overcomplicating the vehicle model.

Implementation tasks:

1. Add simple unlockable travel capabilities instead of full vehicle simulation first.
2. Gate water travel behind `Blue Ribbon`.
3. Gate aircraft travel behind the permit from `Mayor Coupon`.
4. Gate rocket launch behind `Ignition Crouton`.
5. Add the rocket field and space departure flow.
6. Add offworld landing/return flow that works with the current scene system.

Recommended rule for first pass:

1. Vehicles are permissions, not inventory objects with separate stats.
2. We can add full vehicle entities later if the design needs it.

Done when:

1. The player can follow the route from early Earth exploration to space travel.
2. Travel unlocks are tied to story progress instead of raw map access.

## Milestone 5: Eggshell and Key-Item Framework

Purpose: support the main quest structure from Act II through Act V.

Implementation tasks:

1. Add the 12 Brass Eggshell items.
2. Mark 6 as true and 6 as false in data.
3. Add clue-bearing NPCs and item checks around them.
4. Add the `Disposable Crown`, `Ignition Crouton`, `Mirror Skillet`, `Blessing of Nonstick`, and `Half-Omelet Compass`.
5. Add fusion/assembly interactions for the Oracle Buoy.
6. Add the final required-item gate checks for the Unwritten Citadel.

Done when:

1. The game can track which shells were found.
2. The game can distinguish true vs fake shells once the player reaches the relevant reveal.
3. The late-game gate checks are data-driven.

## Milestone 6: Era Content Pass

Purpose: populate the 5 time periods with their named towns, castles, and tone.

Implementation tasks:

1. `Crustacea Prima`
   Add `Castle Ladlekeep`, `Sauropod-on-Sea`, and `The Clam Seminary`.
2. `Brass Afternoon`
   Add `Fort Picklebaron`, `Market of the Nine Almosts`, and `Hotel Umbrella`.
3. `Neon Middling`
   Finish `New Snackramento`, `Port Folio`, and `Castle Kindly-Bent`.
4. `The Apology War`
   Add `Pirate Customer Service`.
5. `Minute Before Creation`
   Add `The Unwritten Citadel`.

Design rule for this milestone:

1. Every named place needs at least one clue NPC, one resource/service NPC, and one memorable nonsense NPC.

Done when:

1. All five eras have their required named places.
2. Every era advances either clue, gate, resource, or memorable nonsense.

## Milestone 7: Offworld Content Pass

Purpose: make Act III and Act IV fully playable.

Implementation tasks:

1. Add named offworld places:
   `Town of Marjoram`, `Village of the Prefects`, `New Jape City`, `Camp Bitbubble`, `Tommorowville`, `Town of Muckler`, and the combined `Castle Crumbelune` / `Town Basko-Basko` map.
2. Make `Mercury`, `Venus`, and `Saturn` optional or misleading on purpose.
3. Add planet-specific shops, clue NPCs, and rewards.
4. Add the required plot items and passwords from the design.
5. Add the combined Blivv location map if the current interior pool is still short.

Done when:

1. The full solar-system progression can be completed.
2. Optional planets feel intentionally weird rather than unfinished.

## Milestone 8: Designed Dungeons, Bosses, and Objectives

Purpose: connect the current procedural content to the actual story.

Implementation tasks:

1. Re-theme existing dungeon/tower presets into story locations.
2. Add plot-critical objectives like:
   `Tar-Pit Shrine`, `Proto-Chicken`, clue towers, and gate-related boss rooms.
3. Use the current persistent dungeon logic as the default implementation path.
4. Only hand-author special boss floors when the procedural model stops serving the design.

Done when:

1. Dungeons and towers are part of the quest flow rather than detached side content.
2. At least one early Eggshell acquisition is fully implemented through a dungeon or boss objective.

## Milestone 9: Minute Before Creation Endgame

Purpose: finish the plot with the designed final sequence.

Implementation tasks:

1. Add the `Half-Omelet Compass` hidden-gate unlock.
2. Re-theme the `legends` era into `Minute Before Creation`.
3. Add `Princess Semicolon`, `Draft Dragon`, and `Professor Yolkayne`.
4. Add the `Mirror Skillet` reflection mechanic for the final battle.
5. Add the battlefield phase cycling across eras.
6. Add the spare/kill ending split and epilogue text.

Done when:

1. The full main quest can be completed from start to finish.
2. The ending responds to the final choice.

## Immediate Next Implementation Chunk

This is the next coding slice I would actually do first:

1. Add quest flags and objective state to `session.h` and save/load.
2. Add conditional dialogue support in `game.h`.
3. Re-theme the opening trio in `world_data.h`:
   `Castle Kindly-Bent`, `New Snackramento`, `Port Folio`.
4. Move the player start to `Neon Middling`, outside `New Snackramento`.
5. Replace the opening NPCs and messages with the Act I design equivalents.
6. Update time gates and map names for the five redesigned eras.

Reason:

1. This gives us the new story immediately.
2. It keeps the game playable while the rest of the world is still being rewritten.
3. It creates the right foundation for vehicles, Eggshells, and offworld travel.

## Files Most Likely To Change

1. `chao/ultimatum/session.h`
   quest flags, objective state, new save fields
2. `chao/ultimatum/world_data.h`
   map names, transitions, gates, NPCs, dialogue, services, story data
3. `chao/ultimatum/items.h`
   quest items, Eggshells, Mirror Skillet, plot rewards
4. `chao/ultimatum/game.h`
   progression checks, dialogue branching, objective display, travel unlock logic
5. `chao/ultimatum/generated_map_data.h`
   renamed display maps and eventually rewritten map layouts
6. `chao/tools/export_map_data.js`
   only if we regenerate or replace map content from a new source pipeline

## Deferred Until The World Rewrite Is Stable

1. Full vehicle simulation with separate combat/movement rules.
2. Major combat-system expansion.
3. Big balance passes for equipment and spells.
4. A polished quest journal UI.
5. Heavy map redraw work for every location at once.

Those are all valuable, but they should come after the plot and world structure are in place.
