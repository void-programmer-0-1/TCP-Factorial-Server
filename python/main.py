import asyncio
import fastmath
from concurrent.futures import ThreadPoolExecutor


executor = ThreadPoolExecutor()


async def handler(reader, writer):

    addr = writer.get_extra_info("peername")
    print(f"Connected to {addr}")

    try:
        data = await reader.readline()
        number = int(data.decode().strip())

        loop = asyncio.get_running_loop()

        result = await loop.run_in_executor(
            executor=executor,
            fastmath.fib,
            number
        )
    
    except Exception as err:    
        writer.write(f"Error {str(err)}\n".encode())
    
    finally:
        writer.close()
        await writer.wait_closed()
        print(f"Closed: {addr}")


async def main():

    server = await asyncio.start_server(
        handler,
        "127.0.0.1",
        8000
    )

    print("Async TCP Server running in port 8000")

    async with server:
        await server.serve_forever()
    

if __name__ == "__main__":
    asyncio.run(main())
