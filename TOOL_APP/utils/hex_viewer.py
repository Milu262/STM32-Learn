# utils/hex_viewer.py

def format_hex_preview(data: bytes) -> str:
    """将二进制数据格式化为十六进制+ASCII预览"""
    hex_lines = []
    for i in range(0, len(data), 16):
        chunk = data[i:i+16]
        hex_part = ' '.join(f"{b:02X}" for b in chunk)
        ascii_part = ''.join(chr(b) if 32 <= b <= 126 else '.' for b in chunk)
        hex_lines.append(f"{i:08X}: {hex_part:<48}  {ascii_part}")
    return '\n'.join(hex_lines)