#include "SessionLog.h"
#include "BuildInfo.h"

namespace
{
    juce::File logDirectory()
    {
#ifdef CHATRACK_LOG_DIR
        juce::File dir(CHATRACK_LOG_DIR);
#else
        juce::File dir = juce::File::getSpecialLocation(
            juce::File::userApplicationDataDirectory)
            .getChildFile("Chatrack")
            .getChildFile("logs");
#endif
        dir.createDirectory();
        return dir;
    }

    juce::String timestamp()
    {
        return juce::Time::getCurrentTime().formatted("%H:%M:%S");
    }

    juce::String dateStamp()
    {
        return juce::Time::getCurrentTime().formatted("%Y-%m-%d");
    }

    void appendFile(const juce::File& file, const juce::String& text)
    {
        file.appendText(text, false, false, "\n");
    }
}

void sessionLogEvent(const juce::String& kind, juce::DynamicObject::Ptr fields)
{
    if (fields == nullptr)
        fields = new juce::DynamicObject();

    fields->setProperty("kind", kind);
    fields->setProperty("time", timestamp());
    fields->setProperty("build", getChatrackBuildLabel());

    const auto dir = logDirectory();
    appendFile(dir.getChildFile(dateStamp() + ".jsonl"),
               juce::JSON::toString(juce::var(fields.get()), true) + "\n");
}

void sessionLogLine(const juce::String& markdownLine)
{
    const auto dir = logDirectory();
    auto latest = dir.getChildFile("latest.md");

    if (!latest.existsAsFile())
    {
        appendFile(latest, "# Chatrack session log\n\n"
                           "Readable dump of chat + actions. JSONL is the same day file.\n\n");
    }

    appendFile(latest, markdownLine.endsWith("\n") ? markdownLine : markdownLine + "\n");
}
