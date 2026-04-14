#!/bin/sh

# Script to print values for Modbus registers 30001-30009
# Usage: ./print_registers.sh <file>

if [ $# -ne 1 ]; then
    echo "Usage: $0 <file>"
    exit 1
fi

FILE="$1"

if [ ! -f "$FILE" ]; then
    echo "Error: File '$FILE' not found"
    exit 1
fi

# Variable to store register 30003 low byte for breaking path value calculation
register_30003_low_byte=""

# Variables to store register 30006 values for running path value calculation
register_30006_high_byte=""
register_30006_low_byte=""

# Function to interpret register 30008 or 30009 (bit-level analysis)
interpret_register_30008_30009() {
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

# Function to interpret register 30001 (escalator state)
interpret_register_30001() {
    local high_dec=$1
    local low_dec=$2
    
    echo "  High byte: 0x$(printf '%02x' $high_dec) ($high_dec) - Not used"
    echo "  Low byte: 0x$(printf '%02x' $low_dec) ($low_dec) - Escalator state code"
    echo "    State:"
    case $low_dec in
        0) echo "      GPSTOP" ;;
        1) echo "      GPUP" ;;
        2) echo "      GPDOWN" ;;
        3) echo "      UP20 (Overspeed > 20%)" ;;
        4) echo "      UP25 (Overspeed > 25%)" ;;
        5) echo "      NOT7SEC (Not stopped after 7 seconds)" ;;
        6) echo "      BREAK" ;;
        7) echo "      SAMOHOD" ;;
        8) echo "      BRAK0 (Error D0)" ;;
        9) echo "      BRAK90 (Error D90)" ;;
        10) echo "      BRAKALLES (Error all signals)" ;;
        11) echo "      BRAKKSR (Error contact)" ;;
        *) echo "      Unknown state code: $low_dec" ;;
    esac
    echo ""
}

# Function to interpret register 30002 (DKSE position and escalator type)
interpret_register_30002() {
    local high_dec=$1
    local low_dec=$2
    
    echo "  High byte: 0x$(printf '%02x' $high_dec) ($high_dec) - DKSE position"
    if [ $high_dec -eq 0 ]; then
        echo "    Position: Left"
    elif [ $high_dec -eq 1 ]; then
        echo "    Position: Right"
    else
        echo "    Position: Unknown value ($high_dec)"
    fi
    
    echo "  Low byte: 0x$(printf '%02x' $low_dec) ($low_dec) - Escalator type & speed"
    echo "    Type code: $low_dec"
    echo "    (See UDKU_TYPE_* enum for type mapping)"
    echo ""
}

# Function to interpret register 30003 (breaking path status and value byte 3)
interpret_register_30003() {
    local high_dec=$1
    local low_dec=$2
    
    echo "  High byte: 0x$(printf '%02x' $high_dec) ($high_dec) - Breaking path status"
    if [ $high_dec -eq 0 ]; then
        echo "    Status: NOT normal"
    elif [ $high_dec -eq 255 ]; then
        echo "    Status: Normal (0xff)"
    else
        echo "    Status: Unknown value ($high_dec)"
    fi
    
    echo "  Low byte: 0x$(printf '%02x' $low_dec) ($low_dec) - Third byte of breaking path value"
    # Store the low byte for combining with register 30004
    register_30003_low_byte=$low_dec
    echo ""
}

# Function to interpret register 30004 (breaking path value bytes 2 and 1)
interpret_register_30004() {
    local high_dec=$1
    local low_dec=$2
    
    echo "  High byte: 0x$(printf '%02x' $high_dec) ($high_dec) - Second byte of breaking path value"
    echo "  Low byte: 0x$(printf '%02x' $low_dec) ($low_dec) - First byte of breaking path value"
    
    # Combine with register 30003 low byte to form the complete breaking path value
    # Order: 30003 low byte (third/msb) | 30004 high byte (second) | 30004 low byte (first/lsb)
    if [ -n "$register_30003_low_byte" ]; then
        local combined_value=$(( (register_30003_low_byte << 16) | (high_dec << 8) | low_dec ))
        local combined_hex=$(printf '0x%06x' $combined_value)
        echo "  Combined breaking path value: $combined_hex ($combined_value)"
    else
        echo "  (Register 30003 low byte not available for combined value)"
    fi
    echo ""
}

# Function to interpret register 30005 (escalator speeds)
interpret_register_30005() {
    local high_dec=$1
    local low_dec=$2
    
    echo "  High byte: 0x$(printf '%02x' $high_dec) ($high_dec) - Nominal escalator speed"
    echo "    Speed: $high_dec cm/s"
    
    echo "  Low byte: 0x$(printf '%02x' $low_dec) ($low_dec) - Current escalator speed"
    echo "    Speed: $low_dec cm/s"
    echo ""
}

# Function to interpret register 30006 (running path bytes 4 and 3)
interpret_register_30006() {
    local high_dec=$1
    local low_dec=$2
    
    echo "  High byte: 0x$(printf '%02x' $high_dec) ($high_dec) - Fourth byte of running path (porbeg)"
    echo "  Low byte: 0x$(printf '%02x' $low_dec) ($low_dec) - Third byte of running path (porbeg)"
    # Store the values for combining with register 30007
    register_30006_high_byte=$high_dec
    register_30006_low_byte=$low_dec
    echo ""
}

# Function to interpret register 30007 (running path bytes 2 and 1)
interpret_register_30007() {
    local high_dec=$1
    local low_dec=$2
    
    echo "  High byte: 0x$(printf '%02x' $high_dec) ($high_dec) - Second byte of running path (porbeg)"
    echo "  Low byte: 0x$(printf '%02x' $low_dec) ($low_dec) - First byte of running path (porbeg)"
    
    # Combine with register 30006 values to form the complete running path value
    # Order: 30006 high byte (fourth/msb) | 30006 low byte (third) | 30007 high byte (second) | 30007 low byte (first/lsb)
    if [ -n "$register_30006_high_byte" ] && [ -n "$register_30006_low_byte" ]; then
        local combined_value=$(( (register_30006_high_byte << 24) | (register_30006_low_byte << 16) | (high_dec << 8) | low_dec ))
        local combined_hex=$(printf '0x%08x' $combined_value)
        echo "  Combined running path value: $combined_hex ($combined_value) kilometers"
    else
        echo "  (Register 30006 values not available for combined value)"
    fi
    echo ""
}

# Function to extract register bytes
extract_register() {
    local reg_num=$1
    local file=$2
    
    # Convert register number for file format
    # File format uses 4-digit numbers: 30001->3001, 30002->3002, ..., 30009->3009, 30010->3010, etc.
    # Pattern: remove the zero after "300" (30001->3001, 30010->3010)
    local file_reg_num
    # For 30001-30009: replace "3000" with "300" (30001->3001)
    # For 30010+: replace "300" with "30" when followed by 1-9 (30010->3010)
    if echo "$reg_num" | grep -q '^3000[0-9]$'; then
        # 30001-30009: remove one zero
        file_reg_num=$(echo "$reg_num" | sed 's/3000/300/')
    elif echo "$reg_num" | grep -q '^300[1-9][0-9]'; then
        # 30010+: remove zero after 300
        file_reg_num=$(echo "$reg_num" | sed 's/300/30/')
    else
        file_reg_num="$reg_num"
    fi
    
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
    
    # Interpret based on register number
    case $reg_num in
        30001)
            interpret_register_30001 $high_dec $low_dec
            ;;
        30002)
            interpret_register_30002 $high_dec $low_dec
            ;;
        30003)
            interpret_register_30003 $high_dec $low_dec
            ;;
        30004)
            interpret_register_30004 $high_dec $low_dec
            ;;
        30005)
            interpret_register_30005 $high_dec $low_dec
            ;;
        30006)
            interpret_register_30006 $high_dec $low_dec
            ;;
        30007)
            interpret_register_30007 $high_dec $low_dec
            ;;
        30008|30009)
            interpret_register_30008_30009 $high_dec "$reg_num" "high"
            interpret_register_30008_30009 $low_dec "$reg_num" "low"
            ;;
        *)
            echo "  High byte: 0x$(printf '%02x' $high_dec) ($high_dec)"
            echo "  Low byte: 0x$(printf '%02x' $low_dec) ($low_dec)"
            echo ""
            ;;
    esac
}

# Main execution
echo "=========================================="
echo "Modbus Registers 30001-30009 Analysis"
echo "=========================================="
echo ""

# Reset breaking path and running path value storage
register_30003_low_byte=""
register_30006_high_byte=""
register_30006_low_byte=""

for reg in 30001 30002 30003 30004 30005 30006 30007 30008 30009; do
    extract_register $reg "$FILE"
    if [ $reg != "30009" ]; then
        echo "------------------------------------------"
        echo ""
    fi
done

