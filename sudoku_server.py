import asyncio
import json
import websockets

TCP_PORT = 12345
WS_PORT = 6789

board = [[0]*9 for _ in range(9)]
ws_clients = set()
board_lock = asyncio.Lock()


async def broadcast(msg):
    if ws_clients:
        await asyncio.gather(*(ws.send(json.dumps(msg)) for ws in ws_clients))


# TCP handler
async def handle_tcp(reader, writer):
    addr = writer.get_extra_info("peername")
    source_id = f"tcp:{addr[0]}:{addr[1]}"
    print(f"[TCP] {addr} connected")
    buffer = ""
    try:
        while not reader.at_eof():
            data = await reader.read(1024)
            if not data:
                break
            buffer += data.decode()
            while "\n" in buffer:
                line, buffer = buffer.split("\n", 1)
                parts = line.strip().split()
                if len(parts) != 3: continue
                try: r, c, v = map(int, parts)
                except ValueError: continue
                async with board_lock:
                    board[r][c] = v
                await broadcast({"type": "update", "row": r, "col": c, "val": v, "source": source_id})
    except:
        pass
    writer.close()
    await writer.wait_closed()
    print(f"[TCP] {addr} disconnected")


async def ws_handler(ws):
    ws_clients.add(ws)
    addr = ws.remote_address
    source_id = f"ws:{addr[0]}:{addr[1]}"
    async with board_lock:
        await ws.send(json.dumps({"type": "snapshot", "board": board}))
    try:
        async for msg in ws:
            try:
                data = json.loads(msg)
                if data["type"] == "update":
                    r, c, v = int(data["row"]), int(data["col"]), int(data["val"])
                    color = data.get("color", None)
                    async with board_lock:
                        board[r][c] = v
                    await broadcast({"type":"update","row":r,"col":c,"val":v,"source":source_id,"color":color})
            except:
                continue
    finally:
        ws_clients.remove(ws)




async def main():
    tcp_server = await asyncio.start_server(handle_tcp, "0.0.0.0", TCP_PORT)
    ws_server = websockets.serve(ws_handler, "0.0.0.0", WS_PORT)
    print(f"TCP server on port {TCP_PORT}, WebSocket on {WS_PORT}")
    async with tcp_server:
        await asyncio.gather(tcp_server.serve_forever(), ws_server)

asyncio.run(main())
