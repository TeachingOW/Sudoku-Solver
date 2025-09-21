### Project Instructions

1. **TCP Server (Python)**

   * Implement a TCP server & Websocket in Python (`server.py`).
   * The server acts as a simple chat relay.
   * Only two clients will connect:

     * **Writer client** (sends messages).
     * **Reader client** (receives messages).
   * The server does not interpret messages. It only forwards them between the two clients.

---

2. **Writer Client (C++)**

   * Implement a TCP client in C++ (`sudoku_client.cpp`).
   * This client sends messages to the Python TCP server.
   * Each message must follow the format:

     ```
     (row, column, value, color)
     ```
   * Example: `(2, 3, 5, red)` would mean placing the number `5` in row `2`, column `3` with the color `red`.

---

3. **Reader Client (HTML/JavaScript)**

   * Implement a reader client using **HTML + JavaScript**.
   * This client receives updates from the Python TCP server.
   * The interface should display a **9×9 Sudoku grid** (with 3×3 subgrids).
   * Grid requirements:

     * Make the grid visually appealing (“beautify” it).
     * When a received message has `value = 0`, remove the value from the corresponding cell (leave the cell empty).
     * Apply the received `color` to the updated cell.

---

✅ Deliverables:

* `server.py` → Python TCP server
* `sudoku_client.cpp` → C++ writer client
* `index.html` + JavaScript → Reader client with Sudoku UI

