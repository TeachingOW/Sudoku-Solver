import socket
import threading
import tkinter as tk

class SudokuServer:
    def __init__(self, host="0.0.0.0", port=12345):
        self.host = host
        self.port = port
        self.root = tk.Tk()
        self.root.title("Sudoku Grid (Shared)")
        self.entries = [[None for _ in range(9)] for _ in range(9)]
        self.build_grid()
        self.start_server()

    def build_grid(self):
        """Create an empty editable Sudoku grid"""
        for r in range(9):
            for c in range(9):
                e = tk.Entry(self.root, width=2, font=("Arial", 20), justify="center")
                e.grid(row=r, column=c, padx=3, pady=3)
                self.entries[r][c] = e

    def update_cell(self, row, col, value):
        """Update a single cell in the grid"""
        if 0 <= row < 9 and 0 <= col < 9:
            self.entries[row][col].delete(0, tk.END)
            if value != 0:
                self.entries[row][col].insert(0, str(value))

    def handle_client(self, conn, addr):
        """Handle one client in a separate thread"""
        print(f"Client connected: {addr}")
        buffer = ""
        with conn:
            while True:
                try:
                    data = conn.recv(1024).decode("utf-8")
                except ConnectionResetError:
                    break
                if not data:
                    break
                buffer += data
                while "\n" in buffer:
                    line, buffer = buffer.split("\n", 1)
                    parts = line.strip().split()
                    if len(parts) == 3:
                        try:
                            row, col, val = map(int, parts)
                            print(f"[{addr}] row={row}, col={col}, val={val}")
                            self.root.after(0, self.update_cell, row, col, val)
                        except ValueError:
                            print(f"[{addr}] Invalid integers:", parts)
                    else:
                        print(f"[{addr}] Ignored malformed:", line)
        print(f"Client disconnected: {addr}")

    def start_server(self):
        """Run server socket accepting many clients"""
        def run():
            with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as server:
                server.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
                server.bind((self.host, self.port))
                server.listen()
                print(f"Server listening on {self.host}:{self.port}")
                while True:
                    conn, addr = server.accept()
                    threading.Thread(target=self.handle_client, args=(conn, addr), daemon=True).start()

        threading.Thread(target=run, daemon=True).start()

    def run(self):
        self.root.mainloop()


if __name__ == "__main__":
    SudokuServer().run()
