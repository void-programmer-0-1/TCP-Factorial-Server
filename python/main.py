import asyncio
import factorial
from concurrent.futures import ThreadPoolExecutor

executor = ThreadPoolExecutor()

async def handle_client(reader, writer):
    addr = writer.get_extra_info("peername")
    print(f"Client connected: {addr}")

    try:
        data = await reader.readline()
        n = int(data.decode().strip())

        loop = asyncio.get_running_loop()

        # Call Cython+C++ Fibonacci without blocking event loop
        result = await loop.run_in_executor(
            executor,
            factorial.factorial,
            n
        )

        writer.write(f"{result}\n".encode())
        await writer.drain()

    except Exception as e:
        writer.write(f"Error: {e}\n".encode())

    finally:
        writer.close()
        await writer.wait_closed()
        print(f"Client disconnected: {addr}")

async def main():
    server = await asyncio.start_server(
        handle_client,
        host="127.0.0.1",
        port=9000
    )

    print("Fibonacci server running on port 9000")

    async with server:
        await server.serve_forever()

if __name__ == "__main__":
    asyncio.run(main())
