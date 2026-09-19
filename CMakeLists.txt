cmake_minimum_required(VERSION 3.22)
project(Chatrack VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(libs/juce)

juce_add_plugin(Chatrack
    PRODUCT_NAME "Chatrack"
    COMPANY_NAME "Chatrack"
    BUNDLE_ID "com.chatrack.chatrack"
    PLUGIN_MANUFACTURER_CODE Chtr
    PLUGIN_CODE Chtr
    FORMATS AU VST3 Standalone
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS TRUE
    NEEDS_WEBVIEW2 TRUE
    COPY_PLUGIN_AFTER_BUILD TRUE
)

target_sources(Chatrack
    PRIVATE
        src/BuildInfo.cpp
        src/PluginProcessor.cpp
        src/PluginEditor.cpp
        src/dsp/RackProcessor.cpp
        src/dsp/EQProcessor.cpp
        src/dsp/CompressorProcessor.cpp
        src/dsp/SaturationProcessor.cpp
        src/dsp/StereoWidthProcessor.cpp
        src/engine/RecipeEngine.cpp
        src/engine/MixOntology.cpp
        src/engine/ActionHistory.cpp
        src/engine/RackController.cpp
        src/llm/CloudLLMProvider.cpp
        src/llm/IntentParser.cpp
        src/SessionLog.cpp
)

target_compile_definitions(Chatrack
    PUBLIC
        JUCE_WEB_BROWSER=1
        JUCE_USE_WIN_WEBVIEW2_WITH_STATIC_LINKING=1
        JUCE_VST3_CAN_REPLACE_VST2=0
        JUCE_USE_CURL=1
        JUCE_DISPLAY_SPLASH_SCREEN=0
    PRIVATE
        CHATRACK_VERSION="${PROJECT_VERSION}"
        CHATRACK_LOG_DIR="${CMAKE_SOURCE_DIR}/logs"
)

# Recompile BuildInfo.cpp every build so the header stamp is trustworthy.
add_custom_target(chatrack_touch_buildinfo
    COMMAND ${CMAKE_COMMAND} -E touch "${CMAKE_CURRENT_SOURCE_DIR}/src/BuildInfo.cpp"
    COMMENT "Refreshing build stamp"
)
add_dependencies(Chatrack chatrack_touch_buildinfo)

add_custom_target(run_host
    COMMAND "${CMAKE_SOURCE_DIR}/scripts/launch-host.sh"
    COMMENT "Open AudioPluginHost with chatrack.filtergraph"
    USES_TERMINAL
)
add_dependencies(run_host Chatrack_AU Chatrack_VST3)

target_include_directories(Chatrack
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(Chatrack
    PRIVATE
        juce::juce_audio_utils
        juce::juce_audio_processors
        juce::juce_dsp
        juce::juce_gui_extra
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
)
