import asyncio
import websockets

TCP_PORT = 12345
WS_PORT = 6789

# Store the single websocket reader client
ws_clients = set()

async def handle_tcp(reader, writer):
    """Handle TCP writer client"""
    addr = writer.get_extra_info("peername")
    print(f"[TCP] Writer connected from {addr}")
    try:
        while not reader.at_eof():
            data = await reader.readline()
            if not data:
                break
            msg = data.decode().strip()
            print(f"[TCP] Got: {msg}")
            # forward to websocket client(s)
            if ws_clients:
                await asyncio.gather(*(ws.send(msg) for ws in ws_clients))
    except Exception as e:
        print(f"[TCP] Exception: {e}")
    finally:
        writer.close()
        await writer.wait_closed()
        print(f"[TCP] Writer disconnected")

async def handle_websocket(ws):
    """Handle WebSocket reader client"""
    ws_clients.add(ws)
    print("[WS] Reader connected")
    try:
        async for _ in ws:  # reader does not send anything
            pass
    finally:
        ws_clients.remove(ws)
        print("[WS] Reader disconnected")

async def main():
    tcp_server = await asyncio.start_server(handle_tcp, "0.0.0.0", TCP_PORT)
    ws_server = websockets.serve(handle_websocket, "0.0.0.0", WS_PORT)

    print(f"Server running: TCP on {TCP_PORT}, WebSocket on {WS_PORT}")
    async with tcp_server:
        await asyncio.gather(tcp_server.serve_forever(), ws_server)

asyncio.run(main())
