# MicroMaker

Low-latency market-making simulator.

## Pipeline
Python tick generator → C++ low-latency engine → orders.csv → Python backtester → PnL chart

## Features
- Processes 200k ticks with ~55µs p50 latency
- Passive market-making orders
- CSV logging for offline backtesting
- Cumulative PnL visualization

## How to run
1. Run the tick generator:
   ```bash
   python3 sim/tick_generator.py --rate 5000
