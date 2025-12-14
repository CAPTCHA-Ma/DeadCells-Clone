import os
import json
import xml.etree.ElementTree as ET
import base64
import zlib
import struct

# --- 配置 ---
MAP_DIR = "./Resources/room/Prison"      # TMX 文件夹路径
OUTPUT_FILE = "./Resources/room/map_data.json" # 输出文件路径
TARGET_LAYER_NAME = "lnk"         # 开口图层名称
TARGET_TILESET_KEYWORD = "lnk"    # Tileset 文件名包含的关键字 (用于定位 firstgid)
GROUP_SIZE = 5                    # 连续瓦片数量

# Tiled 翻转标志位掩码
GID_MASK = 0x1FFFFFFF

def scan_continuous_exits(line_gids):
    """
    扫描一行/一列 GID，寻找连续 GROUP_SIZE 个相同的非空瓦片。
    返回列表: [{"gid": local_id, "index": pos}, ...]
    """
    exits = []
    length = len(line_gids)
    
    i = 0
    while i <= length - GROUP_SIZE:
        gid = line_gids[i]
        
        # 1. 跳过空瓦片 (Local ID < 0 视为无效/空)
        if gid < 0:
            i += 1
            continue
            
        # 2. 检查接下来的 4 个是否跟它一样
        is_continuous = True
        for offset in range(1, GROUP_SIZE):
            if line_gids[i + offset] != gid:
                is_continuous = False
                break
        
        if is_continuous:
            exits.append({
                "gid": gid,   # 这里的 gid 已经是修正后的 Local ID (0, 1, 2...)
                "index": i
            })
            i += GROUP_SIZE
        else:
            i += 1
            
    return exits

def parse_tmx(filepath):
    try:
        tree = ET.parse(filepath)
        root = tree.getroot()
    except Exception as e:
        print(f"Error parsing {filepath}: {e}")
        return None

    # 获取地图尺寸
    width = int(root.attrib.get('width'))
    height = int(root.attrib.get('height'))
    expected_count = width * height  # 我们预期应该有多少个格子

    # --- 1. 查找目标 Tileset 的 firstgid ---
    first_gid = 0
    found_tileset = False
    
    for tileset in root.findall('tileset'):
        source = tileset.attrib.get('source', '')
        if TARGET_TILESET_KEYWORD in source:
            first_gid = int(tileset.attrib.get('firstgid'))
            found_tileset = True
            break
            
    if not found_tileset:
        print(f"Warning: Tileset containing '{TARGET_TILESET_KEYWORD}' not found in {filepath}")
        return None

    # --- 2. 查找图层 ---
    layer = None
    for l in root.findall('layer'):
        if l.attrib.get('name') == TARGET_LAYER_NAME:
            layer = l
            break
    
    if layer is None:
        return None

    data_node = layer.find('data')
    encoding = data_node.attrib.get('encoding')
    compression = data_node.attrib.get('compression')
    
    raw_gids = []
    
    # --- 3. 解析数据 ---
    if encoding == 'csv':
        raw_gids = [int(x) for x in data_node.text.strip().replace('\n', '').split(',')]
    elif encoding == 'base64':
        decoded_data = base64.b64decode(data_node.text.strip())
        
        if compression == 'zlib':
            decoded_data = zlib.decompress(decoded_data)
        elif compression == 'gzip':
            import gzip
            decoded_data = gzip.decompress(decoded_data)
        
        # 确保字节长度是4的倍数 (防止 struct.error)
        valid_byte_len = (len(decoded_data) // 4) * 4
        decoded_data = decoded_data[:valid_byte_len]
        
        raw_gids = list(struct.unpack(f'<{len(decoded_data)//4}I', decoded_data))

    # ==========================================
    # --- 新增补丁：数据填充 (解决 IndexError) ---
    # ==========================================
    current_count = len(raw_gids)
    if current_count < expected_count:
        # 如果数据不够，用 0 (空) 补齐末尾
        missing = expected_count - current_count
        raw_gids.extend([0] * missing)
        # print(f"Fixed: Padded {missing} tiles in {filepath}") # 调试用
    elif current_count > expected_count:
        # 如果数据多了，截断
        raw_gids = raw_gids[:expected_count]

    # --- 4. GID 修正 (转为 Local ID) ---
    corrected_gids = []
    for g in raw_gids:
        clean_gid = g & GID_MASK
        if clean_gid == 0:
            corrected_gids.append(-1)
        else:
            local_id = clean_gid - first_gid
            if local_id >= 0:
                corrected_gids.append(local_id)
            else:
                corrected_gids.append(-1)

    # 提取四条边 (现在 raw_gids 长度绝对安全了)
    row_top = corrected_gids[0:width]
    row_bottom = corrected_gids[(height-1)*width : height*width]
    col_left = [corrected_gids[r*width] for r in range(height)]
    col_right = [corrected_gids[r*width + width - 1] for r in range(height)]

    return {
        "width": width,
        "height": height,
        "exits": {
            "top": scan_continuous_exits(row_top),
            "bottom": scan_continuous_exits(row_bottom),
            "left": scan_continuous_exits(col_left),
            "right": scan_continuous_exits(col_right)
        }
    }

def main():
    result = {}
    if not os.path.exists(MAP_DIR):
        print(f"Error: Directory {MAP_DIR} does not exist.")
        return

    for root_dir, dirs, files in os.walk(MAP_DIR):
        for file in files:
            if file.endswith(".tmx"):
                print(f"Processing: {file}...")
                data = parse_tmx(os.path.join(root_dir, file))
                if data:
                    result[file] = data

    with open(OUTPUT_FILE, 'w', encoding='utf-8') as f:
        json.dump(result, f, indent=4)
    print(f"Processed {len(result)} maps. Saved to {OUTPUT_FILE}")

if __name__ == "__main__":
    main()