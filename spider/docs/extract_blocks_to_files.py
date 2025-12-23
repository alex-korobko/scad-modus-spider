#!/usr/bin/env python3
"""
Extract Modbus data blocks from log file and convert to register format files.

This script reads a log file containing hexadecimal Modbus response data blocks
(extracted from answer_from_device lines) and converts each block into a separate
file following the format used in 4_1.txt.

The input format (from log_3_20251218.txt):
    Block #1 (first seen around line 64):
    answer_from_device :
        0x3  0x4   0x26
        0x0  0x0   0x1   0xb   0x0   0x0   0x3   0xca
        ... (more data bytes)
        0x0  0x0   0x0   0x0   0x0   0x0   0xfa  0xff

The output format (like 4_1.txt):
    # Modbus Function 4 Response for Device 3
    # Registers starting at 3001
    0x3
    0x4
    0x26
    #3001
    0x0
    0x0
    #3002
    0x1
    0xb
    ... (registers continue)

The script:
1. Parses each block from the input file
2. Extracts the Modbus header (device, function code, byte count)
3. Extracts data bytes (respecting the byte count, excluding CRC)
4. Groups data bytes into registers (2 bytes per register: high byte, low byte)
5. Writes each block to a separate file (4_3_1.txt, 4_3_2.txt, etc.)

Usage:
    python3 extract_blocks_to_files.py <input_file> <output_directory> [device_id]

Arguments:
    input_file       Path to the log file containing blocks (e.g., log_3_20251218.txt)
    output_directory Directory where output files will be created
    device_id        Optional device ID to use in header (default: extracted from data)

Examples:
    # Extract blocks to default location
    python3 extract_blocks_to_files.py docs/log_3_20251218.txt modbus_srv_emulator/gcc_ntox86/1/
    
    # Extract blocks with specific device ID
    python3 extract_blocks_to_files.py docs/log_3_20251218.txt output/ 3

Output files:
    The script creates files named 4_3_1.txt, 4_3_2.txt, etc., where the number
    corresponds to the block number from the input file.
"""

import re
import os
import sys


def extract_blocks_from_file(input_file):
    """
    Extract all data blocks from the input file.
    
    Args:
        input_file: Path to the input log file
        
    Returns:
        List of tuples: (block_number, hex_values_list)
    """
    with open(input_file, 'r') as f:
        content = f.read()
    
    # Split into blocks using regex
    blocks = re.split(r'Block #(\d+)', content)
    
    extracted_blocks = []
    
    # Process each block (blocks[0] is content before first block, then pairs of (num, content))
    for i in range(1, len(blocks), 2):
        block_num = int(blocks[i])
        block_content = blocks[i + 1]
        
        # Extract hex values from the block
        hex_values = []
        in_data_section = False
        
        for line in block_content.split('\n'):
            if 'answer_from_device' in line:
                in_data_section = True
                continue
            
            if in_data_section and line.strip() and '0x' in line:
                # Extract hex values from this line
                hex_vals = re.findall(r'0x[0-9a-fA-F]+', line)
                hex_values.extend(hex_vals)
        
        if len(hex_values) >= 3:  # At least header (device, function, byte_count)
            extracted_blocks.append((block_num, hex_values))
    
    return extracted_blocks


def convert_block_to_register_format(hex_values, device_id=None):
    """
    Convert a list of hex values to register format.
    
    Args:
        hex_values: List of hex strings (e.g., ['0x3', '0x4', '0x26', ...])
        device_id: Optional device ID (if None, extracted from first byte)
        
    Returns:
        Tuple: (device_id, function_code, byte_count, registers_list)
               where registers_list is [(reg_num, high_byte, low_byte), ...]
    """
    if len(hex_values) < 3:
        raise ValueError("Not enough hex values for Modbus header")
    
    # Extract header
    device_byte = hex_values[0]
    function_code = hex_values[1]
    byte_count_byte = hex_values[2]
    
    # Use provided device_id or extract from data
    if device_id is None:
        device_id = int(device_byte, 16)
    else:
        device_id = int(device_id)
    
    function_code_int = int(function_code, 16)
    byte_count = int(byte_count_byte, 16)
    
    # Extract data bytes (skip header, respect byte_count, exclude CRC)
    data_bytes = hex_values[3:3 + byte_count]
    
    # Group into registers (2 bytes per register)
    registers = []
    reg_num = 3001  # Registers start at 3001
    
    for i in range(0, len(data_bytes), 2):
        if i + 1 < len(data_bytes):
            high_byte = data_bytes[i]
            low_byte = data_bytes[i + 1]
            registers.append((reg_num, high_byte, low_byte))
            reg_num += 1
    
    return device_id, function_code_int, byte_count, registers


def write_register_file(output_file, device_id, function_code, byte_count, registers):
    """
    Write registers to a file in the 4_1.txt format.
    
    Args:
        output_file: Path to output file
        device_id: Device ID number
        function_code: Modbus function code
        byte_count: Number of data bytes
        registers: List of (reg_num, high_byte, low_byte) tuples
    """
    with open(output_file, 'w') as f:
        # Write header
        f.write(f'# Modbus Function 4 Response for Device {device_id}\n')
        f.write('# Registers starting at 3001\n')
        
        # Write Modbus header bytes
        f.write(f'0x{device_id:x}\n')
        f.write(f'0x{function_code:x}\n')
        f.write(f'0x{byte_count:x}\n')
        
        # Write registers
        for reg_num, high_byte, low_byte in registers:
            f.write(f'#{reg_num}\n')
            f.write(f'{high_byte}\n')
            f.write(f'{low_byte}\n')


def main():
    """Main function to process command line arguments and extract blocks."""
    if len(sys.argv) < 3:
        print(__doc__)
        print("\nError: Missing required arguments")
        print("Usage: python3 extract_blocks_to_files.py <input_file> <output_directory> [device_id]")
        sys.exit(1)
    
    input_file = sys.argv[1]
    output_dir = sys.argv[2]
    device_id = sys.argv[3] if len(sys.argv) > 3 else None
    
    # Validate input file
    if not os.path.isfile(input_file):
        print(f"Error: Input file '{input_file}' not found")
        sys.exit(1)
    
    # Create output directory if it doesn't exist
    os.makedirs(output_dir, exist_ok=True)
    
    # Extract blocks from input file
    print(f"Reading blocks from: {input_file}")
    blocks = extract_blocks_from_file(input_file)
    print(f"Found {len(blocks)} blocks\n")
    
    # Process each block
    for block_num, hex_values in blocks:
        try:
            # Convert to register format
            dev_id, func_code, byte_cnt, registers = convert_block_to_register_format(
                hex_values, device_id
            )
            
            # Write to output file
            output_file = os.path.join(output_dir, f'4_3_{block_num}.txt')
            write_register_file(output_file, dev_id, func_code, byte_cnt, registers)
            
            print(f"Created {output_file}")
            print(f"  Device ID: {dev_id}, Function: {func_code}, Byte count: {byte_cnt}")
            print(f"  Registers: {len(registers)} (3001-{3000 + len(registers)})")
            print()
            
        except Exception as e:
            print(f"Error processing block #{block_num}: {e}")
            continue
    
    print(f"Successfully processed {len(blocks)} blocks")
    print(f"Output files saved to: {output_dir}")


if __name__ == '__main__':
    main()

