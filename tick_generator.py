#!/usr/bin/env python3
import socket, time, argparse, csv, random
from datetime import datetime

def now_ns():
    return int(time.time() * 1e9)

def run(port=9000, rate_hz=5000):
    # simple orderbook base price that drifts
    base = 30000.0
    srv = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    srv.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    srv.bind(('0.0.0.0', port))
    srv.listen(1)
    print("Waiting for connection on port", port)
    conn, addr = srv.accept()
    print("Connected:", addr)
    interval = 1.0 / rate_hz
    try:
        while True:
            # generate synthetic top-of-book bid/ask ticks alternately
            side = random.choice([0,1])
            spread = 0.5 + random.random()*0.5
            if side == 0:
                price = base - spread*0.5 + random.random()*0.1
            else:
                price = base + spread*0.5 + random.random()*0.1
            ts = now_ns()
            size = 0.001 + random.random()*0.01
            line = f"{ts},{price:.2f},{size:.6f},{side}\n"
            conn.sendall(line.encode())
            time.sleep(interval)
            # optional slight base drift
            base += (random.random()-0.5)*0.01
    except BrokenPipeError:
        print("Client disconnected")
    finally:
        conn.close()
        srv.close()

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("--port", type=int, default=9000)
    parser.add_argument("--rate", type=int, default=5000, help="ticks per second")
    args = parser.parse_args()
    run(args.port, args.rate)