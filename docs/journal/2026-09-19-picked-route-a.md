# 2026-09-19 — picked Route A

**Stage:** 2 usable
**Shipped this session:** decision only. ADR-0001. Did not build follow-ups.
**Publish later as:** “The courage to not build the chatbot”

---

## Standing questions

### Would I actually use this?

I use the PoC by typing, getting knobs, and dialing to taste. I would use a better version of *that*. I would not use a product that wants me to say “too much” instead of turning the gain down. So I am building the one I would open.

### What is the use case?

I know how I want it to sound. I do not want to hunt the tool. Chat is a summon. The processor is the product. The person this is for can hear. The person this is not for wants to describe until it is done, maybe with their voice, maybe walking around. That is a real use case and a different product.

### Is this cheating?

Navigation. Time. Menu depth. Not taste. If we start finishing the sound in language, we are cheating at the part I still want to do.

### Should this exist?

Yes, if it stays a faster way to get to a tool I then mix with. No, if “AI mixing assistant” means the chat is the mixer. I am choosing the first so the second cannot accidentally become the pitch.

### What did I just give up?

Follow-up chat. The cheap demo of “too much.” Voice / walk-around. A product that tries to serve beginners who cannot find a Q *and* engineers who can. I am giving those up on purpose so the path stays one path.

### What would make me uninstall it?

If the next feature we ship teaches me to negotiate with the model. If the knobs stay hidden behind “Adjust parameters” while we call this a tool picker. If we add Route B later “just as a mode” because it is easy.

---

## What happened

- Built / decided / showed someone: stopped before multi-turn. Picked Route A. Filed the follow-up plan instead of building it. Reason: taste is what you refuse, especially when AI can build either fork this afternoon.
- Surprise: the PoC already hides the knobs. The UI is a chatbot that happens to have an accordion. Route A has to fix that or the decision is only a document.
- Evidence: [`docs/adr/0001-route-a-tool-not-chat.md`](../adr/0001-route-a-tool-not-chat.md)

## Open threads

- Build the tool surface (knobs first, clear A/B vs revert). That is the next plan.
- Route B stays filed. Do not reopen without superseding ADR-0001.
- Taste-as-tool-choice (past sessions, Mag EQ) is still later A, not a consolation prize for skipping B.
