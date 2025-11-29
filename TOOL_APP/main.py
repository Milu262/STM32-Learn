# main.py
import tkinter as tk
from app.main_window import FlashToolApp

if __name__ == "__main__":
    root = tk.Tk()
    app = FlashToolApp(root)
    root.mainloop()