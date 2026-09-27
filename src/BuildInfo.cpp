#include "BuildInfo.h"

#ifndef CHATRACK_VERSION
#define CHATRACK_VERSION "0.1.0"
#endif

juce::String getChatrackVersion()
{
    return CHATRACK_VERSION;
}

juce::String getChatrackBuildStamp()
{
    // __DATE__ / __TIME__ refresh whenever this file is recompiled.
    return juce::String(__DATE__) + " " + juce::String(__TIME__);
}

juce::String getChatrackBuildLabel()
{
#if JUCE_DEBUG
    const char* config = "debug";
#else
    const char* config = "release";
#endif
    return getChatrackVersion() + " " + config + " · " + getChatrackBuildStamp();
}
