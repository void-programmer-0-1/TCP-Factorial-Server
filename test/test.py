import asyncio
import time
import statistics

HOST = "127.0.0.1"
PORT = 8080
N = 50

TOTAL_REQUESTS = 5000
CONCURRENCY = 100


latencies = []


async def one_request(sem):
    async with sem:
        start = time.perf_counter()
        reader, writer = await asyncio.open_connection(HOST, PORT)
        writer.write(f"{N}\n".encode())
        await writer.drain()
        await reader.read(1024 * 1024)
        writer.close()
        await writer.wait_closed()
        end = time.perf_counter()

        latencies.append((end - start) * 1000)  # ms


async def main():
    sem = asyncio.Semaphore(CONCURRENCY)

    start_time = time.perf_counter()

    tasks = [asyncio.create_task(one_request(sem)) for _ in range(TOTAL_REQUESTS)]
    await asyncio.gather(*tasks)

    total_time = time.perf_counter() - start_time

    # ----- METRICS -----
    latencies.sort()
    p50 = latencies[int(0.50 * len(latencies))]
    p95 = latencies[int(0.95 * len(latencies))]
    p99 = latencies[int(0.99 * len(latencies))]

    avg_latency = statistics.mean(latencies)
    rps = TOTAL_REQUESTS / total_time

    print("\n===== LOAD TEST RESULTS =====")
    print(f"Total Requests      : {TOTAL_REQUESTS}")
    print(f"Concurrency         : {CONCURRENCY}")
    print(f"Total Time          : {total_time:.2f} s")
    print(f"Throughput (RPS)    : {rps:.2f}")
    print(f"Latency avg         : {avg_latency:.3f} ms")
    print(f"Latency p50         : {p50:.3f} ms")
    print(f"Latency p95         : {p95:.3f} ms")
    print(f"Latency p99         : {p99:.3f} ms")
    print("==============================\n")


asyncio.run(main())
