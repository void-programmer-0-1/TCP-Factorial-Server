import asyncio
import uvloop

# ---------- CONFIG ----------
HOST = "127.0.0.1"
PORT = 9000
MAX_N = 1000   # safety limit
# ----------------------------

asyncio.set_event_loop_policy(uvloop.EventLoopPolicy())

# Precompute factorials once
FACT = [1] * (MAX_N + 1)
for i in range(2, MAX_N + 1):
    FACT[i] = FACT[i - 1] * i


async def handle_client(reader: asyncio.StreamReader,
                        writer: asyncio.StreamWriter):
    addr = writer.get_extra_info("peername")

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
                    writer.write(str(FACT[n]).encode() + b"\n")

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

    print(f"Pure Python optimized server on {PORT}")

    async with server:
        await server.serve_forever()


if __name__ == "__main__":
    asyncio.run(main())
