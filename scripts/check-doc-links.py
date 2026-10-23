#!/usr/bin/env python3
"""Check repository-local links in maintained Markdown documentation.

Only file/directory existence is checked. External URLs and heading fragments
are deliberately left to external link/anchor validation tools.
"""

from __future__ import annotations

import re
import sys
from pathlib import Path
from urllib.parse import unquote, urlsplit

ROOT = Path(__file__).resolve().parents[1]
LINK_PATTERN = re.compile(r"(?<!!)\[[^\]]+\]\((<[^>]+>|[^\s)]+)(?:\s+\"[^\"]*\")?\)")
EXTERNAL_SCHEMES = {"http", "https", "mailto", "tel", "data"}
SKIP_DIRS = {"build", "build-debug", "build-release", ".git", ".venv", "node_modules"}


def markdown_sources() -> list[Path]:
    sources = [ROOT / "README.md"]
    docs = ROOT / "docs"
    if docs.exists():
        sources.extend(docs.rglob("*.md"))
    return sorted(path for path in sources if path.is_file()
                  and not any(part in SKIP_DIRS for part in path.relative_to(ROOT).parts))


def checked_links(path: Path) -> tuple[int, list[str]]:
    text = path.read_text(encoding="utf-8")
    # Ignore code blocks, which can contain illustrative Markdown syntax.
    outside_code = re.sub(r"(?ms)^\s*`{3,}[^\n]*\n.*?^\s*`{3,}[^\n]*(?:\n|$)", "\n", text)
    count = 0
    errors: list[str] = []
    for match in LINK_PATTERN.finditer(outside_code):
        raw = match.group(1).strip("<>")
        parsed = urlsplit(raw)
        if parsed.scheme.lower() in EXTERNAL_SCHEMES or parsed.netloc:
            continue
        if not parsed.path:
            continue  # Same-document heading link.
        count += 1
        destination = (path.parent / unquote(parsed.path)).resolve()
        if not destination.exists():
            relative = path.relative_to(ROOT).as_posix()
            errors.append(f"{relative}: missing target for [{match.group(0)}]")
    return count, errors


def main() -> int:
    sources = markdown_sources()
    total = 0
    errors: list[str] = []
    for source in sources:
        n, failures = checked_links(source)
        total += n
        errors.extend(failures)
    for error in errors:
        print(f"ERROR: {error}", file=sys.stderr)
    print(f"Checked {total} local links across {len(sources)} Markdown files; "
          f"{len(errors)} missing targets.")
    return 1 if errors else 0


if __name__ == "__main__":
    sys.exit(main())
