# app/serial_manager.py

import serial
import threading
import time
from typing import Optional, Callable
from collections import deque
from protocol.hdlc import build_hdlc_frame, HDLC_FLAG, HDLC_ESCAPE, HDLC_XOR
from protocol.crc16 import crc16_ccitt

class SerialManager:
    def __init__(self, log_callback: Callable[[str], None]):
        self.serial_port = None
        self.log = log_callback
        self.last_response = None
        self.is_running = True
        self._response_lock = threading.Lock()

        # 启动接收线程
        self._reader_thread = threading.Thread(target=self._read_loop, daemon=True)
        self._reader_thread.start()

    def open(self, port: str, baudrate=115200) -> bool:
        try:
            self.serial_port = serial.Serial(port, baudrate, timeout=0.01)
            return True
        except Exception as e:
            self.log(f"Failed to open {port}: {e}")
            return False

    def close(self):
        if self.serial_port:
            self.serial_port.close()
            self.serial_port = None

    def is_open(self) -> bool:
        return self.serial_port and self.serial_port.is_open

    def send_frame(self, frame: bytes):
        if self.is_open():
            self.serial_port.write(frame)

    def wait_for_response(self, expected_cmd: int, timeout=1.0) -> Optional[bytes]:
        start = time.time()
        while time.time() - start < timeout:
            with self._response_lock:
                if self.last_response and self.last_response[0] == expected_cmd:
                    resp = self.last_response[1]
                    self.last_response = None
                    return resp
            time.sleep(0.01)
        return None

    def _read_loop(self):
        buffer = bytearray()
        escaped = False
        raw_buffer = bytearray()  # 用于累积非 HDLC 数据

        while self.is_running:
            if not self.is_open():
                time.sleep(0.1)
                continue

            try:
                byte = self.serial_port.read(1)
                if not byte:
                    # 检查是否有积压的原始数据（防止卡住不输出）
                    if raw_buffer:
                        self._flush_raw_data(raw_buffer)
                        raw_buffer.clear()
                    time.sleep(0.01)
                    continue

                b = byte[0]

                # 尝试 HDLC 解析
                if b == HDLC_FLAG:
                    # 如果之前有 raw_buffer 数据，先输出
                    if raw_buffer:
                        self._flush_raw_data(raw_buffer)
                        raw_buffer.clear()

                    # 处理 HDLC 帧
                    if len(buffer) >= 4:
                        cmd_id = int.from_bytes(buffer[:2], 'big')
                        payload = buffer[2:-2]
                        crc_recv = int.from_bytes(buffer[-2:], 'big')
                        crc_calc = crc16_ccitt(buffer[:-2])
                        if crc_calc == crc_recv:
                            with self._response_lock:
                                self.last_response = (cmd_id, payload)
                        else:
                            self.log(f"[CRC Error] Recv:0x{crc_recv:04X} Calc:0x{crc_calc:04X}")
                    buffer.clear()
                    escaped = False
                elif b == HDLC_ESCAPE:
                    if raw_buffer:
                        raw_buffer.append(b)
                    else:
                        escaped = True
                else:
                    if escaped:
                        real_byte = b ^ HDLC_XOR
                        buffer.append(real_byte)
                        escaped = False
                    else:
                        # 正常 HDLC 数据段 or 普通数据？
                        if raw_buffer or len(buffer) > 0:
                            # 已经在 HDLC 模式中，继续追加
                            buffer.append(b)
                        else:
                            # 还没看到 FLAG，可能是普通数据
                            raw_buffer.append(b)

            except Exception as e:
                self.log(f"Serial read error: {e}")
                time.sleep(0.1)

        # 循环结束前清空缓冲
        if raw_buffer:
            self._flush_raw_data(raw_buffer)
    def _flush_raw_data(self, data: bytearray):
        """将原始字节尝试解码为可读字符串并输出到 log"""
        if not data:
            return
        try:
            # 尝试 UTF-8 解码，替换非法字符
            text = data.decode('utf-8', errors='replace')
            # 去掉末尾可能的乱码，保留可打印部分
            clean_lines = []
            for line in text.splitlines(keepends=True):
                clean_lines.append(line)
            output = ''.join(clean_lines).rstrip('\n\r')  # 避免多余空行
            if output:
                self.log(f"[UART] {output}")
        except Exception as e:
            # 如果无法解码，以 hex 形式显示
            hex_str = ' '.join(f"{x:02X}" for x in data)
            self.log(f"[UART RAW] {hex_str}")
    def shutdown(self):
        self.is_running = False
        self.close()