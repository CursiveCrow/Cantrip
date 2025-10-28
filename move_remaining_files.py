#!/usr/bin/env python3
"""
Script to move and rename remaining type files with section number updates
"""

import re
import shutil

def update_section_number(content, old_section, new_section):
    """Generic function to update section numbers"""
    # Update main header
    pattern = rf'# Part II: Type System - §{old_section}\.'
    content = re.sub(pattern, f'# Part II: Type System - §{new_section}.', content)
    
    # Update section markers
    content = re.sub(rf'\*\*Section\*\*: §{old_section}', f'**Section**: §{new_section}', content)
    
    # Update subsection headers
    content = re.sub(rf'## {old_section}\.', f'## {new_section}.', content)
    content = re.sub(rf'### {old_section}\.(\d+)', rf'### {new_section}.\1', content)
    content = re.sub(rf'#### {old_section}\.(\d+)\.(\d+)', rf'#### {new_section}.\1.\2', content)
    content = re.sub(rf'##### {old_section}\.(\d+)\.(\d+)\.(\d+)', rf'##### {new_section}.\1.\2.\3', content)
    
    # Update cross-references
    content = re.sub(rf'\(§{old_section}\.(\d+)\)', rf'(§{new_section}.\1)', content)
    content = re.sub(rf'\(§{old_section}\)', f'(§{new_section})', content)
    
    return content

def process_file(source_path, dest_path, old_num, new_num):
    """Process and move a file with section number updates"""
    print(f'Processing {source_path} -> {dest_path}')
    print(f'  Updating §{old_num} -> §{new_num}')
    
    try:
        with open(source_path, 'r', encoding='utf-8') as f:
            content = f.read()
        
        # Update section numbers
        content = update_section_number(content, old_num, new_num)
        
        # Update navigation links to point to new locations
        # (Will need manual verification)
        
        # Write to new location
        with open(dest_path, 'w', encoding='utf-8') as f:
            f.write(content)
        
        print(f'  [OK] Complete')
        return True
        
    except FileNotFoundError:
        print(f'  [ERROR] File not found: {source_path}')
        return False

def main():
    moves = [
        # (source, destination, old_section, new_section)
        
        # Collections: §6 -> §2.4
        ('Spec/02_Types/02_ArraysAndSlices.md', 
         'Spec/02_Types/PART_B_Composite/04_CollectionTypes.md', 
         '6', '2.4'),
        
        # Pointers: §9 -> §2.5
        ('Spec/02_Types/09_Pointers.md', 
         'Spec/02_Types/PART_C_References/05_Pointers.md', 
         '9', '2.5'),
        
        # Traits: §6 -> §2.6
        ('Spec/02_Types/06_Traits.md', 
         'Spec/02_Types/PART_D_Abstraction/06_Traits.md', 
         '6', '2.6'),
        
        # Generics: §11 -> §2.7
        ('Spec/02_Types/07_Generics.md', 
         'Spec/02_Types/PART_D_Abstraction/07_Generics.md', 
         '11', '2.7'),
        
        # MapTypes: §10 -> §2.8 (rename to FunctionTypes)
        ('Spec/02_Types/10_MapTypes.md', 
         'Spec/02_Types/PART_D_Abstraction/08_FunctionTypes.md', 
         '10', '2.8'),
        
        # TypeAliases: §11 -> §2.9
        ('Spec/02_Types/11_TypeAliases.md', 
         'Spec/02_Types/PART_E_Utilities/09_TypeAliases.md', 
         '11', '2.9'),
        
        # SelfType: §12 -> §2.10
        ('Spec/02_Types/12_SelfType.md', 
         'Spec/02_Types/PART_E_Utilities/10_SelfType.md', 
         '12', '2.10'),
    ]
    
    print('=' * 60)
    print('Moving and updating remaining type files')
    print('=' * 60)
    print()
    
    success_count = 0
    for source, dest, old_num, new_num in moves:
        if process_file(source, dest, old_num, new_num):
            success_count += 1
        print()
    
    print('=' * 60)
    print(f'Complete: {success_count}/{len(moves)} files processed successfully')
    print('=' * 60)
    
    # Additional warning about conflicts
    if success_count > 0:
        print()
        print('NOTE: Some cross-references may need manual fixes:')
        print('  - §6 was used for both Arrays and Traits (context-dependent)')
        print('  - §9 was used for both Enums and Pointers (context-dependent)')
        print('  - §11 was used for both Generics and TypeAliases (context-dependent)')
        print('  - §12 was used for both Modals and SelfType (context-dependent)')
        print()
        print('  These will need to be fixed in the global cross-reference update step.')

if __name__ == '__main__':
    main()
