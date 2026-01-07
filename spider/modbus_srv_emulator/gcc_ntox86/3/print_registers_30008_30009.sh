#!/bin/sh

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
                    7) bit_desc="KANL+RDO+KANP" ;; #КАНЛ+РДО+КАНП
                    6) bit_desc="KKV+KAVL+KAVP" ;; #ККВ+КАВЛ+КАВП (Ключі «СТОП» на балюстраді верхні)
                    5) bit_desc="KnC2+K3P2" ;; #КнС2+КЗП2 (Ключі заборони пуску верхні)
                    4) bit_desc="DUO" ;; #ДУО (Дистанційна зупинка)
                    3) bit_desc="Not used" ;;
                    2) bit_desc="Mashzal door (1 means door is open)" ;;
                    1) bit_desc="RKN2 (+110V)" ;; #РКН2 (Реле контролю напруги +110В)
                    0) bit_desc="RKN2 (~380V)" ;; #РКН1 (Реле контролю напруги ~380В)
                esac
            else  # low byte
                case $bit in
                    7) bit_desc="RKP rele of start control (0 means escalator is running on nominal speed, 1 mean escalator is stopped or in starting mode)" ;;
                    6) bit_desc="RB2 circut of power supply (1 mean is power supply NOT normal)" ;;
                    5) bit_desc="KT breaks kontaktor (1 mean breaks is ON)" ;;
                    4) bit_desc="RG telemechanics is ready" ;; # NOT USED Готовність телемеханіки (1 mean not ready)
                    3) bit_desc="RPV1 kontactor UP (1 means escalator running UP)" ;;
                    2) bit_desc="RPN1 kontactor DOWN (1 means escalator running DOWN)" ;; #
                    1) bit_desc="RG2 1 is ready, 0 is not ready" ;; #RG2 (Реле готовности)
                    0) bit_desc="MU-TU 1 is on TU mode, 0 is on MU mode" ;; #MU- (Вимикач допоміжного приводу)
                esac
            fi
        elif [ "$register_name" = "30009" ]; then
            if [ "$byte_type" = "high" ]; then
                case $bit in
                    7) bit_desc="RTL+RTP" ;; #РТЛ+РТП (Термореле підшипника вхідного валу редуктора)
                    6) bit_desc="RUP+RUL" ;; #РУП+РУЛ (Контакти реле заклинювання поручнів)
                    5) bit_desc="VPS" ;; #ВПС (Блокування підйому ступені нижнє)
                    4) bit_desc="VVNP+VVNL" ;; #ВВНП+ВВНЛ (Блокування нижньої вхідної площадки)
                    3) bit_desc="VNP+VSPN+VSLN+VNL" ;; #ВНП+ВСПН+ВСЛН+ВНЛ (Блокування натяжного пристрою + опускання ступені нижнє)
                    2) bit_desc="VPNL+VPNP" ;; #ВПНЛ+ВПНП (Блокування сходу поручня нижнє)
                    1) bit_desc="VPP+VPL+VPVL+VPVP" ;; #ВПП+ВПЛ+ВПВЛ+ВПВП (Блокування витяжки + сходу поручня верхнє)
                    0) bit_desc="VBP+VBL" ;; #ВБП+ВБЛ (Блокування бігунка ступені)
                esac
            else  # low byte
                case $bit in
                    7) bit_desc="VCPV+VCLV" ;; #ВСПВ+ВСЛВ (Блокування опускання ступені верхнє)
                    6) bit_desc="VVC+VOP+VOL" ;;  #ВВС+ВОП+ВОЛ (Блокування демонтажу ступені + блокування «СТОП» в нахилі)
                    5) bit_desc="VVVP+VVVL" ;;
                    4) bit_desc="VV+VA+VG" ;;
                    3) bit_desc="P3+PAT" ;;
                    2) bit_desc="VT" ;;
                    1) bit_desc="KnC" ;;
                    0) bit_desc="KnC3 + K3P3" ;;
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

