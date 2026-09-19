# 2026-09-19 — two use cases (stop before follow-ups)

**Stage:** 2 usable
**Shipped this session:** no code; logged a fork before building multi-turn
**Publish later as:** “Typing was supposed to replace tool hunting, not knob tweaking”

---

## Standing questions

### Would I actually use this?

The way I use the PoC today: I type “make it brighter,” it grabs EQ, I open the knobs and dial to taste. That is still classic mixing. The LLM is not generating what I want from the get-go. It gets close and saves me the hunt. I would use *that*. I do not currently say “too much” in chat, because I just turn the knob down. Building follow-ups assumes I will stop doing that and start supervising the model instead of finishing the sound myself.

### What is the use case?

Two different people, maybe two products:

1. **Get me the right tool.** I know what I am doing. I can hear. I hate digging through menus, loading plugins, keeping them organized. Chat picks the processor (or later: *which* EQ — Mag EQ on this vocal because I always do). I tweak to taste. The thing we automate away is **hunting**, not **taste**.
2. **Chat until it sounds right.** I describe, it moves, I say “too much / more / actually warmer,” maybe I never open a knob. Hands-off, eventually voice, walking around the room. The thing we automate away is **turning knobs at all**. That is a real use case. It is a different product than (1). I am not that user today.

There is a later sibling of (1), not of (2): when we host real plugins, pick from *my* history. “He always puts Mag EQ on this vocal and lifts the high band.” Or: “this guitar wants that analog EQ because of the midrange.” Taste as tool choice, not taste as the last 0.2 of gain. Do not build this yet.

### Is this cheating?

(1) cheats at navigation. I still have to hear it and own the last move. That matches the original thesis: typing replaces tool hunting, not knob tweaking.

(2) cheats at finishing. “LLM, this is too much” means I am not going to click the knob. Fine for a voice assistant. Different moral object than “grab EQ for me.” Learning from keep-vs-undo, or from past sessions, can slide (1) into (2) if we are sloppy about what “taste” means.

### Should this exist?

Both can exist. They should not share a success metric. If we make the chat so good at dialing that the knobs become theater, we have built (2) and abandoned the person who already knows how to mix. If we never do follow-ups, we leave the hands-off / voice studio on the table. I think we build “too much” anyway — it is small, it is demoable, it does not remove the knobs — but we do not let it become the north star this week. The north star for *me* is still (1).

### What did I just give up?

If we lean into follow-up chat: we give up a clean story that Chatrack is a picker with knobs. We start training me (and anyone watching a demo) to negotiate with the model instead of finishing by ear.

If we refuse follow-ups: we give up voice, walking around, and the person who can describe sound but cannot find a Q.

### What would make me uninstall it?

If the chat becomes the only respected way to change a sound, and grabbing the knob feels like fighting the product. If it gets *worse* at picking the right tool because we spent the week teaching it to argue about magnitude. If “smarter” means it starts choosing my analog EQ from a cloud of my old sessions before I asked it to learn my taste.

---

## What happened

- Built / decided / showed someone: stopped on the threshold of multi-turn (“too much”, “actually warmer”). Noticed that feature forks the product. Original plan already said the quiet part: *typing replaces tool hunting, not knob tweaking.* Follow-ups start replacing the tweaking.
- Surprise: my actual use is already (1). The roadmap item we were about to build is (2). We can still ship (2) as a second mode. We should not pretend it is the same as making (1) better.
- Evidence: none. The evidence is how I sit in front of the action card.

## Open threads

- Build multi-turn anyway, as a **mode**, not as the definition of Stage 2. Knobs stay primary. Journal after: did I say “too much” or did I still reach for the knob?
- Adjacent plan (do not start): **taste as tool choice** — past projects, hosted plugins, “Mag EQ on this vocal,” character vs clean. That is (1) getting smarter, not (2) getting chattier.
- Voice / walk-around studio is (2) with a mic. Park it with Magenta and the DAW: identity, not this week.
- Do both use cases in one plugin? Maybe. Same rack, two prompts: “pick” vs “keep going.” Do not merge their metrics.
- Essay 0 still unpaid. This fork belongs in it.
