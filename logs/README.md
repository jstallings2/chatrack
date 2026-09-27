# Session logs

Debug builds write here so chat + applied actions are readable without screenshots.

- `latest.md` — human transcript
- `YYYY-MM-DD.jsonl` — one JSON object per event

These files are gitignored. Rebuild the AU after adding logging, then use the plugin; the next pass can read `logs/latest.md`.
