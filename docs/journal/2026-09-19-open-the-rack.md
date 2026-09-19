# 2026-09-19 — open the rack

**Stage:** 2 usable / 3 conversational (this slice) — Stage 4 listening stays a side experiment
**Shipped this session:** prompt can set real params, not only magnitude
**Publish later as:** not a post

---

## Standing questions

### Would I actually use this?

I would use a summon that can put a shelf at 12 k when I say shine. I would not use another synonym table. I already have seek/sonic-analysis for talking about tracks. That is not this plugin.

### What is the use case?

Drum loop in AudioPluginHost. “Make it brighter” then “high end to shine on the cymbals.” Today that added 0.25 dB at 7 kHz. A mixer would move the shelf. The model already knows that. We forbade it.

### Is this cheating?

Letting GPT pick 12 kHz is still a starting point I finish by ear. Mapping “brighter” to a recipe is a preset. The first one I respect more.

### Should this exist?

Yes if the thought is on the card and the knobs stay live. No if we train a 4B model before this works.

### What did I just give up?

The fantasy that intelligence lives in recipes.json. Also: not starting the mix-deconstruct intern inside this repo. That belongs in seek.

### What would make me uninstall it?

Same shelf, same gain, a new sentence. Or a chatbot essay instead of a tool.

---

## What happened

- Decided: drop the closed “more/less magnitude” rule. C++ already has `parameter_overrides`. The prompt never offered them. LLM2Fx-Tools is the neighborhood (tools, then a plan) but their job is reverse-engineering a reference, not this insert.
- Seek already did Layer 1 librosa + Layer 2 GPT-4o-audio (July 11, four bounces). A&R notes, not mix deconstruction. ChatGPT stem drop is the listening test; do not wait on it to open Chatrack.
- Evidence: `logs/latest.md` 12:25, cymbals follow-up.

## Open threads

- Route A knobs-open / A/B still queued. Do not block on it.
- Mix conversation / stems: seek, not a Chatrack branch.
- Ears (band energy into context) after this is audible.
- Master A/B does not gain-match. Dry is quieter than the processed rack, so bypass can read as “worse” for the wrong reason. Note it; not this slice.
