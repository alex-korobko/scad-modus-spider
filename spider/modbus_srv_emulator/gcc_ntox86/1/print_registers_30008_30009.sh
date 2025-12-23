#!/bin/bash

# Script to print bit values for Modbus registers 30008 and 30009
# Usage: ./print_registers_30008_30009.sh <file>

if [ $# -ne 1 ]; then
    echo "Usage: $0 <file>"
    exit 1
fi

FILE="$1"

if [ ! -f "$FILE" ]; then
    echo "Error: File '$FILE' not found"
    exit 1
fi

# Function to print bits for a byte
print_bits() {
    local byte=$1
    local register_name=$2
    local byte_type=$3  # "high" or "low"
    
    # Convert hex to decimal
    local dec=$((byte))
    
    echo "  $byte_type byte: 0x$(printf '%02x' $dec) ($dec)"
    echo "    Bit values:"
    
    # Print bits 7 to 0
    for bit in 7 6 5 4 3 2 1 0; do
        local bit_value=$(( (dec >> bit) & 1 ))
        local bit_desc=""
        
        if [ "$register_name" = "30008" ]; then
            if [ "$byte_type" = "high" ]; then
                case $bit in
                    7) bit_desc="КАНЛ+РДО+КАНП (Ключі «СТОП» на балюстраді нижні + вимикач СУРСТ)" ;;
                    6) bit_desc="ККВ+КАВЛ+КАВП (Ключі «СТОП» на балюстраді верхні)" ;;
                    5) bit_desc="КнС2+КЗП2 (Ключі заборони пуску верхні)" ;;
                    4) bit_desc="ДУО (Дистанційна зупинка)" ;;
                    3) bit_desc="Not used" ;;
                    2) bit_desc="Mashzal door" ;;
                    1) bit_desc="РКН2 (Реле контролю напруги +110В)" ;;
                    0) bit_desc="РКН1 (Реле контролю напруги ~380В)" ;;
                esac
            else  # low byte
                case $bit in
                    7) bit_desc="RKP rele of start control (1 mean escalator running on nominal speed, 0 mean escalator is stopped or in starting mode)" ;;
                    6) bit_desc="RB2 circut of power supply (1 mean is power supply NOT normal)" ;;
                    5) bit_desc="KT breaks kontaktor (1 mean breaks is ON)" ;;
                    4) bit_desc="RG Готовність телемеханіки (1 mean not ready)" ;;
                    3) bit_desc="RPV1 kontactor UP (1 means escalator running UP)" ;;
                    2) bit_desc="RPN1 kontactor DOWN (1 means escalator running DOWN)" ;;
                    1) bit_desc="RBK (Реле блокувального кола)" ;;
                    0) bit_desc="B16 (Вимикач допоміжного приводу)" ;;
                esac
            fi
        elif [ "$register_name" = "30009" ]; then
            if [ "$byte_type" = "high" ]; then
                case $bit in
                    7) bit_desc="РТЛ+РТП (Термореле підшипника вхідного валу редуктора)" ;;
                    6) bit_desc="РУП+РУЛ (Контакти реле заклинювання поручнів)" ;;
                    5) bit_desc="ВПС (Блокування підйому ступені нижнє)" ;;
                    4) bit_desc="ВВНП+ВВНЛ (Блокування нижньої вхідної площадки)" ;;
                    3) bit_desc="ВНП+ВСПН+ВСЛН+ВНЛ (Блокування натяжного пристрою + опускання ступені нижнє)" ;;
                    2) bit_desc="ВПНЛ+ВПНП (Блокування сходу поручня нижнє)" ;;
                    1) bit_desc="ВПП+ВПЛ+ВПВЛ+ВПВП (Блокування витяжки + сходу поручня верхнє)" ;;
                    0) bit_desc="ВБП+ВБЛ (Блокування бігунка ступені)" ;;
                esac
            else  # low byte
                case $bit in
                    7) bit_desc="ВСПВ+ВСЛВ (Блокування опускання ступені верхнє)" ;;
                    6) bit_desc="ВВС+ВОП+ВОЛ (Блокування демонтажу ступені + блокування «СТОП» в нахилі)" ;;
                    5) bit_desc="ВВВП+ВВВЛ (Блокування верхньої вхідної площадки)" ;;
                    4) bit_desc="ВВ+ВА+ВГ (Гвинт + упор + гайка аварійного гальма)" ;;
                    3) bit_desc="РЗ+РАТ (Блокування зі схеми аварійного гальма)" ;;
                    2) bit_desc="ВТ (Блокування робочого гальма)" ;;
                    1) bit_desc="КнС (Кнопка «СТОП» на шафі керування)" ;;
                    0) bit_desc="КнС3+КЗП3 (Ключі заборони пуску нижні)" ;;
                esac
            fi
        fi
        
        echo "      Bit $bit: $bit_value - $bit_desc"
    done
    echo ""
}

# Function to extract register bytes
extract_register() {
    local reg_num=$1
    local file=$2
    
    # Convert 30008 to 3008, 30009 to 3009 for file format
    # Remove the "0" before the last digit for 5-digit numbers
    local file_reg_num=$(echo "$reg_num" | sed 's/3000\([89]\)/300\1/')
    
    # Find the line with the register comment
    local reg_line=$(grep -n "^#${file_reg_num}$" "$file" | head -1 | cut -d: -f1)
    
    if [ -z "$reg_line" ]; then
        echo "Register $reg_num not found in file"
        return 1
    fi
    
    # Get high byte (line after comment)
    local high_byte_line=$((reg_line + 1))
    local high_byte=$(sed -n "${high_byte_line}p" "$file" | sed 's/^0x//' | tr '[:lower:]' '[:upper:]')
    
    # Get low byte (line after high byte)
    local low_byte_line=$((reg_line + 2))
    local low_byte=$(sed -n "${low_byte_line}p" "$file" | sed 's/^0x//' | tr '[:lower:]' '[:upper:]')
    
    # Convert hex to decimal
    local high_dec=$((0x${high_byte}))
    local low_dec=$((0x${low_byte}))
    
    echo "Register $reg_num:"
    print_bits $high_dec "$reg_num" "high"
    print_bits $low_dec "$reg_num" "low"
}

# Main execution
echo "=========================================="
echo "Modbus Registers 30008 and 30009 Bit Analysis"
echo "=========================================="
echo ""

extract_register 30008 "$FILE"
echo "------------------------------------------"
echo ""
extract_register 30009 "$FILE"

