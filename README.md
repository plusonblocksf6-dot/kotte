# Bomberman World

**Student:** Opeoluwa Raji

## How to play

* **Controls:** WASD or arrow keys to move, Space to place a bomb on the current tile, E to activate the shield, R to restart after a round has ended, Q or Escape to quit.
* **Objective and rules:** Defeat 10 enemies with bomb blasts to win (if the seed generates fewer than 10, the target is lowered to the number that exists). Being touched by an enemy or caught in a blast is a loss. **If the player dies in the same frame the objective is completed, the round is lost.** After a win or loss, the world freezes until R is pressed. Bombs explode after 2 s, and a blast reaches the bomb's tile plus a line in each direction that stops at walls and at the first crate. Bombs are solid: the player can walk off a bomb they just placed but cannot re-enter it, and enemies are always blocked. Extension rules: up to **3 live bombs** at once (one per tile); a blast that reaches a live bomb **detonates it immediately**; pressing E gives **3 s of protection** from enemies and blasts, with the next shield available 8 s after activation (the halo blinks in the last second). Cyan pickups near the start add +1 blast range (start 3, max 6) to bombs placed afterward.
* **Configurations/demo setup:** Default. There are no debug keys or separate stress configuration. The seed is shown in the lower-left corner, and restarting reuses the same seed and layout. Tuning values such as `enemies_to_defeat_`, `bomb_capacity_` and the shield durations are constants in `application.hpp`.

## Features and validation

|Feature|Check, observed result, and expected behaviour|
|-|-|
|1. Player damage and death|**Tested:** stood in my own blast while an enemy was pressed against me, then kept touching enemies after death. **Expected:** one death only; the `defeats` counter stays at 1. **Observed:** [fill in]|
|2. Round states and objective|**Tested:** temporarily set `enemies_to_defeat_` to 1 and caught the last enemy and myself in the same blast. **Expected:** LOST, not WON (loss is checked first), and the world freezes. **Observed:** [fill in]|
|3. Safe restart|**Tested:** ended a round with 3 bombs still ticking and a blast effect on screen, then restarted three times. **Expected:** no old explosions; `bombs 0/3`, `blast range 3/6`, shield ready, pickups back, same layout each time. **Observed:** [fill in]|
|4. Solid bombs with owner escape|**Tested:** placed a bomb, walked off, tried to walk back on; watched an enemy walk into a bomb; pressed Space on a tile that already had a bomb. **Expected:** leaving allowed, re-entry blocked, enemy turns away, second bomb refused. **Observed:** [fill in]|
|5. Blast-range pickup|**Tested:** placed a bomb at range 3, collected a pickup during its fuse, then placed a second bomb. **Expected:** first blast still reaches 3 tiles, HUD shows `4/6`, second blast reaches 4; pickup gives +1 once. **Observed:** [fill in]|
|[6] Multiple bombs per owner|**Tested:** placed 3 bombs far apart, pressed Space on a fresh tile, replaced a bomb after it exploded, repeated the cycle, including once where a chain removed two bombs. **Expected:** 4th refused, count drops per removed bomb, replacement accepted, no drift. **Observed:** [fill in]|
|[7] Chain reactions|**Tested:** placed bomb A, waited ~1 s, placed bomb B 2 tiles away, with a pickup collected in between so the bombs had different ranges. **Expected:** B explodes in the same frame as A with its own range, `chain detonations` +1, no second explosion from B. **Observed:** [fill in]|
|[8] Shield / temporary invulnerability|**Tested:** activated the shield, touched an enemy and stood in my own blast, then touched an enemy after the halo ended; also pressed E during the cooldown. **Expected:** both hits absorbed (`hits absorbed` rises), hit after expiry is fatal, E during cooldown does nothing. **Observed:** [fill in]|

**Known issues:**

* **Owner escape uses the foot footprint.** If the player's footprint does not overlap a newly placed bomb (standing near a tile edge), they count as already escaped and can walk away from it but not across it. Checked with seed 323453473861: the bomb appeared beside the player and blocked them straight away. Running `update_bomb_owner_escapes()` before instead of after movement made no visible difference, because the only gap is the first frame after placement (about 1 px of movement at ~236 FPS).
* **Enemies next to a new bomb can be stuck.** An enemy whose footprint overlaps a bomb when it is placed cannot move until the bomb explodes, since only the owner may pass.
* **Shield expiry next to an enemy.** If an enemy is still touching the player when the shield ends, the next contact check is fatal. This is intended (damage resumes normally) and the halo blinks as a warning, but it can feel abrupt.
* **Discrete movement** (unchanged from the starter code). A very large frame delta could in theory move an entity through a blocker.

## Sources and assistance

This project builds on the shared Kotte foundations.

* **External code:** [raylib](https://www.raylib.com/) 6.0 for windowing, input and drawing, and [`Random<RomuDuoJr>`](https://github.com/ulfben/cpp_prngs) for seeded random numbers, both as provided with the course project.
* **Discussion / collaboration:** None
* **AI assistance:** I used Claude during the final assignment. It worked as a tutor and assisted with a lot of the implementation code for the five required features and three extensions from the assignment by explaining what the existing code did, writing some code, and making me complete it to see if I understood, and helped me prepare for the presentation by asking me a few model questions about the features added. I integrated, compiled, and tested the code myself; the observations in the table above are from my own experiments. I was also able to challenge a few things it told me, to better understand not only Kotte but also how to use the AI more efficiently without just accepting everything it gives.
