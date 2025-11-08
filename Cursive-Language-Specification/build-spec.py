#!/usr/bin/env python3
"""
Build script to concatenate all Cursive Language Specification files
into a single markdown document with a table of contents.
"""

import os
import re
from pathlib import Path
from typing import List, Tuple


def extract_sort_key(file_path: Path, spec_root: Path) -> Tuple:
    """
    Extract a sort key from the file path for proper ordering.
    Returns a tuple that can be used for sorting.
    """
    relative = file_path.relative_to(spec_root)
    parts = relative.parts

    # Handle files in numbered directories
    if len(parts) == 2:
        dir_name = parts[0]
        file_name = parts[1]

        # Handle Annex files (should come at the end)
        if dir_name == 'Annex':
            # Annex files: extract letter (A, B, C, etc.)
            match = re.match(r'^([A-Z])_', file_name)
            if match:
                return (100, ord(match.group(1)), file_name)  # 100 puts annexes at the end
            return (100, 0, file_name)

        # Extract directory number (e.g., "01" from "01_Introduction-and-Conformance")
        dir_match = re.match(r'^(\d+)_', dir_name)
        if dir_match:
            dir_num = int(dir_match.group(1))

            # Extract file number (e.g., "01-1" from "01-1_Introduction.md")
            file_match = re.match(r'^(\d+)-(\d+)_', file_name)
            if file_match:
                clause_num = int(file_match.group(1))
                sub_num = int(file_match.group(2))
                return (dir_num, clause_num, sub_num, file_name)

    # Fallback: use string sorting
    return (0, 0, 0, str(relative))


def extract_headers(content: str) -> List[Tuple[int, str]]:
    """
    Extract markdown headers from content.
    Returns list of (level, header_text) tuples.
    """
    headers = []
    for line in content.split('\n'):
        match = re.match(r'^(#{1,6})\s+(.+)$', line)
        if match:
            level = len(match.group(1))
            text = match.group(2).strip()
            headers.append((level, text))
    return headers


def generate_toc(files: List[Path], spec_root: Path) -> str:
    """
    Generate a table of contents by extracting headers from all files.
    Skips duplicate top-level headers.
    """
    toc_lines = ["# Table of Contents\n"]
    seen_headers = set()
    current_clause = None

    for file_path in files:
        with open(file_path, 'r', encoding='utf-8') as f:
            content = f.read()
            headers = extract_headers(content)

            for level, text in headers:
                # Skip "Cursive Language Specification" or "The Cursive Language Specification" headers (level 1)
                if level == 1 and ("Cursive Language Specification" in text):
                    continue

                # Track clause headers (level 2) to avoid duplicates
                if level == 2 and (text.startswith("Clause ") or text.startswith("Annex ")):
                    if text in seen_headers:
                        current_clause = text
                        continue
                    seen_headers.add(text)
                    current_clause = text

                # Create anchor from header text
                anchor = re.sub(r'[^\w\s-]', '', text.lower())
                anchor = re.sub(r'[\s_]+', '-', anchor)

                # Indent based on header level (adjusted since we skip level 1)
                indent = '  ' * (level - 2) if level > 1 else ''
                toc_lines.append(f"{indent}- [{text}](#{anchor})")

    toc_lines.append("")  # Empty line after TOC
    return '\n'.join(toc_lines)


def collect_spec_files(spec_root: Path) -> List[Path]:
    """
    Collect all specification markdown files, excluding SpecificationGuide.md.
    Returns sorted list of file paths.
    """
    files = []

    for md_file in spec_root.rglob('*.md'):
        # Skip SpecificationGuide.md
        if md_file.name == 'SpecificationGuide.md':
            continue

        # Skip the output file itself
        if md_file.name == 'CursiveSpecification.md':
            continue

        files.append(md_file)

    # Sort files using custom sort key
    files.sort(key=lambda f: extract_sort_key(f, spec_root))

    return files


def remove_duplicate_headers(content: str, current_clause: str, seen_clauses: set) -> Tuple[str, str]:
    """
    Remove duplicate top-level headers from content.
    Returns (cleaned_content, detected_clause).
    """
    lines = content.split('\n')
    cleaned_lines = []
    detected_clause = current_clause
    skip_next_blank = False

    for i, line in enumerate(lines):
        # Skip "# Cursive Language Specification" or "# The Cursive Language Specification" headers
        if line.strip() in ["# Cursive Language Specification", "# The Cursive Language Specification"]:
            skip_next_blank = True
            continue

        # Detect and handle clause/annex headers
        clause_match = re.match(r'^## ((?:Clause \d+|Annex [A-E])[^#]*)', line)
        if clause_match:
            clause_text = clause_match.group(1).strip()
            # If we've seen this clause before, skip it
            if clause_text in seen_clauses:
                skip_next_blank = True
                continue
            else:
                seen_clauses.add(clause_text)
                detected_clause = clause_text
                cleaned_lines.append(line)
                continue

        # Skip blank lines that immediately follow removed headers
        if skip_next_blank and line.strip() == '':
            skip_next_blank = False
            continue

        skip_next_blank = False
        cleaned_lines.append(line)

    return '\n'.join(cleaned_lines), detected_clause


def concatenate_files(files: List[Path]) -> str:
    """
    Concatenate all file contents, removing duplicate top-level headers.
    """
    contents = []
    seen_clauses = set()
    current_clause = None

    # Add the main title once at the beginning
    contents.append("# Cursive Language Specification")

    for file_path in files:
        with open(file_path, 'r', encoding='utf-8') as f:
            content = f.read()
            # Remove duplicate headers
            cleaned_content, current_clause = remove_duplicate_headers(
                content, current_clause, seen_clauses
            )
            # Remove trailing whitespace but preserve content structure
            cleaned_content = cleaned_content.rstrip()
            if cleaned_content:  # Only add non-empty content
                contents.append(cleaned_content)

    # Join with double newline for separation
    return '\n\n'.join(contents)


def build_specification():
    """
    Main function to build the concatenated specification.
    """
    # Get the script directory (should be in Cursive-Language-Specification)
    script_dir = Path(__file__).parent.absolute()

    print(f"Building specification from: {script_dir}")

    # Collect all specification files
    files = collect_spec_files(script_dir)

    print(f"Found {len(files)} specification files")

    # Generate table of contents
    print("Generating table of contents...")
    toc = generate_toc(files, script_dir)

    # Concatenate all files
    print("Concatenating files...")
    content = concatenate_files(files)

    # Combine TOC and content
    full_content = toc + '\n\n' + content

    # Write output file
    output_file = script_dir / 'CursiveSpecification.md'
    print(f"Writing output to: {output_file}")

    with open(output_file, 'w', encoding='utf-8') as f:
        f.write(full_content)

    print(f"Successfully created {output_file}")
    print(f"Total size: {len(full_content):,} characters")


if __name__ == '__main__':
    build_specification()
