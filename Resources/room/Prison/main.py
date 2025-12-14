import os
import json
import glob
import xml.etree.ElementTree as ET
import base64
import zlib
import struct

# 尝试导入 zstandard，如果需要处理 zstd 压缩的地图
try:
    import zstandard as zstd
except ImportError:
    zstd = None

def get_layer_data(root, layer_name):
    """
    解析指定Layer的数据，返回二维数组 [y][x] = gid
    """
    layer = root.find(f".//layer[@name='{layer_name}']")
    if layer is None:
        return None, 0, 0

    width = int(layer.get('width'))
    height = int(layer.get('height'))
    
    data_node = layer.find('data')
    compression = data_node.get('compression')
    text_data = data_node.text.strip() if data_node.text else ""
    
    if not text_data:
        return None, width, height

    byte_data = base64.b64decode(text_data)
    
    # 解压处理
    if compression == 'zlib':
        byte_data = zlib.decompress(byte_data)
    elif compression == 'gzip':
        byte_data = zlib.decompress(byte_data, zlib.MAX_WBITS | 16)
    elif compression == 'zstd':
        if zstd is None:
            raise ImportError("Map uses 'zstd' compression but 'zstandard' library is not installed. Run: pip install zstandard")
        dctx = zstd.ZstdDecompressor()
        byte_data = dctx.decompress(byte_data, max_output_size=width*height*4)
    
    # 按照地图实际尺寸计算预期大小
    num_tiles = width * height 
    expected_bytes = num_tiles * 4

    # 简单填充或截断
    if len(byte_data) < expected_bytes:
        byte_data += b'\x00' * (expected_bytes - len(byte_data))
    elif len(byte_data) > expected_bytes:
        byte_data = byte_data[:expected_bytes]

    gids = struct.unpack(f'<{num_tiles}I', byte_data)
    
    # 去除翻转标志
    MASK = 0x1FFFFFFF # 简化掩码
    
    tile_map = []
    for y in range(height):
        row = []
        for x in range(width):
            gid = gids[y * width + x]
            row.append(gid & MASK)
        tile_map.append(row)
        
    return tile_map, width, height

def get_lnk_firstgid(root):
    for tileset in root.findall('tileset'):
        source = tileset.get('source')
        if source and 'lnk' in source:
            return int(tileset.get('firstgid'))
    return None

def process_tmx(file_path):
    tree = ET.parse(file_path)
    root = tree.getroot()
    
    lnk_firstgid = get_lnk_firstgid(root)
    if lnk_firstgid is None:
        return None

    GID_UP    = lnk_firstgid + 0
    GID_RIGHT = lnk_firstgid + 1
    GID_DOWN  = lnk_firstgid + 2
    GID_LEFT  = lnk_firstgid + 3
    
    layer_map, width, height = get_layer_data(root, 'lnk')
    if layer_map is None:
        return None

    # --- 初始化结果结构：直接分为四个方向 ---
    result = {
        "width": width,
        "height": height,
        "entrances": { "top": [], "bottom": [], "left": [], "right": [] },
        "exits":     { "top": [], "bottom": [], "left": [], "right": [] }
    }

    SEQ_LEN = 5
    
    # 1. Top Edge (Row 0) -> 归入 "top"
    # 规则: Up(0)=Exit, Down(2)=Entrance
    y = 0
    x = 0
    while x <= width - SEQ_LEN:
        segment = layer_map[y][x : x+SEQ_LEN]
        if len(set(segment)) == 1:
            gid = segment[0]
            if gid == GID_UP:
                result["exits"]["top"].append([x, y])
                x += SEQ_LEN
            elif gid == GID_DOWN:
                result["entrances"]["top"].append([x, y])
                x += SEQ_LEN
            else:
                x += 1
        else:
            x += 1

    # 2. Bottom Edge (Row height-1) -> 归入 "bottom"
    # 规则: Down(2)=Exit, Up(0)=Entrance
    y = height - 1
    x = 0
    while x <= width - SEQ_LEN:
        segment = layer_map[y][x : x+SEQ_LEN]
        if len(set(segment)) == 1:
            gid = segment[0]
            if gid == GID_DOWN:
                result["exits"]["bottom"].append([x, y])
                x += SEQ_LEN
            elif gid == GID_UP:
                result["entrances"]["bottom"].append([x, y])
                x += SEQ_LEN
            else:
                x += 1
        else:
            x += 1

    # 3. Left Edge (Column 0) -> 归入 "left"
    # 规则: Left(3)=Exit, Right(1)=Entrance
    x = 0
    y = 0
    while y <= height - SEQ_LEN:
        segment = [layer_map[iy][x] for iy in range(y, y+SEQ_LEN)]
        if len(set(segment)) == 1:
            gid = segment[0]
            if gid == GID_LEFT:
                result["exits"]["left"].append([x, y])
                y += SEQ_LEN
            elif gid == GID_RIGHT:
                result["entrances"]["left"].append([x, y])
                y += SEQ_LEN
            else:
                y += 1
        else:
            y += 1
            
    # 4. Right Edge (Column width-1) -> 归入 "right"
    # 规则: Right(1)=Exit, Left(3)=Entrance
    x = width - 1
    y = 0
    while y <= height - SEQ_LEN:
        segment = [layer_map[iy][x] for iy in range(y, y+SEQ_LEN)]
        if len(set(segment)) == 1:
            gid = segment[0]
            if gid == GID_RIGHT:
                result["exits"]["right"].append([x, y])
                y += SEQ_LEN
            elif gid == GID_LEFT:
                result["entrances"]["right"].append([x, y])
                y += SEQ_LEN
            else:
                y += 1
        else:
            y += 1
            
    return result

def main():
    output_data = {}
    tmx_files = glob.glob("*.tmx")
    print(f"Found {len(tmx_files)} TMX files.")
    
    for filename in tmx_files:
        print(f"Processing {filename}...")
        try:
            map_info = process_tmx(filename)
            if map_info:
                output_data[filename] = map_info
        except Exception as e:
            print(f"Error processing {filename}: {e}")
            
    with open("map_data.json", "w", encoding='utf-8') as f:
        json.dump(output_data, f, indent=4)
    print("Done! Saved to map_data.json")

if __name__ == "__main__":
    main()