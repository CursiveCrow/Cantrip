#!/usr/bin/env python3
"""
Update navigation links (Previous/Next) in all reorganized type files
"""

import re
import os

def update_navigation(filepath, prev_text, prev_link, next_text, next_link):
    """Update navigation footer in a file"""
    try:
        with open(filepath, 'r', encoding='utf-8') as f:
            content = f.read()
        
        # Pattern to match existing navigation
        nav_pattern = r'\*\*Previous\*\*:.*?\*\*Next\*\*:.*?\n'
        
        # New navigation
        new_nav = f'**Previous**: [{prev_text}]({prev_link}) | **Next**: [{next_text}]({next_link})\n'
        
        # Replace all occurrences (there might be one at top and one at bottom)
        content = re.sub(nav_pattern, new_nav, content)
        
        with open(filepath, 'w', encoding='utf-8') as f:
            f.write(content)
        
        return True
    except Exception as e:
        print(f'  Error updating {filepath}: {e}')
        return False

def main():
    base = 'Spec/02_Types'
    
    # Navigation chain
    nav_chain = [
        # (filepath, prev_text, prev_link, next_text, next_link)
        (f'{base}/00_Introduction.md', 
         'Part I: Foundations', '../01_FrontMatter/00_Overview.md',
         'Primitive Types', 'PART_A_Primitives/01_PrimitiveTypes.md'),
        
        (f'{base}/PART_A_Primitives/01_PrimitiveTypes.md',
         'Type System Introduction', '../00_Introduction.md',
         'Product Types', '../PART_B_Composite/02_ProductTypes.md'),
        
        (f'{base}/PART_B_Composite/02_ProductTypes.md',
         'Primitive Types', '../PART_A_Primitives/01_PrimitiveTypes.md',
         'Sum Types', '03_SumTypes.md'),
        
        (f'{base}/PART_B_Composite/03_SumTypes.md',
         'Product Types', '02_ProductTypes.md',
         'Collection Types', '04_CollectionTypes.md'),
        
        (f'{base}/PART_B_Composite/04_CollectionTypes.md',
         'Sum Types', '03_SumTypes.md',
         'Pointers', '../PART_C_References/05_Pointers.md'),
        
        (f'{base}/PART_C_References/05_Pointers.md',
         'Collection Types', '../PART_B_Composite/04_CollectionTypes.md',
         'Traits', '../PART_D_Abstraction/06_Traits.md'),
        
        (f'{base}/PART_D_Abstraction/06_Traits.md',
         'Pointers', '../PART_C_References/05_Pointers.md',
         'Generics', '07_Generics.md'),
        
        (f'{base}/PART_D_Abstraction/07_Generics.md',
         'Traits', '06_Traits.md',
         'Function Types', '08_FunctionTypes.md'),
        
        (f'{base}/PART_D_Abstraction/08_FunctionTypes.md',
         'Generics', '07_Generics.md',
         'Type Aliases', '../PART_E_Utilities/09_TypeAliases.md'),
        
        (f'{base}/PART_E_Utilities/09_TypeAliases.md',
         'Function Types', '../PART_D_Abstraction/08_FunctionTypes.md',
         'Self Type', '10_SelfType.md'),
        
        (f'{base}/PART_E_Utilities/10_SelfType.md',
         'Type Aliases', '09_TypeAliases.md',
         'Part III: Expressions', '../../03_Expressions/00_Introduction.md'),
    ]
    
    print('Updating navigation links...')
    print('=' * 60)
    
    success_count = 0
    for filepath, prev_text, prev_link, next_text, next_link in nav_chain:
        filename = os.path.basename(filepath)
        print(f'Updating {filename}...')
        if update_navigation(filepath, prev_text, prev_link, next_text, next_link):
            success_count += 1
            print(f'  [OK] {prev_text} <- {filename} -> {next_text}')
        else:
            print(f'  [FAIL] Could not update {filename}')
    
    print('=' * 60)
    print(f'Complete: {success_count}/{len(nav_chain)} files updated')

if __name__ == '__main__':
    main()
