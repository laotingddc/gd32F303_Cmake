import json
import os
import sys
import re

def parse_pin_string(pin_str):
    match = re.match(r"P([A-G])(\d+)", pin_str.upper())
    if not match:
        raise ValueError(f"Invalid pin format: {pin_str}. Expected format like PA2, PB10.")
    port_char = match.group(1)
    pin_num = int(match.group(2))
    port_idx = ord(port_char) - ord('A')
    pin_mask = 1 << pin_num
    return port_idx, pin_mask

def gen_c_code(json_file, output_c, output_h):
    with open(json_file, 'r') as f:
        data = json.load(f)
    
    gpios = data['gpios']
    parsed_gpios = []
    for gpio in gpios:
        port_idx, pin_mask = parse_pin_string(gpio['pin'])
        parsed_gpios.append({
            'name': gpio['name'].upper(),
            'port_idx': port_idx,
            'pin_mask': pin_mask,
            'mode': gpio['mode'],
            'init_level': gpio['init_level']
        })

    # 生成 .h 文件
    with open(output_h, 'w') as f:
        f.write("#ifndef GPIO_CFG_H\n#define GPIO_CFG_H\n\n")
        f.write("#include \"mhal_gpio.h\"\n\n")
        
        # 生成枚举 ID
        f.write("typedef enum {\n")
        for gpio in parsed_gpios:
            f.write(f"    MHAL_GPIO_ID_{gpio['name']},\n")
        f.write("    MHAL_GPIO_ID_MAX\n")
        f.write("} mhal_gpio_id_t;\n\n")

        # 生成便捷操作宏
        for gpio in parsed_gpios:
            id_name = f"MHAL_GPIO_ID_{gpio['name']}"
            f.write(f"#define {gpio['name']}_WRITE(level)  mhal_gpio_id_write({id_name}, level)\n")
            f.write(f"#define {gpio['name']}_TOGGLE()      mhal_gpio_id_toggle({id_name})\n")
            f.write(f"#define {gpio['name']}_READ()        mhal_gpio_id_read({id_name})\n")

        f.write(f"\nextern const mhal_gpio_cfg_t g_board_gpio_cfg[];\n")
        f.write(f"extern const size_t g_board_gpio_cnt;\n")
        f.write("\n#endif\n")

    # 生成 .c 文件
    with open(output_c, 'w') as f:
        f.write("#include \"gpio_cfg.h\"\n\n")
        f.write("const mhal_gpio_cfg_t g_board_gpio_cfg[] = {\n")
        for gpio in parsed_gpios:
            f.write(f"    [MHAL_GPIO_ID_{gpio['name']}] = {{ {gpio['port_idx']}, {gpio['pin_mask']}, {gpio['mode']}, {gpio['init_level']} }},\n")
        f.write("};\n\n")
        f.write("const size_t g_board_gpio_cnt = MHAL_GPIO_ID_MAX;\n")

if __name__ == "__main__":
    if len(sys.argv) < 4:
        print("Usage: python gen_gpio_cfg.py <input.json> <output.c> <output.h>")
    else:
        gen_c_code(sys.argv[1], sys.argv[2], sys.argv[3])
