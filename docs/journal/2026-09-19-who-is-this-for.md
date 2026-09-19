# 2026-09-19 — who is this for

**Stage:** 2 usable
**Shipped this session:** no code. Tightened the A/B story after getting hung up.
**Publish later as:** “The pro product is not buildable this week”

---

## Standing questions

### Would I actually use this?

I already use both jobs the PoC does: it grabs a tool, and it parks the knobs somewhere. Then I finish. A pro with a Pro-Q shortcut does not need a generic stock EQ, and does not need my starting point. I am not that pro-with-a-shortcut today when I open Chatrack. I am the person who will take a starting point and tweak. Hung up is not the same as “I should build follow-ups for a beginner I am not.”

### What is the use case?

A and B are not two apps. They are layers, and they already coexist:

1. Summon a tool
2. Land a starting point
3. Finish by ear (or, later, by chat / voice)

Skill level changes which layers matter.

- **Pro with a real workflow:** generic EQ is not a time save. They already have the plugin and the key command. Route A only gets serious when it can pull *their* tool (“you usually put Mag EQ on this vocal”). Mixing *decisions* want machine listening. Neither is this week.
- **Someone who can hear a bit (me, today):** summon + starting point + knobs. Time is real. Starting point is useful even if I could have found it myself.
- **Someone who has never mixed:** needs the starting point more, and might need follow-ups / hands-off. That is Route B as an audience, not as “the only product.”

### Is this cheating?

For the pro, a stock starting point is almost insulting — it cheats at a job they already did. For me, a starting point cheats at setup, not at taste. Follow-ups cheat at finishing. Machine listening plus taste memory is the line I still have not drawn.

### Should this exist?

The stock-rack version should exist for the person who will summon, accept a start, and tweak. The pro version should exist later, and it is a different build (hosted plugins, preference, listening). Building follow-ups *because A looks empty* would be building for an audience I named, not the one I am. Building nothing because the pro path is blocked is the other failure.

### What did I just give up?

The clean story that “we picked A, so we know what to build.” A for pros is mostly unbuilt until real plugins. B is clearer to implement and easier to hide behind. I am not giving ADR-0001 away: chat is still not the mixer. I am giving away the idea that A and B were mutually exclusive products.

### What would make me uninstall it?

If we spend the week in circles instead of making the existing hybrid honest (knobs visible). If we ship follow-ups so we have “something to build” and then pitch it as the pro tool. If we pretend a stock rack is a pro workflow.

---

## What happened

- Built / decided / showed someone: noticed the PoC already does A *and* a proto-B (starting point). Noticed A-for-pros is blocked on third-party plugins + taste memory. Felt like maybe build B because B has a todo list. That feeling is a trap. Tag features by audience. Do not pick a feature set until there is more than one audience to ship to.
- Surprise: “what do you build for Route A?” is a good question. Answer for *this week*: make the hybrid you already have usable (tool surface). Answer for *the pro*: don’t, yet.
- Evidence: none.

## Open threads

- Tag every queued feature `A-pro` / `A-now` / `B-hands-off`. After enough exist, you can cut a feature set per audience. Not before.
- ADR-0001 still means: do not make chat the mixer. It does not mean “starting points are B and must die.” Starting points are already in the recipes.
- Machine listening: Stage 4. Real. Not a way out of today’s hang.
- Next build is still the tool surface, for *me*, tagged `A-now`. Follow-ups stay filed, tagged `B-hands-off`. Plugins + taste stay parked, tagged `A-pro`.
