# Research table

High-level index of papers we actually opened or pointed at. One or two sentences on *what the paper is doing*, not a review. Add rows when a new paper earns a place. Do not start a Chatrack feature from a paper.

Last updated: 2026-09-19.

| Paper | Link | What it is doing |
|---|---|---|
| **LLM2Fx-Tools** — Doh, Koo, Martínez-Ramírez et al. (ICLR 2026) | [arXiv:2512.01559](https://arxiv.org/abs/2512.01559) · [demo](https://seungheondoh.github.io/llm2fx-tools-demo/) | Multimodal LLM with tool calling: listen to audio (often dry/wet pairs), pick an Fx chain, order, and parameters, with CoT. Closest “tools + audio” paper. Job is reverse-engineering / style transfer, not “brighter hats on this insert.” They also ship LP-Fx and a Qwen3-4B-class setup. Do not train a 4B model because this exists. |
| **LLM2Fx** — Doh et al. (WASPAA 2025) | [SonyResearch/LLM2Fx](https://github.com/SonyResearch/LLM2Fx) | Earlier sibling: can an LLM predict effect parameters from *text* (zero-shot / ICL), no tool-calling audio loop. Language → knobs, still blind. |
| **MixAssist** — Clemens & Marasović (COLM 2025) | [arXiv:2507.06329](https://arxiv.org/abs/2507.06329) | Dataset of 431 audio-grounded turns from 7 live expert–amateur mixing sessions. Closest academic “talk about the mix while it plays.” Mentoring-in-session, not “deconstruct this famous record.” Their own eval: models are weak at detailed hearing in situ. |
| **ST-ITO** — Steinmetz, Singh, Comunità, Reiss et al. (ISMIR 2024) | [arXiv:2410.21233](https://arxiv.org/abs/2410.21233) | Style transfer by *searching* an effect chain at inference (gradient-free) against a learned production-style metric. Works on unseen / non-differentiable plugins. Not chat; not object detection. |
| **Diff-MST** — Vanka, Steinmetz, Reiss, Fazekas et al. (ISMIR 2024) | [arXiv:2407.08889](https://arxiv.org/abs/2407.08889) | Differentiable mixing console + transformer: raw tracks + a reference mix → interpretable gain/pan/EQ/comp. Needs stems. Style transfer, not “find the hat in a stereo loop.” |
| **SPHERE** — Guo, Murdock, Dixon, Reiss et al. (2026) | [arXiv:2608.30559](https://arxiv.org/abs/2608.30559) · [demo](https://sphere-music-binauralization.github.io/) | Audio-language model post-trained to predict *spatial* mix params (azimuth, elevation, gain) **from stems**, with mixing-heuristic rewards. Assumes the objects are already separated. |
| **HT Demucs** — Rouard, Massa, Défossez (ICASSP 2023) | [arXiv:2211.08553](https://arxiv.org/abs/2211.08553) · [code](https://github.com/adefossez/demucs) | Music source separation into drums / bass / vocals / other. On a drum loop this just says “drums.” Not hat-vs-shaker. Hybrid Demucs predecessor: [arXiv:2111.03600](https://arxiv.org/abs/2111.03600). |
| **Magenta Realtime 2.0** | [magenta.withgoogle.com](https://magenta.withgoogle.com/magenta-realtime-2) | Generates audio. Chatrack transforms existing audio. Keep as a fork, not a stage. |

Steinmetz’s site is a useful index of the neighborhood: [christiansteinmetz.com](https://www.christiansteinmetz.com/).

## How this maps onto us

- **Chatrack today** is LLM2Fx-the-first-paper: language → a tool call, no ears. V2 is making that language land on real parameters (`parameter_overrides`), not a bigger synonym table.
- **Seek scene mode** is the intern listen: one stereo file → named objects. gpt-audio-1.5 is a *voice* model (successor to `gpt-4o-audio-preview`, retired 2026-05-07). Same family as Advanced Voice, not a mix deconstructor.
- **Demucs / SPHERE / Diff-MST** all get more useful once you already have *elements*. A loop with kick + clap + hat + shaker is one Demucs stem. The missing research is: what is an element, and can you localize it in a stereo bounce without assuming one source per track.
- **MixAssist** is the conversation shape (mentor in the session). We are not building that in the plugin.

Journal that named the bet: [`docs/journal/2026-09-19-ears-not-recipes.md`](../journal/2026-09-19-ears-not-recipes.md). Element / experiment thread: [`docs/journal/2026-09-19-elements-and-ears.md`](../journal/2026-09-19-elements-and-ears.md).
