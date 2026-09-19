# 2026-09-19 — ears, not recipes

**Stage:** 4 hearing (thought only) / 2 usable (do not start Stage 4)
**Shipped this session:** no code. Named the actual problem.
**Publish later as:** “The model is playing dumb on purpose — we never gave it audio”

---

## Standing questions

### Would I actually use this?

I would use a copilot that can reason about a *session* the way we reason about this repo. I am not sure I would use a better recipe mapper, even a polished one. The current LLM does not know what it is doing. It maps my words onto known moves.

### What is the use case?

Not “do the mix.” Get a model enough audio context that it can talk about the mix at a pro-audio level: masking, pocket, brightness that is actually on the tape, not brightness as a synonym for `eq.band3`. Code works with LLMs because it is already text. Audio is signal. The meaning is not sitting in the waveform as tokens. The interesting product is the **sensor surface** — machine listening, embeddings, analysis — so a language model can reason *over* a mix, not over a vocabulary list.

That is session-scale. One-track recipes are the tutorial.

### Is this cheating?

Reasoning with ears is closer to a junior engineer. Mapping “brighter” to +3 dB is a preset. The first one scares me more and is the one I would respect.

### Should this exist?

If the model can hear and explain, and I still finish: yes. If it silently “fixes” the mix from a black box: no. The LA2A-modeling era (2023 master’s) was “can a net become a device.” This is “can a net become a listening intern.” Different object.

### What did I just give up?

The fantasy that Chatrack’s intelligence lives in the prompt. It does not. The prompt is a switch statement with extra steps.

### What would make me uninstall it?

If we spend a year on chat follow-ups and never give the model audio. If we jump to listening research and leave an inaudible plugin.

---

## What happened

- Built / decided / showed someone: aside while polishing follow-ups. The real dub is not mixing. It is mix *context* for an LM. Steinmetz / Reiss-line work is the research neighborhood (differentiable consoles, mix style transfer, now audio-language models + tool calling). Do not start it today.
- Surprise: we already wrote Stage 4 as “make it hear.” This is that, said honestly. Recipes are Stage 1.
- Evidence: none. The evidence would be the model disagreeing with me from the signal.

## Open threads

- Finish follow-up polish (audible edit, not stack). Then stop and use it.
- Stage 4 is this: analysis → language → reason. Not a bigger recipe list.
- Pointers to read later, not build: table in [`docs/research/PAPERS.md`](../research/PAPERS.md) (ST-ITO, Diff-MST, LLM2Fx-Tools, MixAssist, SPHERE, Demucs). Steinmetz site: [christiansteinmetz.com](https://www.christiansteinmetz.com/).
- Session context without a new DAW still points at Ableton SDK / stems, plus listening. Two halves of the same problem.
