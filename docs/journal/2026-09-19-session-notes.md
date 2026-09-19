# 2026-09-19 — session notes (for a human write-up)

**Stage:** 2 usable, with Stage 4 thought and a Seek side experiment
**Shipped this session:** follow-ups + overrides + rack A/B (plugin); scene-graph mode (seek)
**Publish later as:** source notes — write the actual piece yourself. Do not treat this file as the post.

This is a briefing: what we did, what we argued, what we think is true, what is next. Journals with the standing-question format are linked under each thread. Use those when a question actually changed.

---

## What we built

### Chatrack (this repo, branch `feature/2026-09-19-open-the-rack`)

- Follow-up chat that *edits the same tool* instead of stacking cards. Still Route A: last move is a knob.
- Prompt + parser can set real params (`parameter_overrides`), not only magnitude. “Shine / air / cymbals” should move `eq.band3.freq` into 10–16 kHz instead of adding 0.25 dB at 7 kHz.
- Persistent rack strip + master bypass so cards can scroll away and you can still A/B the whole chain. Master A/B is **not** gain-matched (dry is quieter; bypass can lie).
- Host launch after every build (`scripts/run-host.sh` + `chatrack.filtergraph`). `resources/test-loop.wav` is clicks + 8 kHz, not a kit.
- API key lives at `~/Library/Chatrack/config.json` (README used to say Application Support; that folder was empty).

**Not done:** calling the language/override path “V2 shipped.” One phrase family (brighten / shine / cymbals). More tests needed, ideally automated.

### Seek (`seek/tools/sonic-analysis`, not this plugin)

- `--mode scene`: one file → objects with name, time role, freq band. A&R mode stays the default.
- Live listen on `FSS_DPHE2_126_drum_full_loop_pass.wav` (sample-pack loop in the lounge ting Ableton project). Layer 2 had to move from dead `gpt-4o-audio-preview` to `gpt-audio-1.5`.
- Your ears vs the graph: kick yes; backbeat is a clap not a snare; closed hat on off-8ths; shaker almost 16ths; **no bass**. The model invented bass and collapsed hat + shaker. Schema already had `clap` and `shaker`.
- Leak we can fix without a new model: Layer 1 `bass_weight` is *all energy below 250 Hz*. A kick-only loop scores 0.94. Scene mode was told to “use the numbers.”

Mix-deconstruct / famous-record essays / Suno recreation: not worth another lap. The intern-quality listen is the finding.

---

## Conversations and decisions (the actual threads)

### Route A vs Route B

We almost built “too much / more / actually warmer” as if that were the product. It is a different product.

| | Job | Who | Status |
|---|---|---|---|
| **A-now** | Summon a stock tool, park a starting point, finish on knobs | You; anyone who can hear | Building |
| **A-pro** | *Which* plugin (“Mag EQ on this vocal”) + listening for decisions | Someone with a real workflow | Parked — needs hosting |
| **B-hands-off** | Chat / voice until it sounds right | Never-mixed, or walking around | Filed, not queued |

ADR: [`docs/adr/0001-route-a-tool-not-chat.md`](../adr/0001-route-a-tool-not-chat.md). Journals: [`two-use-cases`](2026-09-19-two-use-cases.md), [`picked-route-a`](2026-09-19-picked-route-a.md), [`who-is-this-for`](2026-09-19-who-is-this-for.md).

Clarified the same day: A and B are **layers**, not two apps. The PoC already summons (A) and parks a start (proto-B). Skill level changes which layer matters. Do not build B because A-pro has no todo.

### Recipes are a synonym table

“Cymbals brighter” that does nothing audible is not a prompt bug first — it is mapping language onto a fixed recipe. The interesting product is a **sensor surface**, not a bigger `recipes.json`. That is already Stage 4. Journal: [`ears-not-recipes`](2026-09-19-ears-not-recipes.md).

Do not put that sensor in the plugin first. Lab it in seek.

### What “hear the hats” would actually take

In a perfect world the model finds objects finer than Demucs stems (drums/bass/vox/other). Then “add air on the hats” has a home.

A cheaper experiment, worth running: the *prompt names the object*. “Hats to shine” is evidence hats exist. Task becomes: most likely hat-like signal in this file — not “caption a kit from a house-music prior.”

You cannot assume one sound source per track. Loops lie. Sessions with one instrument per track are the easy case. We still have to **define element**: a source you would solo, with a time role and a spectral home. Until that definition is operational, stem splitting and chat are both stuck at the wrong grain.

Machine listening / audio LMs are bumping the wall we hit today (clap vs snare, hat vs shaker, invented bass). Research table: [`docs/research/PAPERS.md`](../research/PAPERS.md).

### LLM2Fx-Tools vs Chatrack

Closest paper. Different job. They reverse-engineer a reference (dry/wet or style transfer) with tools + CoT + an audio encoder. We summon a tool on an insert the user finishes by ear. Their CoT is post-hoc of sampled chains. Do not train their 4B. Do not copy their product.

### Mix-deconstruct as a side product

ChatGPT-web on *Start The Fire* (stereo, not stems) was intern-quality: useful, one pitch miss (last bass note is C, not G). MixAssist is the academic neighbor (mentor in a live session), not “explain this record.” Analysis engine in seek; DAW chat is last mile. No Demucs in JUCE.

---

## Insights worth keeping (if you only steal bullets)

- Typing was supposed to replace **tool hunting**, not knob tweaking.
- Starting points are allowed. Chat as the mixer is not.
- A generic stock EQ does not save a Pro-Q person time. A-pro is hosting + taste-as-tool-choice.
- Intelligence does not live in the prompt. The prompt is a switch statement with extra steps.
- `bass_weight` ≠ bassline. Whole-file energy ratios will gaslight a scene graph.
- Voice audio models (gpt-audio-1.5) are not mix ears. The API switch was a retirement, not an upgrade to listening.
- Demucs on a kit loop says “drums.” SPHERE/Diff-MST want stems. The hole is *elements inside one file*.
- Master bypass without gain-match will fool you.
- Do not wait on listening research to make the plugin something you would open.

---

## Where we think we can go next

Preview, not a new stage. Full queue: [`docs/ROADMAP.md`](../ROADMAP.md).

**Tomorrow-shaped (Chatrack V2 language, still A-now)**

1. More host tests on overrides. Not one phrase. Mud, harsh, warmth, width, “too much” as a *knob* move if it even comes up.
2. **Synthetic language sweep** — generate variants (“hats shine,” “air on the tops,” “clap forward,” “less boxy”) against known loops; log intent + override keys + freq. Automate what we only did by hand for brighten. You were interested in this for tomorrow.
3. Tool surface still queued: knobs open, A/B vs revert. [`.cursor/plans/route_a_tool_surface.plan.md`](../../.cursor/plans/route_a_tool_surface.plan.md).

**Seek / research (not the plugin)**

4. Tighten scene prompt: stop feeding A&R scalars; force clap vs snare, hat vs shaker; no bass unless something pitches under the kick. Re-listen the same loop.
5. Read the table. Question to keep in mind: who actually localizes *objects in a stereo file*, not stems, not style transfer.
6. Define “element” in a paragraph you would defend. Then decide if the next experiment is prompt-conditioned localization, better DSP sensors, or a paper we have not filed yet.

**Do not start**

Demucs in Chatrack. Training a 4B. Mix-deconstruct as a product. Route B follow-up chat. Hosted plugins. Gain-match (note only). Magenta / generation.

---

## Files to write from

| If you want to write about… | Start here |
|---|---|
| Should this exist / use case | [`after-the-demo`](2026-09-19-after-the-demo.md), essay 0 still owed |
| Tool vs chat mixer | ADR-0001 + the three Route A journals |
| Blind recipes vs hearing | [`ears-not-recipes`](2026-09-19-ears-not-recipes.md) |
| Overrides / open the rack | [`open-the-rack`](2026-09-19-open-the-rack.md) |
| Elements, stems, experiments | [`elements-and-ears`](2026-09-19-elements-and-ears.md) |
| Papers | [`docs/research/PAPERS.md`](../research/PAPERS.md) |
| What shipped, technically | [`docs/WORKLOG.md`](../WORKLOG.md) |
