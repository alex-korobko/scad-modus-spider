#!/usr/bin/env python3
"""
Script to extract unique hexadecimal data blocks from log files.

This script parses log files looking for "answer_from_device :" markers
and extracts the hexadecimal data blocks that follow them, keeping only
unique blocks.
"""

import re
from collections import OrderedDict
import sys
import os


def extract_unique_blocks(input_file, output_file):
    """
    Extract unique hexadecimal data blocks from input file and save to output file.
    
    Args:
        input_file: Path to input log file
        output_file: Path to output file where unique blocks will be saved
    """
    # Read the log file
    try:
        with open(input_file, 'r') as f:
            lines = f.readlines()
    except FileNotFoundError:
        print(f"Error: Input file '{input_file}' not found.")
        return False
    except Exception as e:
        print(f"Error reading input file: {e}")
        return False

    # Dictionary to store unique data blocks
    unique_blocks = OrderedDict()

    # Track if we're in a data block
    in_block = False
    current_block = []

    for i, line in enumerate(lines):
        if 'answer_from_device' in line:
            # If we have a previous block, save it
            if current_block:
                block_str = '\n'.join(current_block)
                if block_str not in unique_blocks:
                    unique_blocks[block_str] = i
                current_block = []
            in_block = True
            continue
        
        if in_block:
            # Check if line contains hex data (starts with tab and has 0x)
            if line.strip() and (line.startswith('\t') or line.startswith(' ') or '0x' in line):
                # Extract hex values
                hex_values = re.findall(r'0x[0-9a-fA-F]+', line)
                if hex_values:
                    current_block.append(line.rstrip())
            else:
                # End of block - save it
                if current_block:
                    block_str = '\n'.join(current_block)
                    if block_str not in unique_blocks:
                        unique_blocks[block_str] = i
                    current_block = []
                in_block = False

    # Save the last block if exists
    if current_block:
        block_str = '\n'.join(current_block)
        if block_str not in unique_blocks:
            unique_blocks[block_str] = len(lines)

    # Write unique blocks to output file
    try:
        # Create output directory if it doesn't exist
        output_dir = os.path.dirname(output_file)
        if output_dir:
            os.makedirs(output_dir, exist_ok=True)
        
        with open(output_file, 'w') as f:
            f.write(f'Unique hexadecimal data blocks extracted from {os.path.basename(input_file)}\n')
            f.write(f'Total unique blocks found: {len(unique_blocks)}\n')
            f.write('=' * 80 + '\n\n')
            
            for idx, (block, line_num) in enumerate(unique_blocks.items(), 1):
                f.write(f'Block #{idx} (first seen around line {line_num}):\n')
                f.write('answer_from_device :\n')
                f.write(block)
                f.write('\n\n')
        
        print(f'Extracted {len(unique_blocks)} unique data blocks')
        print(f'Saved to {output_file}')
        return True
    except Exception as e:
        print(f"Error writing output file: {e}")
        return False


def main():
    """Main function to run the script from command line."""
    if len(sys.argv) < 3:
        print("Usage: python3 extract_unique_hex_blocks.py <input_log_file> <output_file>")
        print("\nExample:")
        print("  python3 extract_unique_hex_blocks.py /path/to/log_3.txt /path/to/output.txt")
        sys.exit(1)
    
    input_file = sys.argv[1]
    output_file = sys.argv[2]
    print(f"Input file: {input_file}; Output file: {output_file}")
    success = extract_unique_blocks(input_file, output_file)
    sys.exit(0 if success else 1)


if __name__ == '__main__':
    main()

