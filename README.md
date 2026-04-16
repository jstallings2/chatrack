# Chatrack — Real-Time C++ Audio Plugin with LLM-Driven Control

Chatrack is a C++ audio plugin (JUCE) that combines real-time audio processing with a natural language interface. It translates user intent into parameter changes across a DSP processing chain, while maintaining low-latency and thread-safe operation.

## Key Technical Highlights

- **Real-time C++ system (JUCE)** with low-latency audio processing constraints  
- **Thread-safe architecture** separating audio processing and UI/LLM interaction  
- **Lock-free parameter updates** between control layer and DSP chain  
- **Modular DSP pipeline** (EQ, compression, saturation, stereo processing)  
- **C++ ↔ JavaScript bridge** via embedded WebView (React UI)  
- **LLM-driven control layer** translating natural language into deterministic parameter updates  

## Architecture Overview

- DSP Chain: Real-time audio processing (EQ → Compressor → Saturation → Width)  
- Control Layer: Maps intent → parameter changes (RecipeEngine)  
- Interface Layer: WebView UI communicates with C++ backend  
- Threading: Audio thread isolated from UI + network operations  

> Note: Designed with emphasis on real-time safety, separation of concerns, and extensibility.

### Example

> **You:** "This vocal sounds muddy"
>
> **Chatrack:** Applied a low-mid EQ cut around 280 Hz. [Undo] [Adjust Parameters]

## Features

- **Natural language interface** — Describe your intent, not the tools
- **Built-in DSP processors** — EQ (4-band), Compressor, Saturation, Stereo Width
- **Action cards** — Every change is explainable, reversible, and adjustable
- **Inline knob controls** — Expand action cards to fine-tune parameters
- **Undo system** — Full history of changes with one-click undo

## Supported Intents

- `reduce mud` — Cut low-mid frequencies (200-400Hz)
- `brighten` / `add air` — Boost high frequencies
- `add warmth` — Subtle saturation for analog warmth
- `tame harshness` — Cut harsh upper-mid frequencies
- `make more forward` — Boost presence
- `widen` / `narrow` — Adjust stereo width
- `add saturation` — Harmonic distortion
- `glue` / `compress` — Dynamic control
- `make punchier` — Enhance transients

## Requirements

- macOS 11+ (for WebView support)
- CMake 3.22+
- Xcode 14+ (for building)
- Node.js 18+ (for UI development)
- OpenAI API key (for LLM features)

## Building

### 1. Clone with submodules

```bash
git clone --recursive https://github.com/youruser/chatrack.git
cd chatrack
```

Or if already cloned:

```bash
git submodule update --init --recursive
```

### 2. Build the plugin

```bash
mkdir build && cd build
cmake .. -GXcode
cmake --build . --config Release
```

The plugin will be installed to:
- AU: `~/Library/Audio/Plug-Ins/Components/Chatrack.component`
- VST3: `~/Library/Audio/Plug-Ins/VST3/Chatrack.vst3`

### 3. Build the UI (for development)

```bash
cd ui
npm install
npm run dev
```

The dev server runs at `http://localhost:5173`. In Debug builds, the plugin loads from this server for hot reloading.

### 4. Deploy UI for release

```bash
cd ui
npm run deploy
```

This builds and zips the UI into `resources/` for embedding in release builds.

## Configuration

1. Open the plugin in your DAW
2. Click the Settings (gear) icon
3. Enter your OpenAI API key
4. The key is stored locally in `~/Library/Application Support/Chatrack/config.json`

## Architecture

```
┌─────────────────────────────────────────────────┐
│                 JUCE Plugin                      │
│  ┌───────────────────────────────────────────┐  │
│  │           WebView (React UI)               │  │
│  │  Chat Panel → Action Cards → Knob Controls │  │
│  └────────────────────┬──────────────────────┘  │
│                       │ JS ↔ C++ Bridge         │
│  ┌────────────────────┴──────────────────────┐  │
│  │              Plugin Backend                │  │
│  │  LLMService → RecipeEngine → RackController│  │
│  └────────────────────┬──────────────────────┘  │
│                       │ lock-free params        │
│  ┌────────────────────┴──────────────────────┐  │
│  │              DSP Chain                     │  │
│  │     EQ → Compressor → Saturation → Width  │  │
│  └───────────────────────────────────────────┘  │
└─────────────────────────────────────────────────┘
```

## Project Structure

```
chatrack/
├── CMakeLists.txt           # Build configuration
├── libs/juce/               # JUCE framework (submodule)
├── src/
│   ├── PluginProcessor.*    # Audio processor, parameter layout
│   ├── PluginEditor.*       # WebView host, native functions
│   ├── dsp/                 # DSP processors
│   ├── engine/              # Recipe engine, undo system
│   └── llm/                 # LLM service, intent parsing
├── ui/                      # React + Tailwind WebView UI
│   ├── src/
│   │   ├── components/      # Chat, ActionCard, Knobs
│   │   └── juce/           # JUCE JS bridge
│   └── package.json
└── resources/
    └── recipes.json         # Intent → parameter mappings
```

## Development

### Running in Debug

1. Start the UI dev server: `cd ui && npm run dev`
2. Build the plugin in Debug: `cmake --build build --config Debug`
3. Load the plugin in your DAW
4. Edit React components—changes hot reload in the plugin

### Adding a new intent

1. Add the recipe to `src/engine/RecipeEngine.cpp` and `resources/recipes.json`
2. Update the system prompt in `src/llm/CloudLLMProvider.cpp`
3. Rebuild the plugin

## License

[Add your license here]

## Credits

- Built with [JUCE](https://juce.com)
- UI powered by React + Tailwind CSS
- LLM integration via OpenAI API
