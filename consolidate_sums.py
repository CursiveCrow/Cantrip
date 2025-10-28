#!/usr/bin/env python3
"""
Script to consolidate sum types (enums + unions + modals) into PART_B/03_SumTypes.md
Updates section numbers: §9 (enums) → §2.3.1, §8.5 (unions) → §2.3.2, §12 (modals) → §2.3.3
"""

import re

def update_enum_numbers(content):
    """Update section numbers for enums: §9.x → §2.3.1.x"""
    # Update main headers
    content = re.sub(r'# Part II: Type System - §9\. Enums', '## 2.3.1 Enums', content)
    content = re.sub(r'## 9\. Enums', '## 2.3.1 Enums', content)
    
    # Update subsection headers
    content = re.sub(r'### 9\.(\d+)', r'### 2.3.1.\1', content)
    content = re.sub(r'#### 9\.(\d+)\.(\d+)', r'#### 2.3.1.\1.\2', content)
    
    # Update cross-references
    content = re.sub(r'\(§9\.(\d+)\)', r'(§2.3.1.\1)', content)
    content = re.sub(r'\(§9\)', '(§2.3.1)', content)
    
    return content

def update_union_numbers(content):
    """Update section numbers for unions: §8.5.x → §2.3.2.x"""
    # Update main headers
    content = re.sub(r'# Part II: Type System - §8\.5\. Union Types', '## 2.3.2 Unions', content)
    content = re.sub(r'## 8\.5 Union Types', '## 2.3.2 Unions', content)
    
    # Update subsection headers
    content = re.sub(r'### 8\.5\.(\d+)', r'### 2.3.2.\1', content)
    content = re.sub(r'#### 8\.5\.(\d+)\.(\d+)', r'#### 2.3.2.\1.\2', content)
    content = re.sub(r'##### 8\.5\.(\d+)\.(\d+)\.(\d+)', r'##### 2.3.2.\1.\2.\3', content)
    
    # Update cross-references
    content = re.sub(r'\(§8\.5\.(\d+)\)', r'(§2.3.2.\1)', content)
    content = re.sub(r'\(§8\.5\)', '(§2.3.2)', content)
    
    # Update section markers
    content = re.sub(r'\*\*Section\*\*: §8\.5', '**Section**: §2.3.2', content)
    
    return content

def update_modal_numbers(content):
    """Update section numbers for modals: §12.x → §2.3.3.x"""
    # Update main headers
    content = re.sub(r'# Part II: Type System - §12\. Modals', '## 2.3.3 Modals', content)
    content = re.sub(r'## 12\. Modals', '## 2.3.3 Modals', content)
    
    # Update subsection headers
    content = re.sub(r'### 12\.(\d+)', r'### 2.3.3.\1', content)
    content = re.sub(r'#### 12\.(\d+)\.(\d+)', r'#### 2.3.3.\1.\2', content)
    
    # Update cross-references (but be careful not to update SelfType §12 refs)
    # Only update when clearly modal-related
    content = re.sub(r'\(§12\.(\d+)\)', r'(§2.3.3.\1)', content)
    
    return content

def main():
    # Start building consolidated file
    output = []
    
    # Header
    output.append('# Part II: Type System - §2.3 Sum Types\n\n')
    output.append('**Section**: §2.3 | **Part**: Type System (Part II)\n')
    output.append('**Previous**: [Product Types](02_ProductTypes.md) | ')
    output.append('**Next**: [Collection Types](04_CollectionTypes.md)\n\n')
    output.append('---\n\n')
    
    # Introduction
    output.append('## 2.3 Sum Types Overview\n\n')
    output.append('Sum types (also called tagged unions or variant types) represent values that ')
    output.append('can be one of several alternatives. Cantrip provides three sum type forms:\n\n')
    output.append('- **Enums** (§2.3.1): Tagged unions with pattern matching\n')
    output.append('- **Unions** (§2.3.2): Untagged unions for C FFI and low-level programming\n')
    output.append('- **Modals** (§2.3.3): State machines as types with compile-time verification\n\n')
    output.append('**Key distinction:** Enums are safe (compiler-enforced exhaustiveness checking), ')
    output.append('unions are unsafe (programmer must ensure correct field access), and modals add ')
    output.append('state transition verification.\n\n')
    output.append('---\n\n')
    
    # Process enums
    print('Processing enums...')
    try:
        with open('Spec/02_Types/05_Enums.md', 'r', encoding='utf-8') as f:
            content = f.read()
        
        # Remove header and navigation
        content = re.sub(r'^#.*?\n.*?\n---\n\n', '', content, count=1, flags=re.DOTALL)
        content = re.sub(r'\n---\n\n\*\*Previous\*\*:.*$', '', content, flags=re.DOTALL)
        
        content = update_enum_numbers(content)
        output.append(content)
        output.append('\n\n---\n\n')
        
    except FileNotFoundError:
        print('Warning: Enums file not found')
    
    # Process unions
    print('Processing unions...')
    try:
        with open('Spec/02_Types/04a_Unions.md', 'r', encoding='utf-8') as f:
            content = f.read()
        
        # Remove header and navigation
        content = re.sub(r'^#.*?\n.*?\n---\n\n', '', content, count=1, flags=re.DOTALL)
        content = re.sub(r'\n---\n\n\*\*Previous\*\*:.*$', '', content, flags=re.DOTALL)
        
        content = update_union_numbers(content)
        output.append(content)
        output.append('\n\n---\n\n')
        
    except FileNotFoundError:
        print('Warning: Unions file not found')
    
    # Process modals
    print('Processing modals...')
    try:
        with open('Spec/02_Types/08_Modals.md', 'r', encoding='utf-8') as f:
            content = f.read()
        
        # Remove header and navigation
        content = re.sub(r'^#.*?\n.*?\n---\n\n', '', content, count=1, flags=re.DOTALL)
        content = re.sub(r'\n---\n\n\*\*Previous\*\*:.*$', '', content, flags=re.DOTALL)
        
        content = update_modal_numbers(content)
        output.append(content)
        output.append('\n\n')
        
    except FileNotFoundError:
        print('Warning: Modals file not found')
    
    # Footer navigation
    output.append('---\n\n')
    output.append('**Previous**: [Product Types](02_ProductTypes.md) | ')
    output.append('**Next**: [Collection Types](04_CollectionTypes.md)\n')
    
    # Write consolidated file
    output_path = 'Spec/02_Types/PART_B_Composite/03_SumTypes.md'
    print(f'Writing to {output_path}...')
    
    with open(output_path, 'w', encoding='utf-8') as f:
        f.write(''.join(output))
    
    print('Done!')

if __name__ == '__main__':
    main()
