import asyncio
import time
import statistics

HOST = "127.0.0.1"
PORT = 9000
N = 50

CONCURRENCY = 100
REQUESTS_PER_CONN = 50 
TOTAL_REQUESTS = CONCURRENCY * REQUESTS_PER_CONN

latencies = []

async def worker():
    reader, writer = await asyncio.open_connection(HOST, PORT)

    for _ in range(REQUESTS_PER_CONN):
        start = time.perf_counter()
        writer.write(f"{N}\n".encode())
        await writer.drain()
        await reader.readline()
        end = time.perf_counter()
        latencies.append((end - start) * 1000)

    writer.close()
    await writer.wait_closed()

async def main():
    start_time = time.perf_counter()

    tasks = [asyncio.create_task(worker()) for _ in range(CONCURRENCY)]
    await asyncio.gather(*tasks)

    total_time = time.perf_counter() - start_time

    latencies.sort()
    p50 = latencies[int(0.50 * len(latencies))]
    p95 = latencies[int(0.95 * len(latencies))]
    p99 = latencies[int(0.99 * len(latencies))]

    rps = TOTAL_REQUESTS / total_time

    print("\n===== LOAD TEST RESULTS =====")
    print(f"Total Requests      : {TOTAL_REQUESTS}")
    print(f"Concurrency         : {CONCURRENCY}")
    print(f"Requests / Conn     : {REQUESTS_PER_CONN}")
    print(f"Total Time          : {total_time:.2f} s")
    print(f"Throughput (RPS)    : {rps:.2f}")
    print(f"Latency avg         : {statistics.mean(latencies):.3f} ms")
    print(f"Latency p50         : {p50:.3f} ms")
    print(f"Latency p95         : {p95:.3f} ms")
    print(f"Latency p99         : {p99:.3f} ms")
    print("==============================\n")

asyncio.run(main())
