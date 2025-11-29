# app/main_window.py

import tkinter as tk
from tkinter import ttk, filedialog, scrolledtext, messagebox, simpledialog
import threading
import time
import struct
from typing import Optional
from utils.hex_viewer import format_hex_preview
from app.serial_manager import SerialManager
from protocol.hdlc import build_hdlc_frame


class FlashToolApp:
    CMD_WRITE_FLASH_BLOCK = 0x1001
    CMD_WRITE_FLASH_ACK = 0x1004
    CMD_READ_FLASH_REQUEST = 0x1002
    CMD_READ_FLASH_RESPONSE = 0x1003
    CMD_LED_CONTROL = 0x2001
    CMD_I2C_READ_REG = 0x2002
    CMD_I2C_READ_RESULT = 0x2003
    CMD_SPI_READ_REG = 0x2004
    CMD_SPI_READ_RESULT = 0x2005

    def __init__(self, root):
        self.root = root
        self.root.title("STM32 Programmer v3.1 - Modular")
        self.root.geometry("1100x850")
        self.root.protocol("WM_DELETE_WINDOW", self.on_closing)

        self.serial_mgr = None
        self.is_sending = False
        self.bin_data = b''
        self.received_flash_data = bytearray()
        self.log_queue = []

        self.create_widgets()
        self.update_ports()
        self.start_log_thread()

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
        import serial.tools.list_ports
        ports = [p.device for p in serial.tools.list_ports.comports()]
        self.port_combo['values'] = ports
        if ports and not self.port_var.get():
            self.port_var.set(ports[0])

    def open_serial(self):
        port = self.port_var.get()
        if not port:
            messagebox.showerror("Error", "Please select a serial port")
            return
        self.serial_mgr = SerialManager(self._log_to_queue)
        if self.serial_mgr.open(port):
            self._enable_buttons()
            self._log_to_queue(f"Serial opened: {port}")
        else:
            self.serial_mgr = None
            messagebox.showerror("Serial Error", "Failed to open serial port")

    def close_serial(self):
        if self.serial_mgr:
            self.serial_mgr.shutdown()
            self.serial_mgr = None
        self._disable_buttons()
        self._log_to_queue("Serial closed")

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
                self.hex_text.delete(1.0, tk.END)
                self.hex_text.insert(tk.END, format_hex_preview(self.bin_data))
                self._log_to_queue(f"Loaded {len(self.bin_data)} bytes from {path}")
            except Exception as e:
                messagebox.showerror("File Error", str(e))

    # ========================
    # Robust Send with Retry
    # ========================
    def handle_device_disconnect(self):
        self.is_sending = False
        if self.serial_mgr:
            self.serial_mgr.shutdown()
            self.serial_mgr = None
        self.root.after(0, self._disable_buttons)
        self.root.after(0, lambda: messagebox.showwarning(
            "Device Disconnected",
            "No response from device after 3 retries.\n"
            "Please check connection and reopen serial port."
        ))
        self._log_to_queue("⚠️ Device disconnected: no ACK received.")

    def send_with_retry(self, cmd_id: int, payload: bytes, expected_response_cmd: int = None, ack_required=True) -> Optional[bytes]:
        if not self.serial_mgr or not self.serial_mgr.is_open():
            return None

        if not ack_required:
            self.serial_mgr.send_frame(build_hdlc_frame(cmd_id, payload))
            return b''

        for attempt in range(3):
            self.serial_mgr.send_frame(build_hdlc_frame(cmd_id, payload))
            resp = self.serial_mgr.wait_for_response(expected_response_cmd, timeout=1.0)
            if resp is not None:
                return resp
            self._log_to_queue(f"[Attempt {attempt+1}/3] No response for CMD 0x{cmd_id:04X}")
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
        if not self.serial_mgr or not self.serial_mgr.is_open():
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

        self._log_to_queue(f"Writing {total_size} bytes to Flash...")

        for i in range(total_blocks):
            if not self.is_sending:
                break
            start = i * block_size
            end = min(start + block_size, total_size)
            block = self.bin_data[start:end]
            payload = struct.pack('>IH', start, len(block)) + block

            ack = self.send_with_retry(
                self.CMD_WRITE_FLASH_BLOCK,
                payload,
                expected_response_cmd=self.CMD_WRITE_FLASH_ACK
            )
            if ack is None:
                return

            progress = (end / total_size) * 100
            elapsed = time.time() - start_time
            speed = end / elapsed / 1024 if elapsed > 0 else 0
            self.root.after(0, lambda p=progress, s=speed: (
                self.progress.config(value=p),
                self.speed_label.config(text=f"{s:.1f} KB/s")
            ))

        self.is_sending = False
        self.root.after(0, lambda: self.send_btn.config(state='normal'))
        self._log_to_queue("✅ Write complete!")

    def read_flash(self):
        if not self.serial_mgr or not self.serial_mgr.is_open():
            messagebox.showwarning("Warning", "Serial port not open")
            return

        # 强制主窗口获取焦点并更新
        self.root.focus_force()
        self.root.update_idletasks()

        addr_str = simpledialog.askstring(
            "Read Flash", 
            "Start address (hex):", 
            initialvalue="00000000",
            parent=self.root  # 👈 显式指定父窗口
        )
        if not addr_str:
            return
        try:
            addr = int(addr_str, 16)
        except Exception as e:
            messagebox.showerror("Error", f"Invalid address: {e}")
            return

        # 再次确保焦点和刷新
        self.root.focus_force()
        self.root.update_idletasks()

        length = simpledialog.askinteger(
            "Read Flash", 
            "Length (bytes):", 
            initialvalue=1024,
            minvalue=1,
            maxvalue=65536,
            parent=self.root  # 👈 显式指定父窗口
        )
        if length is None or length <= 0:
            return

        self.received_flash_data = bytearray()
        self.is_sending = True
        self.read_btn.config(state='disabled')
        threading.Thread(target=self._read_flash_worker, args=(addr, length), daemon=True).start()

    def _read_flash_worker(self, addr, length):
        block_size = 256
        total_blocks = (length + block_size - 1) // block_size
        self._log_to_queue(f"Reading {length} bytes from Flash...")

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
            # 注意：实际项目中需在 CMD_READ_FLASH_RESPONSE 处理时 append 到 self.received_flash_data
            # 此处为简化，假设 STM32 返回的数据已通过某种方式存入 self.received_flash_data
            # 实际应修改 SerialManager 或增加回调机制 —— 本 demo 暂略

        data = bytes(self.received_flash_data[:length])
        save_path = filedialog.asksaveasfilename(defaultextension=".bin", filetypes=[("Binary files", "*.bin")])
        if save_path:
            with open(save_path, 'wb') as f:
                f.write(data)
            self._log_to_queue(f"💾 Saved {len(data)} bytes to {save_path}")

        self.is_sending = False
        self.root.after(0, lambda: self.read_btn.config(state='normal'))

    # ========================
    # Peripheral Control
    # ========================
    def toggle_led(self):
        payload = bytes([0, 1])
        self.send_with_retry(self.CMD_LED_CONTROL, payload, ack_required=False)
        self._log_to_queue("Sent LED ON command")

    def i2c_read(self):
        try:
            dev_addr = int(self.i2c_addr.get(), 16)
            reg_addr = int(self.i2c_reg.get(), 16)
        except Exception as e:
            messagebox.showerror("Error", f"Invalid I2C address: {e}")
            return

        # 禁用按钮防止重复点击
        self.i2c_read_btn.config(state='disabled')
        self._log_to_queue(f"I2C Read: Dev=0x{dev_addr:02X}, Reg=0x{reg_addr:02X}")

        # 启动后台线程
        threading.Thread(target=self._i2c_read_worker, args=(dev_addr, reg_addr), daemon=True).start()

    def _i2c_read_worker(self, dev_addr: int, reg_addr: int):
        payload = bytes([dev_addr, reg_addr])
        resp = self.send_with_retry(
            self.CMD_I2C_READ_REG,
            payload,
            expected_response_cmd=self.CMD_I2C_READ_RESULT
        )
        # 回到主线程更新 UI
        self.root.after(0, self._on_i2c_read_complete, resp, dev_addr, reg_addr)

    def _on_i2c_read_complete(self, resp, dev_addr, reg_addr):
        self.i2c_read_btn.config(state='normal')  # 恢复按钮
        if resp is not None and len(resp) >= 1:
            value = resp[0]
            self.i2c_result.set(f"0x{value:02X} ({value})")
            self._log_to_queue(f"I2C Read OK: 0x{value:02X}")
        else:
            self.i2c_result.set("Timeout")
            # 注意：device disconnect 已在 send_with_retry 中处理，这里无需重复弹窗

    def spi_read(self):
        try:
            reg_addr = int(self.spi_reg.get(), 16)
        except Exception as e:
            messagebox.showerror("Error", f"Invalid SPI register: {e}")
            return

        self.spi_read_btn.config(state='disabled')
        self._log_to_queue(f"SPI Read: Reg=0x{reg_addr:02X}")
        threading.Thread(target=self._spi_read_worker, args=(reg_addr,), daemon=True).start()

    def _spi_read_worker(self, reg_addr: int):
        payload = bytes([reg_addr])
        resp = self.send_with_retry(
            self.CMD_SPI_READ_REG,
            payload,
            expected_response_cmd=self.CMD_SPI_READ_RESULT
        )
        self.root.after(0, self._on_spi_read_complete, resp, reg_addr)

    def _on_spi_read_complete(self, resp, reg_addr):
        self.spi_read_btn.config(state='normal')
        if resp is not None and len(resp) >= 1:
            value = resp[0]
            self.spi_result.set(f"0x{value:02X} ({value})")
            self._log_to_queue(f"SPI Read OK: 0x{value:02X}")
        else:
            self.spi_result.set("Timeout")

    # ========================
    # Logging
    # ========================
    def _log_to_queue(self, msg):
        self.log_queue.append(msg)

    def process_log_queue(self):
        while self.log_queue:
            msg = self.log_queue.pop(0)
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
        if self.serial_mgr:
            self.serial_mgr.shutdown()
        self.root.destroy()