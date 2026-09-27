# Chatrack Roadmap

Living status. Update this file when a stage starts, ships, or gets parked.
Journal entries go in `docs/journal/`. Technical sessions stay in `WORKLOG.md`.
Stage-end essays go in `docs/essays/` when a stage's questions have actually changed.

**Current stage:** 2 — Make it something you would open
**Last updated:** 2026-09-19 (end of day)

---

## How to use this

Two tracks run in parallel:

| Track | Job | Where | Becomes |
|---|---|---|---|
| Build log | What shipped, what broke, what you decided | `WORKLOG.md` | Technical posts |
| Thought log | Honest answers to the standing questions | `docs/journal/` | The blog series |

**Feature gate:** no new roadmap item starts without a journal stub. Five sentences is enough. Copy `docs/journal/_TEMPLATE.md`.

**Stage gate:** a stage is not done when the code works. It is done when its gate below is true.

**Parking lot:** Magenta / generation, “full mix agent,” becoming a DAW, product-vs-OSS, and “taste from past sessions” are identity questions, not this week's todos. See below.

---

## Two routes (layers, not two apps)

Logged in [`2026-09-19-two-use-cases.md`](journal/2026-09-19-two-use-cases.md), decided in [ADR-0001](adr/0001-route-a-tool-not-chat.md), refined in [`2026-09-19-who-is-this-for.md`](journal/2026-09-19-who-is-this-for.md).

The PoC already does both jobs: **summon a tool** and **park a starting point**. Then you tweak. Skill level changes which layer matters. Tag features. Do not collapse this into “pick A or B forever,” and do not build B because A-pro has no todo.

| Tag | Who | What | Now? |
|---|---|---|---|
| **A-now** | Me; anyone who can hear and will finish | Stock-rack summon + starting point + knobs | Yes — tool surface |
| **A-pro** | Mixer with a real workflow and favorite plugins | Hosted plugins, taste memory (“usually Mag EQ on this vocal”), later machine listening for *which* tool | Parked |
| **B-hands-off** | Never-mixed, or voice / walk-around | Follow-up chat, “too much,” describe until done | Filed, not queued |

**Still true:** chat is not the mixer. Starting points stay (they are already the recipes). A generic stock EQ is not a pro time-save.

**Feature tags:** every queued item gets `A-now` / `A-pro` / `B-hands-off`. After there is a real pile, you can cut a feature set per audience. Not before.

---

## Standing questions

Answer these before you start a feature, and again after you finish it. The delta is the post.

1. **Would I actually use this?** Not “would a beginner.” You. On a real vocal, in a real session, instead of grabbing FabFilter.
2. **What is the use case?** A concrete moment: track, time pressure, skill gap, what you would have done instead.
3. **Is this cheating?** Cheating at what — speed, taste, learning, craft, employment?
4. **Should this exist?** Who is helped, who is deskilled, what failure mode is unacceptable.
5. **What did I just give up?** Control, latency, trust, taste, offline use, DAW-native workflow.
6. **What would make me uninstall it?** Write this *before* you build the feature.

---

## This week

Saturday 2026-09-19 was a work day. Next check-in is ~2026-09-26. Goal for that check-in: one thing you can *demo* that makes the plugin feel more like something you would open, plus a point of view on the parked ideas (not a DAW prototype).

Do not start Stage 3+ until the Stage 2 gate is true.

End-of-day briefing (what shipped, Route A/B, papers, next): [`docs/journal/2026-09-19-session-notes.md`](journal/2026-09-19-session-notes.md). Write the human piece from that; do not treat it as the post.

### Landed today (not calling language V2 done)

1. **Open the rack** `A-now` — follow-ups edit the same tool; `parameter_overrides` can move freq, not only magnitude; rack strip + master A/B (no gain-match). Journal: [`2026-09-19-open-the-rack.md`](journal/2026-09-19-open-the-rack.md). Still needs a wider host test than “shine / cymbals.”
2. **Seek scene-graph** (not this repo) — `--mode scene` on one stereo file. Ears beat the graph (clap ≠ snare, hat ≠ shaker, no bass). Journal: [`2026-09-19-elements-and-ears.md`](journal/2026-09-19-elements-and-ears.md).

### Next (tomorrow-shaped)

1. **Synthetic language sweep** `A-now` — generate mix-language variants against known loops; log intent + override keys + freq. Only brighten/shine has been tried by hand. This is how we get confident enough to call language V2 done.
2. **More host tests** `A-now` — mud, harsh, warmth, width. Same session, same card when it is a follow-up.
3. **Tool surface** `A-now` — knobs open, A/B vs revert. [`.cursor/plans/route_a_tool_surface.plan.md`](../.cursor/plans/route_a_tool_surface.plan.md)
4. **Seek prompt cleanup** (seek) — stop feeding `bass_weight` into scene mode; force clap/hat/shaker discriminations. Then re-listen the same loop.
5. **See / hear** `A-now` — meters / spectrum — after the language tests, not instead of them.

Mix-deconstruct stays in seek. Do not start `A-pro` plugin hosting. Do not inject scene JSON into the plugin yet. Do not start Demucs / a 4B model.

Documentation:

- [x] Showing-it dump: `docs/journal/2026-09-19-after-the-demo.md`
- [x] Fork: `docs/journal/2026-09-19-two-use-cases.md`
- [x] Decision: `docs/journal/2026-09-19-picked-route-a.md` + [ADR-0001](adr/0001-route-a-tool-not-chat.md)
- [x] Audience: `docs/journal/2026-09-19-who-is-this-for.md`
- [x] Session briefing: `docs/journal/2026-09-19-session-notes.md`
- [x] Research table: [`docs/research/PAPERS.md`](research/PAPERS.md)
- [ ] Backfill PoC journal: `docs/journal/2026-03-12-poc.md`
- [ ] Draft essay 0: *Should this exist?* — this decision belongs in it.

### Parking lot (real, not this week)

Captured 2026-09-19. Do not start these before the Stage 2 gate.

| Idea | Why it is real | Why not now |
|---|---|---|
| Route B follow-up chat | Hands-off / voice / “too much” | Filed: `.cursor/plans/multi-turn_chat_context_febfcce8.plan.md`. ADR-0001. |
| Cache common intents / output | Latency is why chat feels unlike mixing | Polish after the tool surface exists |
| Save prefs + effect chains | Taste you can reuse | Stage 3 presets |
| Learn from keep vs undo | The interesting “copilot” — slides A toward B if we are sloppy | Needs history + a user who actually uses it |
| Taste as tool choice (past sessions, “Mag EQ on this vocal”) | Route A getting smarter once we host real plugins | Adjacent plan; do not start |
| User-hosted plugins | Capability registry / Stage 6 | Different product |
| Session context | Mixing is multiple tracks; one-track is already the wall | Get it via Ableton (SDK / MCP), not by becoming a DAW |
| Become a DAW / write on a DAW engine | Full context, “build the future” | I will not leave Ableton; collaborators will not switch |
| Ableton SDK / existing copilots | Stay where I already work | Research note, after the plugin is worth opening |
| Product vs OSS vs paid | Someone saw a company in this | Identity question; journal it, don’t incorporate it |
| Rack tool list (Logic-style names only) | See what is on the insert without opening cards | After follow-ups read as edits, not stacks |
| Sensor surface for an LM (session-scale listening) | The real bet — reason about the mix, don’t map synonyms | Stage 4. Lab in seek. Read pile: [`docs/research/PAPERS.md`](research/PAPERS.md). Do not start from a paper. |
| Granular “elements” inside one file (finer than Demucs) | “Air on the hats” needs a hat object, not a drums stem | Define element first. Prompt-conditioned localization is the cheap experiment. Journal: [`2026-09-19-elements-and-ears.md`](journal/2026-09-19-elements-and-ears.md) |

---

## Stages

### Stage 0 — Origin

**Status:** done (thesis written, thought piece not written)

**What it is:** “Cursor for mixing.” Intent → recipe → bounded DSP. User keeps the knobs. We are not generating music.

**Docs still owed:**
- Journal: the four questions *as of March 2026*
- Essay draft: `docs/essays/00-should-this-exist.md`

**Blog:** “I wanted Cursor for mixing — and immediately wondered if that was a bad idea.”

---

### Stage 1 — Proof of concept

**Status:** done (March 2026)

**Shipped:**
- JUCE plugin (AU / VST3 / Standalone) with React WebView UI
- OpenAI intent JSON → recipe engine → EQ / comp / sat / width
- Action cards, bypass, snapshot undo
- ~10 perceptual intents (`reduce_mud`, `brighten`, `add_warmth`, …)
- Debug AU validated (`auval -v aufx Chtr Chtr`, 2026-08-20)

**Honest status:** It proves the *interface*, not the *mix*. The original plan said DSP quality is not the point.

**Gate (already passed as a prototype, not as a tool you reach for):** a user can type “this vocal sounds muddy” and a knob moves.

**Journal to backfill:** Would you insert this on a real vocal today? If not, what is missing: sound quality, conversation, trust, or habit?

**Blog:** “The first version that actually moved a knob.”

---

### Stage 2 — Make it something you would open

**Status:** next

**Build:**
1. Continuous chat stream — one stream of reasoning + actions, not chat bubbles
2. Undo vs bypass clarity — undo reverts the action; bypass toggles on/off
3. Multi-level undo — knob tweaks in the same history as AI actions
4. Conversation context / “too much” / “more” — follow-ups that know what “it” is
5. Meters / spectrum / louder DSP — so you can see and hear that something happened

**Gate:** you sit down to mix something you care about and reach for Chatrack on purpose at least once.

**Journal focus:** *Would I use this?* and *What would make me uninstall it?*

**Blog:** “I built a mixing chatbot. I still opened Pro-Q.” (or the opposite, if it surprises you)

---

### Stage 3 — Make it conversational, not a command line

**Status:** later — after Stage 2 gate

**Build:**
1. Tool calling instead of “pick one intent from a list” — multi-intent, clarification tool, schema-bounded params
2. Multi-turn state — last action, current params, modify-last-action
3. Presets — “save this as vocal bright”

**Gate:** “actually smoother” and “brighten and add glue” both work without restating the whole request.

**Journal focus:** *Is this cheating?* You are no longer typing EQ settings. You are supervising.

**Blog:** “When the assistant does two things at once, who is mixing?”

---

### Stage 4 — Make it hear

**Status:** later — this is the actual product bet, not a bigger recipe list

The current LLM is blind. It maps words onto recipes. Code copilots work because the artifact is already text. Audio is signal; meaning is not sitting in the waveform as tokens. The job is a **sensor surface** (machine listening / audio-language embeddings / analysis) so a language model can *reason about a mix or a session*, not execute a synonym table.

We are not trying to “do the mix.” We are trying to get enough audio context that the model can talk the way a mixer talks. Journal: [`2026-09-19-ears-not-recipes.md`](journal/2026-09-19-ears-not-recipes.md).

**Build (when we get here, in order):**
1. Cheap sensors first: RMS / LUFS / band energy / crest / stereo correlation into context
2. Visual EQ / spectrum so *you* can see what it claims
3. Spectral / reference matching
4. Audio-language models and the Steinmetz/Reiss neighborhood (differentiable consoles, mix style transfer, Fx tool-calling). Read pile is in that journal. Do not start from CLAP because it is fashionable.

**Gate:** the assistant can disagree with you *from the signal*. “You said muddy; the buildup is at 180 Hz, not 280.”

**Journal focus:** *Should this exist?* A blind assistant is a preset picker. A hearing assistant is closer to a junior engineer.

**Blog:** “The model couldn’t hear the mix. Then it could. That’s when I got uncomfortable.”

---

### Stage 5 — Trust, craft, and the cheating question

**Status:** ongoing constraint, not a sprint

Keep tightening:
- Every action explainable, reversible, bounded
- A/B that is obvious
- No silent routing changes, no extreme gain
- Local / offline LLM (changes “should this exist” — cloud means mix taste leaves the machine)
- Proper DAW state / VST3 presets if you would actually leave it on a track
- Windows / WebView2 when you care about other people running it

**Journal focus:** moments the tool was *too confident*.

**Blog:** “I will let it cut mud. I will not let it finish the mix.” Draw that line in public and update it.

---

### Stage 6 — Beyond the rack

**Status:** parked until you would use Stage 2–3

Different product. Document that before building it.

- Plugin preference learning (your Pro-Q vs stock EQ)
- Capability registry and third-party plugin hosting
- Session context: masking between tracks, “make the vocal sit”
- Agentic multi-step plans with evaluate / rollback

**Blog:** “The day it wanted to touch the instrumental bed.”

---

### Fork — Generation (not a stage)

**Status:** identity question, not a build item

[Magenta Realtime 2.0](https://magenta.withgoogle.com/magenta-realtime-2) and anything like it **generate** audio. Chatrack **transforms** existing audio. Keep this in the journal as an open question.

**Essay:** *Should a mixing tool also invent the performance?*

---

## Blog series map

Write journal entries continuously. Promote to a post only at stage boundaries, when the questions have changed. One post per stage, after you have used that stage on real audio. No post that is only architecture.

| # | Stage | Working title | Core question |
|---|---|---|---|
| 0 | Origin | Should a mixing assistant exist? | Should this exist? |
| 1 | PoC | Cursor for mixing, inside a plugin | What is the use case? |
| 2 | Usable | I still opened Pro-Q | Would I use this? |
| 3 | Conversational | Supervising vs. mixing | Is this cheating? |
| 4 | Hearing | When the assistant could hear | Should this exist? (harder) |
| 5 | Line in the sand | What I will never let it do | Cheating + should this exist |
| 6 | Fork | Mixing is not generation | Should this exist? |

Technical posts (WebView bridge, recipe engine, real-time safety) are B-sides.

---

## Sources

Roadmap collected from:

- `natural_language_mixing_assistant_plan_v3.md` (thesis + future features)
- `docs/WORKLOG.md` (March 2026 next steps)
- May 2026 discussion: tool calling, mix-aware context, multi-turn, reference matching
- August 2026 Debug AU build / `auval` pass
