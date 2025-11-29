# protocol/hdlc.py

from .crc16 import crc16_ccitt
import struct

HDLC_FLAG = 0x7E
HDLC_ESCAPE = 0x7D
HDLC_XOR = 0x20

def hdlc_escape(data: bytes) -> bytes:
    escaped = bytearray()
    for b in data:
        if b == HDLC_FLAG or b == HDLC_ESCAPE:
            escaped.append(HDLC_ESCAPE)
            escaped.append(b ^ HDLC_XOR)
        else:
            escaped.append(b)
    return bytes(escaped)

def build_hdlc_frame(cmd_id: int, payload: bytes = b'') -> bytes:
    header = struct.pack('>H', cmd_id)
    crc_data = header + payload
    crc = crc16_ccitt(crc_data)
    frame = header + payload + struct.pack('>H', crc)
    escaped_frame = hdlc_escape(frame)
    return bytes([HDLC_FLAG]) + escaped_frame + bytes([HDLC_FLAG])