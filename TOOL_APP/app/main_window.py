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
from app.handlers import i2c_handler
import app.CMD as CMD



class FlashToolApp:

    def __init__(self, root):
        self.root = root
        self.root.title("STM32 Programmer v3.1 - Modular")
        self.root.geometry("1100x850")
        self.root.protocol("WM_DELETE_WINDOW", self.on_closing)

        self.serial_mgr = None
        self.is_sending = False
        self.bin_data = b''
        self.received_flash_data = bytearray()
        self.received_flash_data_read = bytearray()
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
        self.refresh_btn = ttk.Button(top_frame, text="Refresh", command=self.update_ports)
        self.refresh_btn.grid(row=0, column=2, padx=2)
        self.open_btn = ttk.Button(top_frame, text="Open", command=self.open_serial)
        self.open_btn.grid(row=0, column=3, padx=2)
        self.close_btn = ttk.Button(top_frame, text="Close", command=self.close_serial)
        self.close_btn.grid(row=0, column=4, padx=2)

        ttk.Label(top_frame, text="BIN File:").grid(row=1, column=0, sticky='w', pady=(5,0))
        self.file_path = tk.StringVar()
        ttk.Entry(top_frame, textvariable=self.file_path, width=50, state='readonly').grid(row=1, column=1, columnspan=3, pady=(5,0))
        ttk.Button(top_frame, text="Browse...", command=self.browse_file).grid(row=1, column=4, pady=(5,0))

        preview_frame = ttk.LabelFrame(self.flash_tab, text="BIN Content (Full Hex View)")
        preview_frame.pack(pady=5, fill='both', expand=True)

        self.hex_text = scrolledtext.ScrolledText(preview_frame, font=("Courier", 10),wrap='none')
        self.hex_text.pack(fill='both', expand=True)

        btn_frame = ttk.Frame(self.flash_tab)
        btn_frame.pack(pady=5)

        self.send_btn = ttk.Button(btn_frame, text="Write to Flash", command=self.send_bin, state='disabled')
        self.send_btn.pack(side='left', padx=5)

        self.read_btn = ttk.Button(btn_frame, text="Read Flash", command=self.read_flash, state='disabled')
        self.read_btn.pack(side='left', padx=5)

        self.save_bin_btn = ttk.Button(btn_frame, text="Save Received BIN", command=self.save_received_bin, state='disabled')
        self.save_bin_btn.pack(side='left', padx=5)

        self.progress = ttk.Progressbar(btn_frame, orient='horizontal', length=300, mode='determinate')
        self.progress.pack(side='left', padx=10)
        self.speed_label = ttk.Label(btn_frame, text="0 KB/s")
        self.speed_label.pack(side='left')

        # Tab 2: I2C/SPI Control
        self.periph_tab = ttk.Frame(self.notebook)
        self.notebook.add(self.periph_tab, text="I2C/SPI")

        i2c_frame = ttk.LabelFrame(self.periph_tab, text="I2C Register Read")
        i2c_frame.pack(pady=10, padx=10, fill='x')

        #创建I2C设备地址的框
        ttk.Label(i2c_frame, text="Device Addr (0x):").grid(row=0, column=0, sticky='w')
        self.i2c_addr = tk.StringVar(value="A0")
        ttk.Entry(i2c_frame, textvariable=self.i2c_addr, width=6).grid(row=0, column=1, padx=5)

        #创建I2C寄存器地址的框
        ttk.Label(i2c_frame, text="Reg Addr (0x):").grid(row=0, column=2, sticky='w')
        self.i2c_reg = tk.StringVar(value="00")
        ttk.Entry(i2c_frame, textvariable=self.i2c_reg, width=8).grid(row=0, column=3, padx=5)

        #创建I2C写数据的框
        ttk.Label(i2c_frame, text="Write Data (0x):").grid(row=0, column=4, sticky='w')
        self.i2c_write_data = tk.StringVar(value="00")
        ttk.Entry(i2c_frame, textvariable=self.i2c_write_data, width=6).grid(row=0, column=5, padx=5)
        #创建I2C读取按钮的框
        # self.i2c_read_btn = ttk.Button(i2c_frame, text="Read", command=self.i2c_read, state='disabled')
        self.i2c_read_btn = ttk.Button(i2c_frame, text="Read", command=lambda: i2c_handler.i2c_read(self), state='disabled')
        self.i2c_read_btn.grid(row=0, column=6, padx=5)

        #创建I2C写按钮的框
        # self.i2c_write_btn = ttk.Button(i2c_frame, text="Write", command=self.i2c_write, state='disabled')
        self.i2c_write_btn = ttk.Button(i2c_frame, text="Write", command=lambda: i2c_handler.i2c_write(self), state='disabled')
        self.i2c_write_btn.grid(row=0, column=7, padx=5)

        #创建I2C 8位或是16位寄存器地址选择
        self.i2c_addr_size = tk.StringVar(value="8")
        ttk.Radiobutton(i2c_frame, text="8-bit", variable=self.i2c_addr_size, value="8").grid(row=0, column=8, sticky='w')
        ttk.Radiobutton(i2c_frame, text="16-bit", variable=self.i2c_addr_size, value="16").grid(row=0, column=9, sticky='w')

        # self.i2c_result = tk.StringVar()
        # ttk.Label(i2c_frame, textvariable=self.i2c_result, foreground='blue').grid(row=0, column=8, padx=5)

        # EEPROM 加载 bin 文件 (I2C/SPI Tab)
        ttk.Label(i2c_frame, text="BIN File:").grid(row=1, column=0, sticky='w', pady=(5,0))
        self.eeprom_file_path = tk.StringVar()
        ttk.Entry(i2c_frame, textvariable=self.eeprom_file_path, width=50, state='readonly').grid(row=1, column=1, columnspan=3, pady=(5,0))
        ttk.Button(i2c_frame, text="Browse...", command=lambda: self.browse_file(target_var=self.eeprom_file_path)).grid(row=1, column=4, pady=(5,0))

        # 如果你需要在 I2C/SPI Tab 也显示 BIN 内容的预览
        eeprom_preview_frame = ttk.LabelFrame(self.periph_tab, text="EEPROM Content (Full Hex View)")
        eeprom_preview_frame.pack(pady=5, fill='both', expand=True)

        self.eeprom_hex_text = scrolledtext.ScrolledText(eeprom_preview_frame, font=("Courier", 15))
        self.eeprom_hex_text.pack(fill='both', expand=True)

        # 创建一个独立的按钮容器 Frame（放在 preview 下方）
        eeprom_btn_frame = ttk.Frame(self.periph_tab)
        eeprom_btn_frame.pack(pady=5)
        
        #创建一个框用来输入EEPROM的设备地址
        ttk.Label(eeprom_btn_frame, text="EEPROM Addr (0x):").pack(side='left', padx=5)
        self.eeprom_addr = tk.StringVar(value="A0")
        ttk.Entry(eeprom_btn_frame, textvariable=self.eeprom_addr, width=6).pack(side='left', padx=5)
        #创建一个下拉框选择EEPROM型号
        ttk.Label(eeprom_btn_frame, text="EEPROM Model:").pack(side='left', padx=5)
        self.eeprom_model = tk.StringVar(value="AT24C02")
        eeprom_model_combo = ttk.Combobox(eeprom_btn_frame, textvariable=self.eeprom_model, values=["AT24C02", "AT24C04", "AT24C08", "AT24C16", "AT24C32", "AT24C64", "AT24C128"], state='readonly')
        eeprom_model_combo.pack(side='left', padx=5)

        # 将按钮放入这个新 Frame 中
        self.eeprom_write_btn = ttk.Button(
            eeprom_btn_frame,
            text="Write EEPROM",
            command=lambda: i2c_handler.i2c_write_eeprom(self),
            state='disabled'
        )
        self.eeprom_write_btn.pack(side='left', padx=10)
        
        self.eeprom_read_btn = ttk.Button(
            eeprom_btn_frame,
            text="Read EEPROM",
            command=lambda: i2c_handler.i2c_read_eeprom(self),
            state='disabled'
        )
        self.eeprom_read_btn.pack(side='left', padx=10)

        # #SPI
        # spi_frame = ttk.LabelFrame(self.periph_tab, text="SPI Register Read")
        # spi_frame.pack(pady=10, padx=10, fill='x')
        # ttk.Label(spi_frame, text="Reg Addr (0x):").grid(row=0, column=0, sticky='w')
        # self.spi_reg = tk.StringVar(value="00")
        # ttk.Entry(spi_frame, textvariable=self.spi_reg, width=10).grid(row=0, column=1, padx=5)
        # self.spi_read_btn = ttk.Button(spi_frame, text="Read", command=self.spi_read, state='disabled')
        # self.spi_read_btn.grid(row=0, column=2, padx=5)
        # self.spi_result = tk.StringVar()
        # ttk.Label(spi_frame, textvariable=self.spi_result, foreground='blue').grid(row=0, column=3, padx=5)

        # led_frame = ttk.Frame(self.periph_tab)
        # led_frame.pack(pady=10)
        # self.led_btn = ttk.Button(led_frame, text="Toggle LED", command=self.toggle_led, state='disabled')
        # self.led_btn.pack()

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
        self.save_bin_btn.config(state='normal')
        self.i2c_read_btn.config(state='normal')
        # self.spi_read_btn.config(state='normal')
        # self.led_btn.config(state='normal')
        self.i2c_write_btn.config(state='normal')
        self.open_btn.config(state='disabled')
        self.close_btn.config(state='normal')

    def _disable_buttons(self):
        self.send_btn.config(state='disabled')
        self.read_btn.config(state='disabled')
        self.save_bin_btn.config(state='disabled')
        self.i2c_read_btn.config(state='disabled')
        # self.spi_read_btn.config(state='disabled')
        # self.led_btn.config(state='disabled')
        self.i2c_write_btn.config(state='disabled')
        self.open_btn.config(state='normal')
        self.close_btn.config(state='disabled')

    # 修改 browse_file 方法以支持目标变量参数
    def browse_file(self, target_var=None):
        path = filedialog.askopenfilename(filetypes=[("Binary files", "*.bin"), ("All files", "*.*")])
        if path:
            if target_var is None:
                # 默认为 Flash 编程的路径
                self.file_path.set(path)
                self.bin_data = open(path, 'rb').read()
                self.hex_text.delete(1.0, tk.END)
                self.hex_text.insert(tk.END, format_hex_preview(self.bin_data))
            else:
                # 为 EEPROM 加载的路径
                target_var.set(path)
                self.bin_data = open(path, 'rb').read()
                self.eeprom_hex_text.delete(1.0, tk.END)
                self.eeprom_hex_text.insert(tk.END, format_hex_preview(self.bin_data))
            self._log_to_queue(f"Loaded {len(self.bin_data)} bytes from {path}")

    # ========================
    # Robust Send with Retry
    # ========================
    def handle_device_disconnect(self):
        self.is_sending = False
        # if self.serial_mgr:
        #     self.serial_mgr.shutdown()
        #     self.serial_mgr = None
        # self.root.after(0, self._disable_buttons)
        # self.root.after(0, lambda: messagebox.showwarning(
        #     "Device Disconnected",
        #     "No response from device after 3 retries.\n"
        #     "Please check connection and reopen serial port."
        # ))
        self._log_to_queue("⚠️ Device disconnected or No ACK received.")

    def send_with_retry(self, cmd_id: int, payload: bytes, expected_response_cmd: int = None, ack_required=True) -> Optional[bytes]:
        '''
        发送带有重试机制的HDLC帧命令
        
        该函数会通过串口发送HDLC帧格式的命令，并根据需要等待设备响应。如果未收到预期响应，
        将自动重试最多3次。若所有尝试都失败，则判定设备断开连接并进行相应处理。
        
        :param self: 类实例本身
        :param cmd_id: 命令ID，用于标识要执行的操作
        :type cmd_id: int
        :param payload: 要发送的数据载荷
        :type payload: bytes
        :param expected_response_cmd: 期望接收到的响应命令ID，默认为None表示不需要特定响应
        :type expected_response_cmd: int
        :param ack_required: 是否需要确认响应，如果为False则只发送不等待响应，默认为True
        :type ack_required: bool
        :return: 如果成功接收到响应则返回响应数据，否则返回None；如果ack_required为False则返回空字节
        :rtype: bytes | None
        '''
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
        total_blocks = (total_size + block_size - 1) // block_size  # 向上取整除法
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
                CMD.CMD_WRITE_FLASH_BLOCK,
                payload,
                expected_response_cmd=CMD.CMD_WRITE_FLASH_ACK
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

        #TODO 添加处理扇区擦除等操作

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
        data = bytearray()
        # 清空之前的读取数据缓存
        self.received_flash_data_read.clear()

        for i in range(total_blocks):
            if not self.is_sending:
                break
            start = addr + i * block_size
            blk_len = min(block_size, length - i * block_size)
            payload = struct.pack('>IH', start, blk_len)
            resp = self.send_with_retry(
                CMD.CMD_READ_FLASH_REQUEST,
                payload,
                expected_response_cmd=CMD.CMD_READ_FLASH_RESPONSE
            )
            if resp is None:
                return
            #每次循环都将resp的所有数据加入到received_flash_data_read中，循环结束处理
            self.received_flash_data_read.extend(resp)
            
            
            # 注意：实际项目中需在 CMD_READ_FLASH_RESPONSE 处理时 append 到 self.received_flash_data
            # 此处为简化，假设 STM32 返回的数据已通过某种方式存入 self.received_flash_data
            # 实际应修改 SerialManager 或增加回调机制 —— 本 demo 暂略

        #打印data到log框中

        #将读到的数据显示在hex预览框中
        self.root.after(0, lambda: self.hex_text.delete(1.0, tk.END))
        self.root.after(0, lambda: self.hex_text.insert(tk.END, format_hex_preview(self.received_flash_data_read)))
        self._log_to_queue(f"✅ Read complete: {len(self.received_flash_data_read)} bytes")

        # 保存文件
        # save_path = filedialog.asksaveasfilename(defaultextension=".bin", filetypes=[("Binary files", "*.bin")])
        # if save_path:
        #     with open(save_path, 'wb') as f:
        #         f.write(data)
        #     self._log_to_queue(f"💾 Saved {len(data)} bytes to {save_path}")

        self.is_sending = False
        self.root.after(0, lambda: self.read_btn.config(state='normal'))


    def save_received_bin(self):
        if not self.received_flash_data_read:  # 检查是否已接收到数据
            messagebox.showwarning("Warning", "No received flash data to save")
            return
        self.save_bin_btn.config(state='disabled')
        save_path = filedialog.asksaveasfilename(defaultextension=".bin", filetypes=[("Binary files", "*.bin")])
        if save_path:
            with open(save_path, 'wb') as f:
                f.write(self.received_flash_data_read)
            self._log_to_queue(f"💾 Saved {len(self.received_flash_data_read)} bytes to {save_path}")
        self.save_bin_btn.config(state='normal')



    # ========================
    # Peripheral Control
    # ========================
    # def toggle_led(self):
    #     payload = bytes([0, 1])
    #     self.send_with_retry(CMD.CMD_LED_CONTROL, payload, ack_required=False)
    #     self._log_to_queue("Sent LED ON command")

    # def i2c_read(self):
    #     # 检查I2C地址是否为空
    #     if not self.i2c_addr.get():
    #         messagebox.showerror("Error", "I2C address is empty")
    #         return
    #     try:
    #         dev_addr = int(self.i2c_addr.get(), 16)
    #         reg_addr = int(self.i2c_reg.get(), 16)
    #     except Exception as e:
    #         messagebox.showerror("Error", f"Invalid I2C address: {e}")
    #         return
    #     # 禁用按钮防止重复点击
    #     self.i2c_read_btn.config(state='disabled')
    #     if self.i2c_addr_size.get() == "8":
    #         self._log_to_queue(f"I2C Read: Dev=0x{dev_addr:02X}, Reg=0x{reg_addr:02X}")
    #     else:
    #         self._log_to_queue(f"I2C Read: Dev=0x{dev_addr:02X}, Reg=0x{reg_addr:04X}")

    #     # 启动后台线程
    #     threading.Thread(target=self._i2c_read_worker, args=(dev_addr, reg_addr), daemon=True).start()

    # def _i2c_read_worker(self, dev_addr: int, reg_addr: int):
    #     if self.i2c_addr_size.get() == "8":
    #         payload = bytes([dev_addr, reg_addr])
    #     else:
    #         payload = bytes([dev_addr, reg_addr >> 8, reg_addr & 0xFF])
    #     if self.i2c_addr_size.get() == "8":
    #         resp = self.send_with_retry(
    #             CMD.CMD_I2C_READ_REG,
    #             payload,
    #             expected_response_cmd=CMD.CMD_I2C_READ_RESULT
    #         )
    #     else:
    #         resp = self.send_with_retry(
    #             CMD.CMD_I2C_16READ_REG,
    #             payload,
    #             expected_response_cmd=CMD.CMD_I2C_READ_RESULT
    #         )
    #     # 回到主线程更新 UI
    #     self.root.after(0, self._on_i2c_read_complete, resp, dev_addr, reg_addr)

    # def _on_i2c_read_complete(self, resp, dev_addr, reg_addr):
    #     self.i2c_read_btn.config(state='normal')  # 恢复按钮
    #     if resp is not None and len(resp) >= 1:
    #         value = resp[0]
    #         self.i2c_write_data.set(f"0x{value:02X}")
    #         # self.i2c_result.set(f"0x{value:02X} ({value})")
    #         self._log_to_queue(f"I2C Read OK: 0x{value:02X}")
    #     else:
    #         #弹出窗口提示I2C读取超时
    #         messagebox.showerror("Error", f"I2C Read Timeout")

    #         # self.i2c_result.set("Timeout")
    #         # 注意：device disconnect 已在 send_with_retry 中处理，这里无需重复弹窗

    # def i2c_write(self):
    #     try:
    #         dev_addr = int(self.i2c_addr.get(), 16)
    #         reg_addr = int(self.i2c_reg.get(), 16)
    #         write_data = int(self.i2c_write_data.get(), 16)
    #     except Exception as e:
    #         messagebox.showerror("Error", f"Invalid I2C address or write data: {e}")
    #         return

    #     self.i2c_write_btn.config(state='disabled')
    #     self._log_to_queue(f"I2C Write: Dev=0x{dev_addr:02X}, Reg=0x{reg_addr:02X}, Data=0x{write_data:02X}")
    #      # 启动后台线程
    #     threading.Thread(target=self._i2c_write_worker, args=(dev_addr, reg_addr, write_data), daemon=True).start()

    # def _i2c_write_worker(self, dev_addr: int, reg_addr: int, write_data: int):
    #     payload = bytes([dev_addr, reg_addr, write_data])
    #     resp = self.send_with_retry(
    #         CMD.CMD_I2C_WRITE_REG,
    #         payload,
    #         expected_response_cmd=CMD.CMD_I2C_WRITE_ACK
    #     )
    #     # 回到主线程更新 UI
    #     self.root.after(0, self._on_i2c_write_complete, resp, dev_addr, reg_addr, write_data)

    # def _on_i2c_write_complete(self, resp, dev_addr, reg_addr, write_data):
    #     self.i2c_write_btn.config(state='normal')
    #     if resp is not None:
    #         self._log_to_queue(f"I2C Write OK: Dev=0x{dev_addr:02X}, Reg=0x{reg_addr:02X}, Data=0x{write_data:02X}")
    #     else:
    #         self._log_to_queue(f"I2C Write Failed: Dev=0x{dev_addr:02X}, Reg=0x{reg_addr:02X}, Data=0x{write_data:02X}")

    # def spi_read(self):
    #     try:
    #         reg_addr = int(self.spi_reg.get(), 16)
    #     except Exception as e:
    #         messagebox.showerror("Error", f"Invalid SPI register: {e}")
    #         return

    #     self.spi_read_btn.config(state='disabled')
    #     self._log_to_queue(f"SPI Read: Reg=0x{reg_addr:02X}")
    #     threading.Thread(target=self._spi_read_worker, args=(reg_addr,), daemon=True).start()

    # def _spi_read_worker(self, reg_addr: int):
    #     payload = bytes([reg_addr])
    #     resp = self.send_with_retry(
    #         CMD.CMD_SPI_READ_REG,
    #         payload,
    #         expected_response_cmd=CMD.CMD_SPI_READ_RESULT
    #     )
    #     self.root.after(0, self._on_spi_read_complete, resp, reg_addr)

    # def _on_spi_read_complete(self, resp, reg_addr):
    #     self.spi_read_btn.config(state='normal')
    #     if resp is not None and len(resp) >= 1:
    #         value = resp[0]
    #         self.spi_result.set(f"0x{value:02X} ({value})")
    #         self._log_to_queue(f"SPI Read OK: 0x{value:02X}")
    #     else:
    #         self.spi_result.set("Timeout")

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