#!/usr/bin/env python3
"""
Update 00_Introduction.md to reflect new organization
"""

import re

def main():
    filepath = 'Spec/02_Types/00_Introduction.md'
    
    print(f'Reading {filepath}...')
    with open(filepath, 'r', encoding='utf-8') as f:
        content = f.read()
    
    # Add a note at the top about the new organization
    header_addition = '''
**Note:** The type system specification has been reorganized into five parts (A-E) for better navigation:
- **Part A**: Primitive Types (§2.1)
- **Part B**: Composite Types (§2.2-§2.4)
- **Part C**: Reference Types (§2.5)
- **Part D**: Abstraction & Polymorphism (§2.6-§2.8)
- **Part E**: Type Utilities (§2.9-§2.10)

See the organizational structure below for complete details.

---

'''
    
    # Insert after the first section header but before Overview
    pattern = r'(## 0\. Type System Foundations\n\n)'
    content = re.sub(pattern, r'\1' + header_addition, content, count=1)
    
    # Update section references to point to new structure
    updates = [
        (r'\(§5\)', '(§2.1)'),  # Primitives
        (r'\(§6\)', '(§2.4 or §2.6)'),  # Arrays or Traits - context dependent
        (r'\(§7\)', '(§2.2.1)'),  # Tuples
        (r'\(§8\)', '(§2.2.2)'),  # Records
        (r'\(§9\)', '(§2.3.1 or §2.5)'),  # Enums or Pointers - context dependent
        (r'\(§10\)', '(§2.8)'),  # Map Types
        (r'\(§11\)', '(§2.7 or §2.9)'),  # Generics or Aliases - context dependent
        (r'\(§12\)', '(§2.3.3 or §2.10)'),  # Modals or Self - context dependent
    ]
    
    for old, new in updates:
        content = re.sub(old, new, content)
    
    # Update Next link to point to new location
    content = re.sub(
        r'\*\*Next\*\*: \[Primitive Types\]\(01_PrimitiveTypes\.md\)',
        '**Next**: [Primitive Types](PART_A_Primitives/01_PrimitiveTypes.md)',
        content
    )
    
    print(f'Writing updated file...')
    with open(filepath, 'w', encoding='utf-8') as f:
        f.write(content)
    
    print('Done! Updated 00_Introduction.md with organization notes and corrected links')

if __name__ == '__main__':
    main()
