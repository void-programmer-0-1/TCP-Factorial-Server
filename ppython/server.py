import asyncio
import uvloop
import math

# ---------- CONFIG ----------
HOST = "127.0.0.1"
PORT = 9000
MAX_N = 5000   # safety limit (factorial grows fast)
# ----------------------------

asyncio.set_event_loop_policy(uvloop.EventLoopPolicy())


async def handle_client(reader: asyncio.StreamReader,
                        writer: asyncio.StreamWriter):
    try:
        while True:
            data = await reader.readline()
            if not data:
                break  # EOF

            try:
                n = int(data)
                if n < 0 or n > MAX_N:
                    writer.write(b"ERROR\n")
                else:
                    # 🔥 REAL COMPUTATION HERE
                    result = math.factorial(n)
                    writer.write(str(result).encode() + b"\n")

            except ValueError:
                writer.write(b"ERROR\n")

            await writer.drain()

    finally:
        writer.close()
        await writer.wait_closed()


async def main():
    server = await asyncio.start_server(
        handle_client,
        HOST,
        PORT,
        backlog=4096
    )

    print(f"Pure Python COMPUTE server on port {PORT}")

    async with server:
        await server.serve_forever()


if __name__ == "__main__":
    asyncio.run(main())
