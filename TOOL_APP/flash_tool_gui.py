import tkinter as tk
from tkinter import ttk, filedialog, scrolledtext, messagebox, simpledialog
import serial
import serial.tools.list_ports
import threading
import time
import struct
from collections import deque
from typing import Optional

# ----------------------------
# CRC16-CCITT (0xFFFF initial)
# ----------------------------
def crc16_ccitt(data: bytes) -> int:
    crc = 0xFFFF
    for byte in data:
        crc = (crc >> 8) ^ CRC16_TABLE[(crc ^ byte) & 0xFF]
    return crc

CRC16_TABLE = [
    0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50A5, 0x60C6, 0x70E7,
    0x8108, 0x9129, 0xA14A, 0xB16B, 0xC18C, 0xD1AD, 0xE1CE, 0xF1EF,
    0x1231, 0x0210, 0x3273, 0x2252, 0x52B5, 0x4294, 0x72F7, 0x62D6,
    0x9339, 0x8318, 0xB37B, 0xA35A, 0xD3BD, 0xC39C, 0xF3FF, 0xE3DE,
    0x2462, 0x3443, 0x0420, 0x1401, 0x64E6, 0x74C7, 0x44A4, 0x5485,
    0xA56A, 0xB54B, 0x8528, 0x9509, 0xE5EE, 0xF5CF, 0xC5AC, 0xD58D,
    0x3653, 0x2672, 0x1611, 0x0630, 0x76D7, 0x66F6, 0x5695, 0x46B4,
    0xB75B, 0xA77A, 0x9719, 0x8738, 0xF7DF, 0xE7FE, 0xD79D, 0xC7BC,
    0x48C4, 0x58E5, 0x6886, 0x78A7, 0x0840, 0x1861, 0x2802, 0x3823,
    0xC9CC, 0xD9ED, 0xE98E, 0xF9AF, 0x8948, 0x9969, 0xA90A, 0xB92B,
    0x5AF5, 0x4AD4, 0x7AB7, 0x6A96, 0x1A71, 0x0A50, 0x3A33, 0x2A12,
    0xDBFD, 0xCBDC, 0xFBBF, 0xEB9E, 0x9B79, 0x8B58, 0xBB3B, 0xAB1A,
    0x6CA6, 0x7C87, 0x4CE4, 0x5CC5, 0x2C22, 0x3C03, 0x0C60, 0x1C41,
    0xEDAE, 0xFD8F, 0xCDEC, 0xDDCD, 0xAD2A, 0xBD0B, 0x8D68, 0x9D49,
    0x7E97, 0x6EB6, 0x5ED5, 0x4EF4, 0x3E13, 0x2E32, 0x1E51, 0x0E70,
    0xFF9F, 0xEFBE, 0xDFDD, 0xCFFC, 0xBF1B, 0xAF3A, 0x9F59, 0x8F78,
    0x9188, 0x81A9, 0xB1CA, 0xA1EB, 0xD10C, 0xC12D, 0xF14E, 0xE16F,
    0x1080, 0x00A1, 0x30C2, 0x20E3, 0x5004, 0x4025, 0x7046, 0x6067,
    0x83B9, 0x9398, 0xA3FB, 0xB3DA, 0xC33D, 0xD31C, 0xE37F, 0xF35E,
    0x02B1, 0x1290, 0x22F3, 0x32D2, 0x4235, 0x5214, 0x6277, 0x7256,
    0xB5EA, 0xA5CB, 0x95A8, 0x8589, 0xF56E, 0xE54F, 0xD52C, 0xC50D,
    0x34E2, 0x24C3, 0x14A0, 0x0481, 0x7466, 0x6447, 0x5424, 0x4405,
    0xCB7D, 0xDB5C, 0xEB3F, 0xFB1E, 0x8BF9, 0x9BD8, 0xABBB, 0xBB9A,
    0x4A75, 0x5A54, 0x6A37, 0x7A16, 0x0AF1, 0x1AD0, 0x2AB3, 0x3A92,
    0xFD4C, 0xED6D, 0xDD0E, 0xCD2F, 0xBD88, 0xADAF, 0x9DC0, 0x8DE1,
    0x7C0E, 0x6C2F, 0x5C4C, 0x4C6D, 0x3CA6, 0x2C87, 0x1CE4, 0x0CC5,
    0xEF1D, 0xFF3C, 0xCF5F, 0xDF7E, 0xAF99, 0xBFBA, 0x8FD5, 0x9FF4,
    0x6E1B, 0x7E3A, 0x4E59, 0x5E78, 0x2EB3, 0x3E92, 0x0EF1, 0x1ED0,
    0xD96C, 0xC94D, 0xF92E, 0xE90F, 0x9988, 0x89A9, 0xB9CA, 0xA9EB,
    0x5804, 0x4825, 0x7846, 0x6867, 0x1880, 0x08A1, 0x38C2, 0x28E3,
    0xCB3D, 0xDB5C, 0xEB7F, 0xFB5E, 0x8BD9, 0x9BF8, 0xABDB, 0xBBFA,
    0x4A15, 0x5A34, 0x6A57, 0x7A76, 0x0A91, 0x1AB0, 0x2AD3, 0x3AF2
]

# ----------------------------
# HDLC Protocol
# ----------------------------
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

# ----------------------------
# Main Application
# ----------------------------
class FlashToolApp:
    def __init__(self, root):
        self.root = root
        self.root.title("STM32 Programmer v3.0 - with Auto Reconnect & Retry")
        self.root.geometry("1100x850")
        self.root.protocol("WM_DELETE_WINDOW", self.on_closing)

        self.serial_port = None
        self.is_sending = False
        self.bin_data = b''
        self.log_queue = deque()
        self.last_response = None
        self.received_flash_data = bytearray()

        # Command IDs
        self.CMD_WRITE_FLASH_BLOCK = 0x1001
        self.CMD_WRITE_FLASH_ACK    = 0x1004  # ← 新增ACK命令
        self.CMD_READ_FLASH_REQUEST = 0x1002
        self.CMD_READ_FLASH_RESPONSE= 0x1003
        self.CMD_LED_CONTROL        = 0x2001
        self.CMD_I2C_READ_REG       = 0x2002
        self.CMD_I2C_READ_RESULT    = 0x2003
        self.CMD_SPI_READ_REG       = 0x2004
        self.CMD_SPI_READ_RESULT    = 0x2005

        self.create_widgets()
        self.update_ports()
        self.start_log_thread()
        self.start_serial_reader()

    def create_widgets(self):
        self.notebook = ttk.Notebook(self.root)
        self.notebook.pack(fill='both', expand=True, padx=5, pady=5)

        # Tab 1: Flash Programming
        self.flash_tab = ttk.Frame(self.notebook)
        self.notebook.add(self.flash_tab, text="Flash")

        top_frame = ttk.Frame(self.flash_tab)
        top_frame.pack(pady=5, fill='x')

        ttk.Label(top_frame, text="Port:").grid(row=0, column=0, sticky='w')
        self.port_var = tk.StringVar()
        self.port_combo = ttk.Combobox(top_frame, textvariable=self.port_var, width=12)
        self.port_combo.grid(row=0, column=1, padx=2)
        ttk.Button(top_frame, text="Refresh", command=self.update_ports).grid(row=0, column=2, padx=2)
        ttk.Button(top_frame, text="Open", command=self.open_serial).grid(row=0, column=3, padx=2)
        ttk.Button(top_frame, text="Close", command=self.close_serial).grid(row=0, column=4, padx=2)

        ttk.Label(top_frame, text="BIN File:").grid(row=1, column=0, sticky='w', pady=(5,0))
        self.file_path = tk.StringVar()
        ttk.Entry(top_frame, textvariable=self.file_path, width=50, state='readonly').grid(row=1, column=1, columnspan=3, pady=(5,0))
        ttk.Button(top_frame, text="Browse...", command=self.browse_file).grid(row=1, column=4, pady=(5,0))

        preview_frame = ttk.LabelFrame(self.flash_tab, text="BIN Content (Full Hex View)")
        preview_frame.pack(pady=5, fill='both', expand=True)

        self.hex_text = scrolledtext.ScrolledText(preview_frame, font=("Courier", 9))
        self.hex_text.pack(fill='both', expand=True)

        btn_frame = ttk.Frame(self.flash_tab)
        btn_frame.pack(pady=5)

        self.send_btn = ttk.Button(btn_frame, text="Write to Flash", command=self.send_bin, state='disabled')
        self.send_btn.pack(side='left', padx=5)

        self.read_btn = ttk.Button(btn_frame, text="Read Flash to BIN", command=self.read_flash, state='disabled')
        self.read_btn.pack(side='left', padx=5)

        self.progress = ttk.Progressbar(btn_frame, orient='horizontal', length=300, mode='determinate')
        self.progress.pack(side='left', padx=10)
        self.speed_label = ttk.Label(btn_frame, text="0 KB/s")
        self.speed_label.pack(side='left')

        # Tab 2: I2C/SPI Control
        self.periph_tab = ttk.Frame(self.notebook)
        self.notebook.add(self.periph_tab, text="I2C/SPI")

        i2c_frame = ttk.LabelFrame(self.periph_tab, text="I2C Register Read")
        i2c_frame.pack(pady=10, padx=10, fill='x')
        ttk.Label(i2c_frame, text="Device Addr (0x):").grid(row=0, column=0, sticky='w')
        self.i2c_addr = tk.StringVar(value="48")
        ttk.Entry(i2c_frame, textvariable=self.i2c_addr, width=10).grid(row=0, column=1, padx=5)
        ttk.Label(i2c_frame, text="Reg Addr (0x):").grid(row=0, column=2, sticky='w')
        self.i2c_reg = tk.StringVar(value="00")
        ttk.Entry(i2c_frame, textvariable=self.i2c_reg, width=10).grid(row=0, column=3, padx=5)
        self.i2c_read_btn = ttk.Button(i2c_frame, text="Read", command=self.i2c_read, state='disabled')
        self.i2c_read_btn.grid(row=0, column=4, padx=5)
        self.i2c_result = tk.StringVar()
        ttk.Label(i2c_frame, textvariable=self.i2c_result, foreground='blue').grid(row=0, column=5, padx=5)

        spi_frame = ttk.LabelFrame(self.periph_tab, text="SPI Register Read")
        spi_frame.pack(pady=10, padx=10, fill='x')
        ttk.Label(spi_frame, text="Reg Addr (0x):").grid(row=0, column=0, sticky='w')
        self.spi_reg = tk.StringVar(value="00")
        ttk.Entry(spi_frame, textvariable=self.spi_reg, width=10).grid(row=0, column=1, padx=5)
        self.spi_read_btn = ttk.Button(spi_frame, text="Read", command=self.spi_read, state='disabled')
        self.spi_read_btn.grid(row=0, column=2, padx=5)
        self.spi_result = tk.StringVar()
        ttk.Label(spi_frame, textvariable=self.spi_result, foreground='blue').grid(row=0, column=3, padx=5)

        led_frame = ttk.Frame(self.periph_tab)
        led_frame.pack(pady=10)
        self.led_btn = ttk.Button(led_frame, text="Toggle LED", command=self.toggle_led, state='disabled')
        self.led_btn.pack()

        # Log Output
        log_frame = ttk.LabelFrame(self.root, text="Log")
        log_frame.pack(pady=5, padx=5, fill='both', expand=False, side='bottom')
        self.log_text = scrolledtext.ScrolledText(log_frame, height=8)
        self.log_text.pack(fill='both')

    def update_ports(self):
        ports = [p.device for p in serial.tools.list_ports.comports()]
        self.port_combo['values'] = ports
        if ports and not self.port_var.get():
            self.port_var.set(ports[0])

    def open_serial(self):
        port = self.port_var.get()
        if not port:
            messagebox.showerror("Error", "Please select a serial port")
            return
        try:
            self.serial_port = serial.Serial(port, 115200, timeout=0.01)
            self._enable_buttons()
            self.log(f"Serial opened: {port}")
        except Exception as e:
            messagebox.showerror("Serial Error", str(e))

    def close_serial(self):
        if self.serial_port:
            self.serial_port.close()
            self.serial_port = None
        self._disable_buttons()
        self.log("Serial closed")

    def _enable_buttons(self):
        self.send_btn.config(state='normal')
        self.read_btn.config(state='normal')
        self.i2c_read_btn.config(state='normal')
        self.spi_read_btn.config(state='normal')
        self.led_btn.config(state='normal')

    def _disable_buttons(self):
        self.send_btn.config(state='disabled')
        self.read_btn.config(state='disabled')
        self.i2c_read_btn.config(state='disabled')
        self.spi_read_btn.config(state='disabled')
        self.led_btn.config(state='disabled')

    def browse_file(self):
        path = filedialog.askopenfilename(filetypes=[("Binary files", "*.bin"), ("All files", "*.*")])
        if path:
            self.file_path.set(path)
            try:
                with open(path, 'rb') as f:
                    self.bin_data = f.read()
                self.update_hex_preview()
                self.log(f"Loaded {len(self.bin_data)} bytes from {path}")
            except Exception as e:
                messagebox.showerror("File Error", str(e))

    def update_hex_preview(self):
        data = self.bin_data
        hex_lines = []
        for i in range(0, len(data), 16):
            chunk = data[i:i+16]
            hex_part = ' '.join(f"{b:02X}" for b in chunk)
            ascii_part = ''.join(chr(b) if 32 <= b <= 126 else '.' for b in chunk)
            hex_lines.append(f"{i:08X}: {hex_part:<48}  {ascii_part}")
        self.hex_text.delete(1.0, tk.END)
        self.hex_text.insert(tk.END, '\n'.join(hex_lines))

    # ========================
    # Serial Communication
    # ========================
    def send_frame(self, frame: bytes):
        if self.serial_port and self.serial_port.is_open:
            self.serial_port.write(frame)

    def wait_for_response(self, expected_cmd, timeout=1.0):
        start = time.time()
        while time.time() - start < timeout:
            if self.last_response and self.last_response[0] == expected_cmd:
                resp = self.last_response[1]
                self.last_response = None  # 清除，避免重复使用
                return resp
            time.sleep(0.01)
        return None

    def start_serial_reader(self):
        def reader():
            buffer = bytearray()
            escaped = False
            while True:
                if not self.serial_port or not self.serial_port.is_open:
                    time.sleep(0.1)
                    continue
                try:
                    byte = self.serial_port.read(1)
                    if not byte:
                        continue
                    b = byte[0]
                    if escaped:
                        buffer.append(b ^ HDLC_XOR)
                        escaped = False
                    elif b == HDLC_ESCAPE:
                        escaped = True
                    elif b == HDLC_FLAG:
                        if len(buffer) >= 4:
                            cmd_id = int.from_bytes(buffer[:2], 'big')
                            payload = buffer[2:-2]
                            crc_recv = int.from_bytes(buffer[-2:], 'big')
                            crc_calc = crc16_ccitt(buffer[:-2])
                            if crc_calc == crc_recv:
                                self.last_response = (cmd_id, payload)
                        buffer.clear()
                    else:
                        buffer.append(b)
                except:
                    pass
        threading.Thread(target=reader, daemon=True).start()

    # ========================
    # Robust Send with Retry
    # ========================
    def handle_device_disconnect(self):
        """设备断连处理"""
        self.is_sending = False
        if self.serial_port and self.serial_port.is_open:
            try:
                self.serial_port.close()
            except:
                pass
            self.serial_port = None

        self.root.after(0, self._disable_buttons)
        self.root.after(0, lambda: messagebox.showwarning(
            "Device Disconnected",
            "No response from device after 3 retries.\n"
            "Please check connection and reopen serial port."
        ))
        self.log("⚠️ Device disconnected: no ACK received.")

    def send_with_retry(self, cmd_id: int, payload: bytes, expected_response_cmd: int = None, ack_required=True) -> Optional[bytes]:
        if not self.serial_port or not self.serial_port.is_open:
            return None

        if not ack_required:
            self.send_frame(build_hdlc_frame(cmd_id, payload))
            return b''

        for attempt in range(3):
            self.send_frame(build_hdlc_frame(cmd_id, payload))
            resp = self.wait_for_response(expected_response_cmd, timeout=1.0)
            if resp is not None:
                return resp
            self.log(f"[Attempt {attempt+1}/3] No response for CMD 0x{cmd_id:04X}")
            time.sleep(0.1)

        self.handle_device_disconnect()
        return None

    # ========================
    # Flash Operations
    # ========================
    def send_bin(self):
        if not self.bin_data:
            messagebox.showwarning("Warning", "No BIN file loaded")
            return
        if not self.serial_port or not self.serial_port.is_open:
            messagebox.showwarning("Warning", "Serial port not open")
            return
        if self.is_sending:
            return

        self.is_sending = True
        self.send_btn.config(state='disabled')
        self.progress['value'] = 0
        self.speed_label.config(text="0 KB/s")
        threading.Thread(target=self._send_bin_worker, daemon=True).start()

    def _send_bin_worker(self):
        block_size = 256
        total_size = len(self.bin_data)
        total_blocks = (total_size + block_size - 1) // block_size
        start_time = time.time()

        self.log(f"Writing {total_size} bytes to Flash...")

        for i in range(total_blocks):
            if not self.is_sending:
                break
            start = i * block_size
            end = min(start + block_size, total_size)
            block = self.bin_data[start:end]
            
            payload = struct.pack('>IH', start, len(block)) + block
            
            # 发送并等待 ACK（STM32 需返回 CMD_WRITE_FLASH_ACK）
            ack = self.send_with_retry(
                self.CMD_WRITE_FLASH_BLOCK,
                payload,
                expected_response_cmd=self.CMD_WRITE_FLASH_ACK
            )
            if ack is None:
                return  # 已在内部处理断连

            progress = (end / total_size) * 100
            elapsed = time.time() - start_time
            speed = end / elapsed / 1024 if elapsed > 0 else 0
            self.root.after(0, lambda p=progress, s=speed: (
                self.progress.config(value=p),
                self.speed_label.config(text=f"{s:.1f} KB/s")
            ))

        self.is_sending = False
        self.root.after(0, lambda: self.send_btn.config(state='normal'))
        self.log("✅ Write complete!")

    def read_flash(self):
        if not self.serial_port or not self.serial_port.is_open:
            return
        addr_str = simpledialog.askstring("Read Flash", "Start address (hex):", initialvalue="00000000")
        if not addr_str:
            return
        try:
            addr = int(addr_str, 16)
        except:
            messagebox.showerror("Error", "Invalid address")
            return

        length = simpledialog.askinteger("Read Flash", "Length (bytes):", initialvalue=1024)
        if length is None or length <= 0:
            return

        self.received_flash_data = bytearray()
        self.is_sending = True
        self.read_btn.config(state='disabled')
        threading.Thread(target=self._read_flash_worker, args=(addr, length), daemon=True).start()

    def _read_flash_worker(self, addr, length):
        block_size = 256
        total_blocks = (length + block_size - 1) // block_size

        self.log(f"Reading {length} bytes from Flash...")

        for i in range(total_blocks):
            if not self.is_sending:
                break
            start = addr + i * block_size
            blk_len = min(block_size, length - i * block_size)
            
            payload = struct.pack('>IH', start, blk_len)
            resp = self.send_with_retry(
                self.CMD_READ_FLASH_REQUEST,
                payload,
                expected_response_cmd=self.CMD_READ_FLASH_RESPONSE
            )
            if resp is None:
                return

        # Save result
        data = bytes(self.received_flash_data[:length])
        save_path = filedialog.asksaveasfilename(
            defaultextension=".bin",
            filetypes=[("Binary files", "*.bin")]
        )
        if save_path:
            with open(save_path, 'wb') as f:
                f.write(data)
            self.log(f"💾 Saved {len(data)} bytes to {save_path}")

        self.is_sending = False
        self.root.after(0, lambda: self.read_btn.config(state='normal'))

    # ========================
    # Peripheral Control
    # ========================
    def toggle_led(self):
        payload = bytes([0, 1])  # 示例：LED0 ON
        self.send_with_retry(self.CMD_LED_CONTROL, payload, ack_required=False)
        self.log("Sent LED ON command")

    def i2c_read(self):
        try:
            dev_addr = int(self.i2c_addr.get(), 16)
            reg_addr = int(self.i2c_reg.get(), 16)
        except:
            messagebox.showerror("Error", "Invalid I2C address")
            return

        payload = bytes([dev_addr, reg_addr])
        resp = self.send_with_retry(
            self.CMD_I2C_READ_REG,
            payload,
            expected_response_cmd=self.CMD_I2C_READ_RESULT
        )
        if resp and len(resp) >= 1:
            value = resp[0]
            self.i2c_result.set(f"0x{value:02X} ({value})")
            self.log(f"I2C Read: Dev=0x{dev_addr:02X}, Reg=0x{reg_addr:02X} → 0x{value:02X}")

    def spi_read(self):
        try:
            reg_addr = int(self.spi_reg.get(), 16)
        except:
            messagebox.showerror("Error", "Invalid SPI register")
            return

        payload = bytes([reg_addr])
        resp = self.send_with_retry(
            self.CMD_SPI_READ_REG,
            payload,
            expected_response_cmd=self.CMD_SPI_READ_RESULT
        )
        if resp and len(resp) >= 1:
            value = resp[0]
            self.spi_result.set(f"0x{value:02X} ({value})")
            self.log(f"SPI Read: Reg=0x{reg_addr:02X} → 0x{value:02X}")

    # ========================
    # Logging
    # ========================
    def log(self, msg):
        self.log_queue.append(msg)

    def process_log_queue(self):
        while self.log_queue:
            msg = self.log_queue.popleft()
            self.log_text.insert(tk.END, msg + '\n')
            self.log_text.see(tk.END)

    def start_log_thread(self):
        def log_updater():
            while True:
                self.root.after(0, self.process_log_queue)
                time.sleep(0.1)
        threading.Thread(target=log_updater, daemon=True).start()

    def on_closing(self):
        self.is_sending = False
        self.close_serial()
        self.root.destroy()

# ----------------------------
# Run App
# ----------------------------
if __name__ == "__main__":
    root = tk.Tk()
    app = FlashToolApp(root)
    root.mainloop()