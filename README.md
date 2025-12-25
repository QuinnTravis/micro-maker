# Micro-Maker: Market-Making Simulator

## Overview
**Micro-Maker** is a market-making simulator that demonstrates real-time trading logic, strategy backtesting, and optional high-performance simulation.  
It is designed to showcase both **Python-driven prototyping** and **systems-level C++ programming**, making it a strong portfolio piece for quantitative trading or software engineering roles.

---

## Features
- Python tick generator simulates top-of-book bid/ask ticks.
- Python backtesting framework evaluates strategy performance and logs orders.
- Optional C++ tick generator & lock-free ring buffer for low-latency simulation.
- Modular code structure for easy extension and testing.
- CSV outputs for further analysis and visualization.

---

## Folder Structure

micro-maker/
├── README.md
├── src/
│ ├── python/ # Main runnable Python scripts
│ │ ├── tick_generator.py
│ │ └── backtest.py
│ └── cpp/ # Optional high-performance C++ demo
│ ├── main.cpp
│ ├── ring_buffer.h
│ └── types.h
├── data/ # Sample input CSVs
├── results/ # Output plots / CSVs
├── tests/ # Optional unit tests
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
The backtester reads simulated ticks and logs orders to results/ for analysis.

Optional C++ Demo
Compile and run the high-performance version:

bash
Copy code
cd src/cpp/
g++ main.cpp -o micro_maker
./micro_maker 127.0.0.1 9000
