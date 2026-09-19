#pragma once

#include <juce_core/juce_core.h>

void sessionLogEvent(const juce::String& kind, juce::DynamicObject::Ptr fields);
void sessionLogLine(const juce::String& markdownLine);
