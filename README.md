# Micro-Maker: Market-Making Simulator

## Overview
**Micro-Maker** is a market-making simulator designed to demonstrate both **real-time trading logic** and **performance-aware programming**.  
It includes a Python-based tick generator and backtester, with an optional C++ implementation for high-performance simulation.

This project is ideal for demonstrating skills relevant to **quantitative trading** and **software engineering internships**.

---

## Features
- Python tick generator simulates top-of-book bid/ask ticks.
- Python backtesting framework evaluates strategy performance and logs orders.
- Optional C++ tick generator & ring buffer for low-latency, high-throughput simulation.
- Modular and well-organized code structure for easy extension.
- CSV outputs for analysis and visualization.

---

## Folder Structure

micro-maker/
├── README.md
├── src/
│ ├── python/ # main runnable scripts
│ │ ├── tick_generator.py
│ │ └── backtest.py
│ └── cpp/ # optional high-performance demo
│ ├── main.cpp
│ ├── ring_buffer.h
│ └── types.h
├── data/ # sample input CSVs
├── results/ # output plots / CSVs
├── tests/ # optional unit tests
├── requirements.txt # Python dependencies
└── .gitignore

yaml
Copy code

---

## Python Demo

### Install Dependencies
```bash
pip install -r requirements.txt
Run the Tick Generator
bash
Copy code
python src/python/tick_generator.py --port 9000 --rate 5000
Run the Backtester
bash
Copy code
python src/python/backtest.py

Optional C++ Demo

Compile and run the high-performance version:

cd src/cpp/
g++ main.cpp -o micro_maker
./micro_maker 127.0.0.1 9000
