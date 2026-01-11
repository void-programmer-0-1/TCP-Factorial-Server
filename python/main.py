import asyncio
import uvloop
import factorial
import os

HOST = "127.0.0.1"
PORT = 9000
MAX_N = 5000

asyncio.set_event_loop_policy(uvloop.EventLoopPolicy())

async def handle_client(reader: asyncio.StreamReader,
                        writer: asyncio.StreamWriter):
    try:
        while True:
            data = await reader.readline()
            if not data:
                break  # client closed

            try:
                n = int(data)
                if n < 0 or n > MAX_N:
                    writer.write(b"ERROR\n")
                else:
                    res = factorial.factorial(n)
                    writer.write(res + b"\n")
            except Exception:
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
        reuse_port=True,
        backlog=4096
    )

    print(f"Worker {os.getpid()} listening on {PORT}")

    async with server:
        await server.serve_forever()


if __name__ == "__main__":
    asyncio.run(main())
