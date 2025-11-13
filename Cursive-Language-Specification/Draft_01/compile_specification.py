#!/usr/bin/env python3
"""
Compile Cursive Language Specification from individual chapter files.

This script processes all subsection files from chapters 01-17,
removes duplicate headers and metadata, and generates a single
compiled specification document with a table of contents.
"""

import os
import re
from pathlib import Path
from typing import List, Tuple, Set


class SpecificationCompiler:
    """Compiles the Cursive Language Specification from subdirectory files."""

    def __init__(self, base_dir: Path):
        self.base_dir = base_dir
        self.seen_headings: Set[str] = set()
        self.toc_entries: List[Tuple[int, str, str]] = []  # (level, text, anchor)

    def make_anchor(self, heading: str) -> str:
        """Convert heading text to GitHub-style markdown anchor."""
        # Remove section numbers (§X.Y) and brackets [stable.label]
        text = re.sub(r'§[\d.]+\s*', '', heading)
        text = re.sub(r'\[[\w.]+\]', '', text)
        # Convert to lowercase and replace spaces with hyphens
        anchor = text.lower().strip()
        anchor = re.sub(r'[^\w\s-]', '', anchor)
        anchor = re.sub(r'[-\s]+', '-', anchor)
        return anchor

    def extract_heading_info(self, line: str) -> Tuple[int, str] or None:
        """Extract heading level and text from a markdown heading line."""
        match = re.match(r'^(#{1,6})\s+(.+)$', line)
        if match:
            level = len(match.group(1))
            text = match.group(2).strip()
            return (level, text)
        return None

    def is_chapter_heading(self, text: str) -> bool:
        """Check if heading is a chapter-level heading (Clause X — ...)."""
        return bool(re.search(r'Clause\s+\d+\s*[—-]', text))

    def is_major_section(self, text: str) -> bool:
        """Check if heading is a major section (§X.Y but not §X.Y.Z)."""
        return bool(re.match(r'^§\d+\.\d+\s+[^§]+$', text))
    
    def is_subsection(self, text: str) -> bool:
        """Check if heading is a subsection (§X.Y.Z or deeper)."""
        return bool(re.match(r'^§\d+\.\d+\.\d+', text))

    def remove_metadata_header(self, content: str) -> str:
        """Remove the document metadata header but preserve chapter headings."""
        lines = content.split('\n')
        result = []
        in_header = False
        header_found = False
        clause_heading_saved = False

        for i, line in enumerate(lines):
            # Detect start of metadata header (first # line)
            if not header_found and line.strip().startswith('# '):
                in_header = True
                header_found = True
                continue

            # Preserve the chapter heading (## Clause X — ...)
            if in_header and line.strip().startswith('## Clause '):
                result.append(line)
                clause_heading_saved = True
                # Add a blank line after the chapter heading
                result.append('')
                continue

            # Detect end of metadata header
            if in_header and line.strip() == '---':
                in_header = False
                # Skip any blank lines after the separator
                j = i + 1
                while j < len(lines) and not lines[j].strip():
                    j += 1
                # Skip to the position after blank lines
                continue

            # Skip lines within header
            if in_header:
                continue

            result.append(line)

        return '\n'.join(result)

    def remove_footer_navigation(self, content: str) -> str:
        """Remove footer navigation links (--- **Previous**: ... | **Next**: ...)."""
        lines = content.split('\n')

        # Find and remove footer pattern from the end
        for i in range(len(lines) - 1, max(0, len(lines) - 10), -1):
            if '**Previous**:' in lines[i] or '**Next**:' in lines[i]:
                # Remove this line and the preceding --- separator
                if i > 0 and lines[i - 1].strip() == '---':
                    lines = lines[:i-1]
                else:
                    lines = lines[:i]
                break

        return '\n'.join(lines)

    def remove_paragraph_numbers(self, content: str) -> str:
        """Remove paragraph numbers like [1], [2], [3] from the beginning of paragraphs."""
        # Pattern matches [number] at the start of a line or after whitespace
        # but only if it's followed by a space and text
        content = re.sub(r'^(\s*)\[\d+\]\s+', r'\1', content, flags=re.MULTILINE)
        return content

    def process_file_content(self, content: str) -> Tuple[str, List[Tuple[int, str]]]:
        """
        Process a single file's content.
        Returns (cleaned_content, headings_for_toc).
        """
        # Remove metadata and navigation
        content = self.remove_metadata_header(content)
        content = self.remove_footer_navigation(content)
        content = self.remove_paragraph_numbers(content)

        lines = content.split('\n')
        result_lines = []
        headings_for_toc = []

        for line in lines:
            heading_info = self.extract_heading_info(line)

            if heading_info:
                level, text = heading_info

                # Check for duplicate heading
                if line in self.seen_headings:
                    # Skip this duplicate heading
                    continue

                self.seen_headings.add(line)

                # Add to TOC if it's a chapter or major section
                if level == 2 and self.is_chapter_heading(text):
                    headings_for_toc.append((1, text))  # Chapter level in TOC
                elif level == 3 and self.is_major_section(text):
                    headings_for_toc.append((2, text))  # Section level in TOC

            result_lines.append(line)

        return '\n'.join(result_lines), headings_for_toc

    def get_chapter_files(self, chapter_num: int) -> List[Path]:
        """Get all markdown files from a chapter subdirectory."""
        # Actual directory names in the specification
        chapter_dirs = [
            "01_Introduction-and-Conformance",
            "02_Lexical-Structure-and-Translation",
            "03_Modules",
            "04_Declarations",
            "05_Names-Scopes-and-Resolutions",
            "06_Types",
            "07_Expressions",
            "08_Statements-and-Control-Flow",
            "09_Generics-and-Behaviors",
            "10_Memory-Model-Regions-and-Permissions",
            "11_Contracts",
            "12_Witness-System",
            "13_Concurrency-and-Memory-Ordering",
            "14_Interoperability-and-ABI",
            "15_Compile-Time-Evaluation-and-Reflection",
        ]

        # Map chapter numbers to directory names
        chapter_map = {i + 1: name for i, name in enumerate(chapter_dirs)}

        if chapter_num not in chapter_map:
            return []

        chapter_dir = self.base_dir / chapter_map[chapter_num]

        if not chapter_dir.exists():
            return []

        # Get all .md files, sorted alphabetically
        md_files = sorted(chapter_dir.glob('*.md'))

        # Filter out special files like VALIDATION-REPORT.md
        md_files = [f for f in md_files if not f.name.startswith('VALIDATION-')]

        return md_files

    def generate_toc(self) -> str:
        """Generate a flattened table of contents (only §X.Y level, not §X.Y.Z)."""
        toc_lines = ["# Table of Contents\n"]

        for level, text in self.toc_entries:
            # Skip subsections (§X.Y.Z or deeper) - only include major sections (§X.Y)
            if self.is_subsection(text):
                continue
            
            anchor = self.make_anchor(text)
            indent = "  " * (level - 1)

            if level == 1:
                # Chapter level - bold
                toc_lines.append(f"{indent}- **[{text}](#{anchor})**")
            else:
                # Section level - normal
                toc_lines.append(f"{indent}- [{text}](#{anchor})")

        return '\n'.join(toc_lines) + '\n\n---\n\n'

    def compile(self, output_file: Path) -> None:
        """Compile all specification files into a single document."""
        print("Compiling Cursive Language Specification...")

        compiled_content = []

        # Process chapters 01-15
        for chapter_num in range(1, 16):
            files = self.get_chapter_files(chapter_num)

            if not files:
                print(f"  Warning: No files found for chapter {chapter_num:02d}")
                continue

            print(f"  Processing chapter {chapter_num:02d} ({len(files)} files)...")

            for file_path in files:
                try:
                    content = file_path.read_text(encoding='utf-8')
                    cleaned_content, headings = self.process_file_content(content)

                    # Add headings to TOC
                    self.toc_entries.extend(headings)

                    # Add cleaned content
                    if cleaned_content.strip():
                        compiled_content.append(cleaned_content.strip())
                        compiled_content.append('\n\n')  # Add spacing between files

                except Exception as e:
                    print(f"    Error processing {file_path.name}: {e}")

        # Process Annex files
        print("  Processing Annex...")
        annex_dir = self.base_dir / "Annex"
        if annex_dir.exists():
            annex_files = sorted(annex_dir.glob('*.md'))
            for file_path in annex_files:
                try:
                    content = file_path.read_text(encoding='utf-8')
                    cleaned_content, headings = self.process_file_content(content)
                    self.toc_entries.extend(headings)
                    if cleaned_content.strip():
                        compiled_content.append(cleaned_content.strip())
                        compiled_content.append('\n\n')
                except Exception as e:
                    print(f"    Error processing {file_path.name}: {e}")
        else:
            print("    Warning: Annex directory not found")

        # Generate final document
        print("  Generating table of contents...")
        final_document = [
            "# Cursive Language Specification - Complete\n\n",
            self.generate_toc(),
        ]
        final_document.extend(compiled_content)

        # Write output
        print(f"  Writing output to {output_file.name}...")
        output_file.write_text(''.join(final_document), encoding='utf-8')

        print(f"\nCompilation complete!")
        print(f"  Output: {output_file}")
        print(f"  Total TOC entries: {len(self.toc_entries)}")
        print(f"  Removed duplicate headings: {len(self.seen_headings) - len(self.toc_entries)}")


def main():
    """Main entry point."""
    # Get the directory where this script is located
    script_dir = Path(__file__).parent

    # Define output file
    output_file = script_dir / "Cursive-Language-Specification-Complete.md"

    # Create compiler and run
    compiler = SpecificationCompiler(script_dir)
    compiler.compile(output_file)


if __name__ == '__main__':
    main()
