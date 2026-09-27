# 2026-09-19 — elements, not stems

**Stage:** 4 hearing (thought + Seek lab) / 2 usable (do not start Stage 4 in the plugin)
**Shipped this session:** Seek `--mode scene`; one live listen; a fact-check
**Publish later as:** maybe “The model said there was a bassline”

---

## Standing questions

### Would I actually use this?

I would use a copilot that can find the hats in a loop when I say “hats shine.” I would not use Demucs-on-drums as that answer. I would not block opening Chatrack on this working.

### What is the use case?

A stereo file with more than one object (kick, clap, hat, shaker). Language names an object. The system has to point at the most likely signal for that object — freq + time — so a later tool call is not a synonym. Sessions that are one source per track are the easy case. Loops and printed groups are the real case. We have to define **element** before we pick a separator.

### Is this cheating?

Localizing “hat” from the word *hat* plus the file is intern work. Pretending four Demucs stems are a kit is cheating at listening.

### Should this exist?

Yes as a Seek experiment and a read pile. No as a reason the plugin stays blind and inaudible.

### What did I just give up?

The hope that gpt-audio-1.5 (or 4o-audio before it) is a mix ear. It is a voice model. Also: one-source-per-track as a hidden assumption.

### What would make me uninstall it?

If we wait for perfect stem splitting to ship language that already works. If we inject a wrong scene graph into Chatrack and the shelf moves to a bass that is not there.

---

## What happened

- Built / decided / showed someone: scene mode in seek. Listen on a sample-pack drum loop. Graph: kick, snare, closed_hat 16ths, invented bass. Ears: kick, clap, hat on off-8ths, shaker ~16ths, no bass.
- Surprise: Layer 1 `bass_weight` (energy < 250 Hz) is kick energy. We handed it to the model.
- Evidence: `seek/tools/sonic-analysis/scene-test.json` (local); fact-check in chat 2026-09-19.

## Open threads

- Prompt-conditioned localization: the utterance names the object; find that signal. Measure it.
- Synthetic language sweep on Chatrack (tomorrow-shaped) is a different experiment — language → overrides, no ears required.
- Read [`docs/research/PAPERS.md`](../research/PAPERS.md). Cutting edge of *object-in-stereo* is the hole; Demucs/SPHERE/Diff-MST assume a coarser grain.
- Do not inject scene JSON into the plugin yet.
