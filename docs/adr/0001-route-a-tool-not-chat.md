# Route A: get me the tool, I finish by ear

**Status:** accepted
**Date:** 2026-09-19

Chatrack is a tool picker, not a chat mixer. Typing summons a processor with a sensible starting point. The last move is always a knob. We do not build follow-up chat (“too much”, “more”, “actually warmer”), voice-hands-off, or “describe it until it sounds right.”

## Why

That is how the product is already used, and it matches the original thesis: typing replaces tool hunting, not knob tweaking. Now that anything is cheap to generate, the differentiator is the courage to build one thing. Adding Route B because it is easy would make the product unclear — a mixer who can hear would not know whether to turn a knob or argue with the model.

## Considered

- **Route B (chat until it sounds right), including as a “cheap second mode.”** Rejected. A second mode is still a second product. We will not keep a parked feature in the build queue just because the plan already exists.
- **Build both and see.** Rejected on principle. Product taste is what you refuse.

## Filed, not killed

Route B plan: [`.cursor/plans/multi-turn_chat_context_febfcce8.plan.md`](../../.cursor/plans/multi-turn_chat_context_febfcce8.plan.md). Journal: [`docs/journal/2026-09-19-two-use-cases.md`](../journal/2026-09-19-two-use-cases.md), [`docs/journal/2026-09-19-picked-route-a.md`](../journal/2026-09-19-picked-route-a.md).

## Consequences

- Success = I reach for Chatrack instead of digging up an EQ, then I mix. Not “the chat understood too much.”
- Later intelligence (hosted plugins, past-session taste) means *which tool*, not *finishing the sound in language*.
- A future reader who wonders why there is no conversation history in `handleChatMessage`: that is this ADR, not an unfinished stub.

## Clarified later the same day

A and B are **layers and audiences**, not two exclusive apps. The PoC already does both: it summons a tool (A) and parks a starting point (proto-B). Then I tweak.

- **A-now:** stock rack, summon, knobs. Who: me, someone who can hear and will finish. Buildable this week (tool surface).
- **A-pro:** hosted plugins + “you usually grab Mag EQ.” Who: someone with a real workflow. Not buildable until we host third-party tools. A generic stock EQ does not save that person time.
- **B-hands-off:** follow-up chat, voice, “too much.” Who: never-mixed, or walking around the room. Filed, not queued.

ADR-0001 still means chat is not the mixer. It does not mean starting points are forbidden, or that we must invent A-pro work to avoid looking idle. Do not build B *because A-pro has no todo this week.* Journal: [`2026-09-19-who-is-this-for.md`](../journal/2026-09-19-who-is-this-for.md).
