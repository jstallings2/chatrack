
# Natural Language Mixing Assistant
### Hackathon → MVP Implementation Plan (v3, JUCE Architecture)

Author: Updated from product exploration discussion  
Goal: Build a **conversational interface for mixing** that translates producer intent into mixing actions with inline parameter control.

This version updates the architecture to **JUCE**, removing the Bitwig dependency and focusing on a **plugin-based Mix Rack** that works inside any DAW.

---

# 1. Product Thesis

**Natural language becomes the interface to mixing.**

We are **not generating music**.

We are translating:

producer intent  
↓  
mix concepts  
↓  
tool selection  
↓  
plugin parameter actions  

Example:

User:

> this vocal sounds muddy

Assistant:

Action: Reduce Mud  

✓ Applied low‑mid EQ cut  
✓ Centered around ~280 Hz  

[Apply] [Undo]

▼ Adjust parameters

Frequency  
Gain  
Q  

Key principles:

- AI suggests actions
- User remains in control
- Every change is reversible
- Knobs are always accessible
- Existing DAW workflow remains intact

Think **Cursor for mixing**, but inside a plugin.

---

# 2. Key Product Insight

The value is **not controlling a single tool**.

The value is **interpreting intent and choosing the correct tool**.

Example:

User intent: "make this warmer"

Possible tools:

EQ  
Saturation  
Compression  
Tape / harmonic enhancement  

The assistant chooses the best action.

---

# 3. MVP Strategy

Avoid two traps:

❌ Rebuilding a full DAW  
❌ Deep DAW integration

Instead build a **JUCE plugin that acts as a Mix Rack**.

Architecture:

Assistant  
↓  
Mix Rack Plugin (JUCE)  
↓  
Built‑in processors

This proves the **semantic interpretation layer** while keeping engineering manageable.

---

# 4. Core Architecture

Chat UI  
↓  
Intent Parser (LLM)  
↓  
Mix Ontology  
↓  
Action Planner  
↓  
Recipe Engine  
↓  
Rack Controller  
↓  
JUCE Mix Rack Plugin

---

# 5. JUCE Plugin Architecture

Plugin Name: **SemanticMixRack** (working name)

JUCE components:

AudioProcessor  
AudioProcessorEditor  
AudioProcessorValueTreeState  
DSP modules for internal processing

The rack contains several internal processors.

---

# 6. Rack Processors (MVP)

Keep this extremely small.

Processors:

EQ  
Saturation  
Compression  
Stereo Width

These can be implemented using JUCE DSP modules or simple custom processors.

Purpose is **not high‑end DSP**, only proving the interface.

---

# 7. Golden Demo Flow

User has plugin inserted on a vocal track.

User types:

> this vocal sounds muddy

Assistant:

Action: Reduce Mud

✓ Applied low‑mid EQ cut  
✓ Targeted buildup around 280 Hz  

[Apply] [Undo]

▼ Adjust parameters

Frequency: 280 Hz  
Gain: −2.5 dB  
Q: 1.2  

User then types:

> actually make it a bit warmer

Assistant:

Action: Add Warmth

✓ Added subtle saturation  
✓ Slight harmonic thickening

User tweaks drive knob manually.

---

# 8. Core Interaction Loop

describe sound  
↓  
assistant interprets intent  
↓  
assistant selects tool  
↓  
assistant adjusts parameters  
↓  
user tweaks knobs

Typing replaces **tool hunting**, not **knob tweaking**.

---

# 9. Core User Interface

Plugin UI includes:

Chat panel  
Action cards  
Apply / Undo controls  
Accordion parameter controls

---

## Action Card Anatomy

Action Title

✓ Summary bullet  
✓ Summary bullet  

[Apply] [Undo]

▼ Adjust parameters

Knob  
Knob  
Knob  

Every action must be:

Explainable  
Reversible  
Adjustable

---

# 10. Progressive Control Model

Layer 1 — Intent

User describes sound.

Example:

"brighten slightly"

Layer 2 — Inline Controls

Assistant exposes parameters.

Example:

High Shelf Gain  
Shelf Frequency  
Saturation Drive  

Layer 3 — Full Processor UI

Optional deeper access.

---

# 11. Mix Ontology

Define small perceptual vocabulary.

Examples:

mud  
brightness  
warmth  
harshness  
punch  
width  

Each maps to a **recipe**.

---

# 12. Recipe Engine

Recipes translate concepts to actions.

Example:

reduce_mud

Steps:

1 locate EQ processor  
2 create bell band  
3 center ~250‑350 Hz  
4 reduce gain −2 dB  

Controls:

Frequency  
Gain  
Q

---

# 13. Initial Command Set (Hackathon)

Support ~10 intents.

reduce mud  
brighten slightly  
add warmth  
tame harshness  
make more forward  
widen slightly  
add subtle saturation  
glue slightly  
make punchier  
undo

---

# 14. Parameter Control UI

Accordion reveals parameters derived from recipe.

Examples:

Frequency  
Gain  
Q  
Drive  
Threshold  
Width

Knobs update processor state in real time.

---

# 15. Follow‑Up Prompt Handling

Follow‑ups modify the **last action**.

Example:

User:

brighten slightly

Then:

actually smoother

Assistant:

Reduce shelf gain  
Increase saturation slightly

---

# 16. Undo System

Every action records parameter snapshots.

parameter_id  
previous_value  
new_value  

Undo restores previous state.

---

# 17. Technical Stack

Language: C++

Framework: JUCE

Frontend UI:

JUCE Components

Chat panel  
Action card component  
Knob component  
Accordion controls

---

Backend modules:

Intent parser (LLM API)  
Mix ontology  
Recipe engine  
Action history  
Rack controller

---

LLM Responsibilities:

intent classification  
language normalization  
assistant response phrasing

LLM **never manipulates parameters directly**.

---

# 18. Data Model

Intent

{
intent: "reduce_mud",
target: "selected_track",
magnitude: "small"
}

---

Action Card

{
title: "Reduce Mud",
summary: [
"Applied low-mid EQ cut",
"Targeted buildup around 280 Hz"
],
controls: [
{ label: "Frequency", value: 280 },
{ label: "Gain", value: -2.5 }
]
}

---

Undo Snapshot

{
parameter_id: "eq.band1.gain",
before: 0,
after: -2.5
}

---

# 19. Future Features

Plugin Preference Learning

Store user preferences.

Example:

preferred EQ: Pro-Q  
preferred saturation: Decapitator

---

Plugin Capability Registry

Plugins register capabilities:

EQ  
compression  
saturation  
reverb

Assistant selects the best provider.

---

Third‑Party Plugin Hosting

Later versions may allow the rack to host external plugins.

---

Session Context

Future assistant may inspect mix relationships between tracks.

---

# 20. Safety Rules

Assistant must never:

destroy routing  
remove processors silently  
apply extreme gain changes  
modify entire session

All actions must be:

reversible  
visible  
bounded

Assistant behaves like a **careful junior mix engineer**.

---

# 21. Product Vision

The system becomes a **semantic control layer for music production**.

producer language  
↓  
mix concepts  
↓  
tool selection  
↓  
plugin parameter control

Long‑term:

interface → skills → context awareness → personalization → DAW assistant

---

# 22. Success Metric (Hackathon)

If producers say:

"I can tell the plugin what I want and tweak the exact knobs without hunting for tools."

The prototype succeeded.
