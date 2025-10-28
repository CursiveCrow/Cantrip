#!/usr/bin/env python3
"""
Script to consolidate primitive type files into PART_A/01_PrimitiveTypes.md
Updates all section numbers from §5.x to §2.1.x
"""

import re

def update_section_numbers(content, subsection_num):
    """Update section numbers for a specific primitive type"""
    # Update main section header §5.X to §2.1.X
    content = re.sub(r'§5\.(\d+)', f'§2.1.{subsection_num}', content)
    
    # Update definition numbers
    content = re.sub(r'Definition 5\.(\d+)\.(\d+)', f'Definition 2.1.{subsection_num}.\\2', content)
    
    # Update subsection headers like ####### 5.1.2 to ### 2.1.{subsection_num}.2
    def replace_subsection(match):
        depth = len(match.group(1))
        old_num = match.group(2)
        # Adjust heading level (add 2 levels since we're nested deeper)
        new_depth = '#' * (depth + 2)
        return f'{new_depth} 2.1.{subsection_num}.{old_num}'
    
    content = re.sub(r'(#{2,6}) 5\.\d+\.(\d+)', replace_subsection, content)
    
    # Update cross-references in text
    content = re.sub(r'\(§5\.(\d+)\)', f'(§2.1.{subsection_num})', content)
    
    return content

def main():
    # File mappings
    files = [
        ('Spec/02_Types/01a_Integers.md', 1, 'Integer Types'),
        ('Spec/02_Types/01b_FloatingPoint.md', 2, 'Floating-Point Types'),
        ('Spec/02_Types/01c_Boolean.md', 3, 'Boolean Type'),
        ('Spec/02_Types/01d_Character.md', 4, 'Character Type'),
        ('Spec/02_Types/01e_Never.md', 5, 'Never Type'),
        ('Spec/02_Types/01f_String.md', 6, 'String Types'),
    ]
    
    # Start building consolidated file
    output = []
    
    # Header
    output.append('# Part II: Type System - §2.1 Primitive Types\n\n')
    output.append('**Section**: §2.1 | **Part**: Type System (Part II)\n')
    output.append('**Previous**: [Type System Introduction](00_Introduction.md) | ')
    output.append('**Next**: [Product Types](../PART_B_Composite/02_ProductTypes.md)\n\n')
    output.append('---\n\n')
    
    # Introduction
    output.append('## 2.1 Primitive Types Overview\n\n')
    output.append('This section specifies Cantrip\'s primitive types, which form the foundation ')
    output.append('of the type system. Primitive types are built into the language and cannot ')
    output.append('be defined by users.\n\n')
    output.append('**Primitive type categories:**\n')
    output.append('- **Integer types** (§2.1.1): Fixed-width signed and unsigned integers\n')
    output.append('- **Floating-point types** (§2.1.2): IEEE 754 binary floating-point numbers\n')
    output.append('- **Boolean type** (§2.1.3): Two-valued logic (`true`, `false`)\n')
    output.append('- **Character type** (§2.1.4): Unicode scalar values\n')
    output.append('- **Never type** (§2.1.5): Bottom type for diverging computations\n')
    output.append('- **String types** (§2.1.6): UTF-8 encoded text\n\n')
    output.append('---\n\n')
    
    # Process each primitive type file
    for filepath, subsection, title in files:
        print(f'Processing {filepath}...')
        
        try:
            with open(filepath, 'r', encoding='utf-8') as f:
                content = f.read()
            
            # Skip header and navigation links from original file
            # Find the first ## heading (the main section start)
            match = re.search(r'^## \d+\.', content, re.MULTILINE)
            if match:
                content = content[match.start():]
            
            # Update section numbers
            content = update_section_numbers(content, subsection)
            
            # Remove "Previous" and "Next" navigation from original files
            content = re.sub(r'\*\*Previous\*\*:.*?\*\*Next\*\*:.*?\n\n', '', content, flags=re.DOTALL)
            
            # Remove trailing navigation
            content = re.sub(r'\n---\n\n\*\*Previous\*\*:.*$', '', content, flags=re.DOTALL)
            
            # Add subsection marker
            output.append(f'## 2.1.{subsection} {title}\n\n')
            output.append(content)
            output.append('\n\n---\n\n')
            
        except FileNotFoundError:
            print(f'Warning: File not found: {filepath}')
            continue
    
    # Footer navigation
    output.append('**Previous**: [Type System Introduction](../00_Introduction.md) | ')
    output.append('**Next**: [Product Types](../PART_B_Composite/02_ProductTypes.md)\n')
    
    # Write consolidated file
    output_path = 'Spec/02_Types/PART_A_Primitives/01_PrimitiveTypes.md'
    print(f'Writing to {output_path}...')
    
    with open(output_path, 'w', encoding='utf-8') as f:
        f.write(''.join(output))
    
    print('Done!')

if __name__ == '__main__':
    main()
