#!/usr/bin/env python3
"""
Script to consolidate product types (tuples + records) into PART_B/02_ProductTypes.md
Updates section numbers: §7 (tuples) → §2.2.1, §8 (records) → §2.2.2
"""

import re

def update_tuple_numbers(content):
    """Update section numbers for tuples: §7.x → §2.2.1.x"""
    # Update main section header
    content = re.sub(r'# Part II: Type System - §7\. Tuples', 
                     '## 2.2.1 Tuples', content)
    
    # Update section numbers in headers
    content = re.sub(r'## 7\. Tuples', '## 2.2.1 Tuples', content)
    content = re.sub(r'### 7\.(\d+)', r'#### 2.2.1.\1', content)
    content = re.sub(r'#### 7\.(\d+)\.(\d+)', r'##### 2.2.1.\1.\2', content)
    
    # Update cross-references
    content = re.sub(r'\(§7\.(\d+)\)', r'(§2.2.1.\1)', content)
    content = re.sub(r'\(§7\)', '(§2.2.1)', content)
    
    # Update navigation links
    content = re.sub(r'\*\*Section\*\*: §7', '**Section**: §2.2.1', content)
    
    return content

def update_record_numbers(content):
    """Update section numbers for records: §8.x → §2.2.2.x"""
    # Update main section header
    content = re.sub(r'# Part II: Type System - §8\. Records', 
                     '## 2.2.2 Records', content)
    
    # Update section numbers in headers
    content = re.sub(r'## 8\. Records', '## 2.2.2 Records', content)
    content = re.sub(r'### 8\.(\d+)', r'#### 2.2.2.\1', content)
    content = re.sub(r'#### 8\.(\d+)\.(\d+)', r'##### 2.2.2.\1.\2', content)
    content = re.sub(r'##### 8\.(\d+)\.(\d+)\.(\d+)', r'###### 2.2.2.\1.\2.\3', content)
    
    # Update cross-references (but NOT §8.5 which is unions in a different file)
    content = re.sub(r'\(§8\.([1-4,6-9]\d*)\)', r'(§2.2.2.\1)', content)
    content = re.sub(r'\(§8\)(?!\.5)', '(§2.2.2)', content)
    
    # Update navigation links
    content = re.sub(r'\*\*Section\*\*: §8', '**Section**: §2.2.2', content)
    
    # Note: Leave §8.5 (unions) unchanged as it's in a different file
    
    return content

def main():
    # Start building consolidated file
    output = []
    
    # Header
    output.append('# Part II: Type System - §2.2 Product Types\n\n')
    output.append('**Section**: §2.2 | **Part**: Type System (Part II)\n')
    output.append('**Previous**: [Primitive Types](../PART_A_Primitives/01_PrimitiveTypes.md) | ')
    output.append('**Next**: [Sum Types](03_SumTypes.md)\n\n')
    output.append('---\n\n')
    
    # Introduction
    output.append('## 2.2 Product Types Overview\n\n')
    output.append('Product types combine multiple values into a single composite value. ')
    output.append('Cantrip provides three product type forms:\n\n')
    output.append('- **Tuples** (§2.2.1): Anonymous ordered collections with positional access\n')
    output.append('- **Records** (§2.2.2): Named field collections with nominal typing\n')
    output.append('- **Tuple Structs** (§2.2.3): Named wrappers around tuples\n\n')
    output.append('**Key distinction:** Tuples use structural typing (two tuples with the same ')
    output.append('types are interchangeable), while records use nominal typing (two records with ')
    output.append('different names are distinct types even if they have the same fields).\n\n')
    output.append('---\n\n')
    
    # Process tuples
    print('Processing tuples...')
    try:
        with open('Spec/02_Types/03_Tuples.md', 'r', encoding='utf-8') as f:
            tuples_content = f.read()
        
        # Remove header and navigation
        tuples_content = re.sub(r'^#.*?\n.*?\n---\n\n', '', tuples_content, count=1, flags=re.DOTALL)
        tuples_content = re.sub(r'\n---\n\n\*\*Previous\*\*:.*$', '', tuples_content, flags=re.DOTALL)
        
        # Update section numbers
        tuples_content = update_tuple_numbers(tuples_content)
        
        output.append(tuples_content)
        output.append('\n\n---\n\n')
        
    except FileNotFoundError:
        print('Warning: Tuples file not found')
    
    # Process records
    print('Processing records...')
    try:
        with open('Spec/02_Types/04_Records.md', 'r', encoding='utf-8') as f:
            records_content = f.read()
        
        # Remove header and navigation
        records_content = re.sub(r'^#.*?\n.*?\n---\n\n', '', records_content, count=1, flags=re.DOTALL)
        records_content = re.sub(r'\n---\n\n\*\*Previous\*\*:.*$', '', records_content, flags=re.DOTALL)
        
        # Update section numbers
        records_content = update_record_numbers(records_content)
        
        # Extract tuple structs section (§8.8) and make it §2.2.3
        tuple_struct_match = re.search(r'### 8\.8 Tuple Structs.*', records_content, re.DOTALL)
        if tuple_struct_match:
            tuple_structs = tuple_struct_match.group(0)
            # Remove from records content
            records_content = records_content[:tuple_struct_match.start()]
            
            # Update tuple struct numbering
            tuple_structs = re.sub(r'### 8\.8 Tuple Structs', '## 2.2.3 Tuple Structs', tuple_structs)
            tuple_structs = re.sub(r'#### 8\.8\.(\d+)', r'### 2.2.3.\1', tuple_structs)
            tuple_structs = re.sub(r'##### 8\.8\.(\d+)\.(\d+)', r'#### 2.2.3.\1.\2', tuple_structs)
            
            output.append(records_content)
            output.append('\n\n---\n\n')
            output.append(tuple_structs)
        else:
            output.append(records_content)
        
    except FileNotFoundError:
        print('Warning: Records file not found')
    
    # Footer navigation
    output.append('\n\n---\n\n')
    output.append('**Previous**: [Primitive Types](../PART_A_Primitives/01_PrimitiveTypes.md) | ')
    output.append('**Next**: [Sum Types](03_SumTypes.md)\n')
    
    # Write consolidated file
    output_path = 'Spec/02_Types/PART_B_Composite/02_ProductTypes.md'
    print(f'Writing to {output_path}...')
    
    with open(output_path, 'w', encoding='utf-8') as f:
        f.write(''.join(output))
    
    print('Done!')

if __name__ == '__main__':
    main()
