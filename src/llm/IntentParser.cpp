#include "IntentParser.h"

#include <cmath>

namespace
{
    bool isLetter(juce::juce_wchar c)
    {
        return juce::CharacterFunctions::isLetter(c);
    }

    bool isDigit(juce::juce_wchar c)
    {
        return juce::CharacterFunctions::isDigit(c);
    }

    int skipSpaces(const juce::String& s, int i)
    {
        while (i < s.length() && (s[i] == ' ' || s[i] == '\t'))
            ++i;
        return i;
    }

    int skipNumber(const juce::String& s, int i)
    {
        bool sawDot = false;
        while (i < s.length())
        {
            const auto c = s[i];
            if (isDigit(c))
                ++i;
            else if (c == '.' && !sawDot)
            {
                sawDot = true;
                ++i;
            }
            else
                break;
        }
        return i;
    }
}

juce::String IntentParser::sanitizeDashes(const juce::String& text)
{
    return text.replaceCharacter(juce::juce_wchar(0x2013), '-')
               .replaceCharacter(juce::juce_wchar(0x2014), '-')
               .replaceCharacter(juce::juce_wchar(0x2011), '-')
               .replaceCharacter(juce::juce_wchar(0x2212), '-');
}

juce::String IntentParser::unwrapJsonPayload(const juce::String& raw)
{
    auto text = sanitizeDashes(raw).trim();

    if (text.startsWith("```"))
    {
        const auto firstNl = text.indexOfChar('\n');
        if (firstNl >= 0)
            text = text.substring(firstNl + 1);
        const auto fence = text.lastIndexOf("```");
        if (fence >= 0)
            text = text.substring(0, fence);
        text = text.trim();
    }

    const auto start = text.indexOfChar('{');
    const auto end = text.lastIndexOfChar('}');
    if (start >= 0 && end > start)
        text = text.substring(start, end + 1);

    return text;
}

juce::String IntentParser::normalizeOverrideKey(const juce::String& key)
{
    auto k = key.trim();
    if (k.endsWithIgnoreCase(".frequency"))
        k = k.dropLastCharacters(juce::String(".frequency").length()) + ".freq";
    else if (k.equalsIgnoreCase("frequency") || k.equalsIgnoreCase("freq"))
        k = "eq.band3.freq";
    return k;
}

float IntentParser::parseNumericVar(const juce::var& value, bool& ok)
{
    ok = false;

    if (value.isVoid() || value.isUndefined())
        return 0.0f;

    if (value.isString())
    {
        auto s = sanitizeDashes(value.toString()).trim();
        if (s.isEmpty() || s.equalsIgnoreCase("null"))
            return 0.0f;

        const bool khz = s.containsIgnoreCase("khz")
            || s.endsWithIgnoreCase("k");
        const auto numeric = s.retainCharacters("0123456789.-");
        if (numeric.isEmpty())
            return 0.0f;

        float parsed = numeric.getFloatValue();
        if (khz)
            parsed *= 1000.0f;

        ok = true;
        return parsed;
    }

    if (value.isInt() || value.isInt64() || value.isDouble() || value.isBool())
    {
        ok = true;
        return static_cast<float>(value);
    }

    return 0.0f;
}

void IntentParser::readParameterOverrides(const juce::var& overridesVar,
                                          std::map<juce::String, float>& out)
{
    juce::var obj = overridesVar;

    if (obj.isString())
    {
        const auto inner = juce::JSON::parse(unwrapJsonPayload(obj.toString()));
        if (inner.isVoid() || inner.isUndefined())
            return;
        obj = inner;
    }

    if (auto* dyn = obj.getDynamicObject())
    {
        for (const auto& prop : dyn->getProperties())
        {
            bool ok = false;
            const float value = parseNumericVar(prop.value, ok);
            if (!ok)
                continue;

            const auto key = normalizeOverrideKey(prop.name.toString());
            if (key.isNotEmpty())
                out[key] = value;
        }
        return;
    }

    if (auto* arr = obj.getArray())
    {
        for (const auto& item : *arr)
        {
            auto* row = item.getDynamicObject();
            if (row == nullptr)
                continue;

            juce::String key;
            if (row->hasProperty("id"))
                key = row->getProperty("id").toString();
            else if (row->hasProperty("param"))
                key = row->getProperty("param").toString();
            else if (row->hasProperty("name"))
                key = row->getProperty("name").toString();

            const juce::var rawValue = row->hasProperty("value") ? row->getProperty("value")
                                 : row->hasProperty("hz")    ? row->getProperty("hz")
                                                             : juce::var();
            bool ok = false;
            const float value = parseNumericVar(rawValue, ok);
            key = normalizeOverrideKey(key);
            if (ok && key.isNotEmpty())
                out[key] = value;
        }
    }
}

std::optional<float> IntentParser::extractFrequencyHz(const juce::String& text)
{
    const auto s = sanitizeDashes(text);
    int i = 0;

    while (i < s.length())
    {
        if (!isDigit(s[i]))
        {
            ++i;
            continue;
        }

        const int start = i;
        i = skipNumber(s, i);
        const float first = s.substring(start, i).getFloatValue();

        int j = skipSpaces(s, i);
        bool ranged = false;

        if (j < s.length() && s[j] == '-')
        {
            ranged = true;
            j = skipSpaces(s, j + 1);
            j = skipNumber(s, j);
            j = skipSpaces(s, j);
        }
        else if (s.substring(j, j + 2).equalsIgnoreCase("to")
                 && (j + 2 >= s.length() || !isLetter(s[j + 2])))
        {
            ranged = true;
            j = skipSpaces(s, j + 2);
            j = skipNumber(s, j);
            j = skipSpaces(s, j);
        }

        juce::ignoreUnused(ranged);

        const auto rest = s.substring(j).trimStart();
        if (rest.startsWithIgnoreCase("khz") || rest.startsWithIgnoreCase("k hz"))
        {
            const float hz = first * 1000.0f;
            if (hz >= 100.0f)
                return hz;
        }
        else if (rest.startsWithIgnoreCase("hz"))
        {
            if (first >= 100.0f)
                return first;
        }
    }

    return std::nullopt;
}

bool IntentParser::mentionsHighShelfTarget(const juce::String& text)
{
    const auto t = text.toLowerCase();
    return t.contains("shine")
        || t.contains("cymbal")
        || t.contains("sparkle")
        || t.contains("shimmer")
        || t.contains("treble")
        || t.contains("high end")
        || t.contains("high-end")
        || t.contains("highend")
        || t.contains("top end")
        || t.contains("top-end")
        || t.contains("more air")
        || t.contains("some air")
        || t.contains("add air")
        || t.contains("the air")
        || t.contains(" air")
        || t.startsWith("air");
}

bool IntentParser::mentionsMudRequest(const juce::String& text)
{
    const auto t = text.toLowerCase();
    return t.contains("mud")
        || t.contains("boxy")
        || t.contains("boxiness")
        || t.contains("wool")
        || t.contains("rumble")
        || t.contains("low-mid")
        || t.contains("low mid");
}

void IntentParser::applyOverrideFallbacks(IntentResult& result,
                                          const juce::String& userMessage)
{
    if (!result.success || result.intentId != "brighten")
        return;

    juce::String blob = userMessage;
    for (const auto& line : result.summary)
        blob << " " << line;

    const auto it = result.parameterOverrides.find("eq.band3.freq");
    const bool hasFreq = it != result.parameterOverrides.end();
    const float current = hasFreq ? it->second : 7000.0f;
    const auto extracted = extractFrequencyHz(blob);
    const bool extractedInShelf = extracted.has_value()
        && *extracted >= 4000.0f && *extracted <= 16000.0f;
    const bool wantsAir = mentionsHighShelfTarget(blob);

    if (extractedInShelf && (!hasFreq || wantsAir)
        && std::abs(*extracted - current) >= 500.0f)
    {
        result.parameterOverrides["eq.band3.freq"] = *extracted;
        result.overrideFallback = true;
        return;
    }

    if (wantsAir && (!hasFreq || current <= 8000.0f))
    {
        result.parameterOverrides["eq.band3.freq"] = 12000.0f;
        result.overrideFallback = true;
        return;
    }

    if (!hasFreq && extractedInShelf)
    {
        result.parameterOverrides["eq.band3.freq"] = *extracted;
        result.overrideFallback = true;
    }
}

juce::DynamicObject::Ptr IntentParser::toLogObject(const IntentResult& result)
{
    juce::DynamicObject::Ptr obj = new juce::DynamicObject();
    obj->setProperty("success", result.success);
    obj->setProperty("intent", result.intentId);
    obj->setProperty("magnitude", result.magnitude);
    obj->setProperty("title", result.title);
    obj->setProperty("confidence", result.confidence);
    obj->setProperty("error", result.errorMessage);
    obj->setProperty("override_fallback", result.overrideFallback);

    juce::Array<juce::var> summary;
    for (const auto& line : result.summary)
        summary.add(line);
    obj->setProperty("summary", summary);

    juce::DynamicObject::Ptr overrides = new juce::DynamicObject();
    for (const auto& [key, value] : result.parameterOverrides)
        overrides->setProperty(key, value);
    obj->setProperty("parameter_overrides", juce::var(overrides.get()));
    obj->setProperty("override_count", static_cast<int>(result.parameterOverrides.size()));
    return obj;
}

IntentResult IntentParser::parse(const juce::String& llmResponse) const
{
    juce::String content = extractContentFromResponse(llmResponse);
    return parseContent(content);
}

juce::String IntentParser::extractContentFromResponse(const juce::String& response) const
{
    auto parsed = juce::JSON::parse(response);

    if (auto* obj = parsed.getDynamicObject())
    {
        if (auto* choices = obj->getProperty("choices").getArray())
        {
            if (!choices->isEmpty())
            {
                if (auto* choice = (*choices)[0].getDynamicObject())
                {
                    if (auto* message = choice->getProperty("message").getDynamicObject())
                    {
                        return message->getProperty("content").toString();
                    }
                }
            }
        }

        if (auto* content = obj->getProperty("content").getArray())
        {
            if (!content->isEmpty())
            {
                if (auto* block = (*content)[0].getDynamicObject())
                {
                    return block->getProperty("text").toString();
                }
            }
        }
    }

    return response;
}

IntentResult IntentParser::parseContent(const juce::String& jsonContent) const
{
    IntentResult result;

    auto parsed = juce::JSON::parse(unwrapJsonPayload(jsonContent));

    if (auto* obj = parsed.getDynamicObject())
    {
        juce::String intent = obj->getProperty("intent").toString();

        if (intent == "unknown" || intent.isEmpty())
        {
            result.success = false;
            result.errorMessage = obj->getProperty("error").toString();
            if (result.errorMessage.isEmpty())
                result.errorMessage = "Could not understand the request";
            return result;
        }

        result.success = true;
        result.intentId = intent;
        result.magnitude = static_cast<float>(obj->getProperty("magnitude"));

        if (result.magnitude <= 0.0f || result.magnitude > 1.0f)
            result.magnitude = 0.6f;

        if (obj->hasProperty("confidence"))
        {
            result.confidence = static_cast<float>(obj->getProperty("confidence"));
            if (result.confidence <= 0.0f || result.confidence > 1.0f)
                result.confidence = 0.9f;
        }

        result.title = obj->getProperty("title").toString();
        if (result.title.isEmpty())
        {
            result.title = intent.replace("_", " ");
            result.title = result.title.substring(0, 1).toUpperCase() +
                           result.title.substring(1);
        }

        if (auto* summaryArr = obj->getProperty("summary").getArray())
        {
            for (const auto& item : *summaryArr)
            {
                result.summary.add(sanitizeDashes(item.toString()));
            }
        }

        if (obj->hasProperty("parameter_overrides"))
            readParameterOverrides(obj->getProperty("parameter_overrides"),
                                   result.parameterOverrides);
    }
    else
    {
        result.success = false;
        result.errorMessage = "Failed to parse LLM response";
    }

    return result;
}
