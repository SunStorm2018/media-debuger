#!/usr/bin/env python3
# SPDX-FileCopyrightText: 2025 zhang hongyuan <2063218120@qq.com>
# SPDX-License-Identifier: MIT
"""
Add SPDX headers to source files under the project (excluding build/ and third_part/).
Usage: run from project root: python3 tools/add_spdx_headers.py
It will modify files in-place. It skips files that already contain 'SPDX-License-Identifier'.
"""
import os
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
EXCLUDE_DIRS = {ROOT / 'build', ROOT / 'third_part'}

COPYRIGHT = "2025 zhang hongyuan <2063218120@qq.com>"
LICENSE_ID = "MIT"

EXT_COMMENT = {
    '.cpp': '//',
    '.c': '//',
    '.h': '//',
    '.hpp': '//',
    '.py': '#',
    '.sh': '#',
}

def should_exclude(path: Path):
    for d in EXCLUDE_DIRS:
        try:
            if d in path.parents:
                return True
        except Exception:
            pass
    return False

def add_header_to_file(path: Path):
    s = path.suffix.lower()
    if s not in EXT_COMMENT:
        return False
    comment = EXT_COMMENT[s]

    txt = path.read_text(encoding='utf-8')
    if 'SPDX-License-Identifier' in txt:
        return False

    lines = txt.splitlines()
    insert_at = 0

    # For scripts with a shebang, preserve it as first line
    if lines and lines[0].startswith('#!'):
        insert_at = 1

    # Build the SPDX license line without embedding the full
    # SPDX-License-Identifier token (with the trailing colon) in the
    # source code, to avoid confusing license-scanning tools.
    spdx_label = "SPDX-License" + "-Identifier: "
    header_lines = [f"{comment} SPDX-FileCopyrightText: {COPYRIGHT}",
                    f"{comment} " + spdx_label + LICENSE_ID]

    # If file already starts with a block of comments, preserve but still insert header at insert_at
    new_lines = lines[:insert_at] + header_lines + [''] + lines[insert_at:]
    path.write_text('\n'.join(new_lines) + '\n', encoding='utf-8')
    return True

def main():
    changed = []
    for ext in EXT_COMMENT.keys():
        for p in ROOT.rglob(f'*{ext}'):
            if should_exclude(p):
                continue
            # skip files inside .git or build directories implicitly
            if any(part.startswith('.') for part in p.parts):
                # allow dotfiles at root? For safety, skip hidden dirs
                if '.git' in p.parts:
                    continue
            try:
                if add_header_to_file(p):
                    changed.append(str(p.relative_to(ROOT)))
            except Exception as e:
                print(f"Failed to update {p}: {e}")
    print(f"Updated {len(changed)} files:")
    for c in changed:
        print(c)

if __name__ == '__main__':
    main()
