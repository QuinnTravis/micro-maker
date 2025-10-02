#!/usr/bin/env python3
import csv, sys, argparse

def run(csvfile):
    pos = 0.0
    pnl = 0.0
    fee = 0.0
    bid = None
    ask = None
    with open(csvfile) as f:
        reader = csv.reader(f)
        for row in reader:
            ts, price, size, side = row
            price = float(price); side = int(side)
            # update top-of-book
            if side==0:
                bid = price
            else:
                ask = price
            # strategy: we always have passive limit orders at mid +- small
            if bid and ask:
                mid = (bid+ask)/2
                # naive fill: if price crosses our buy price, we get filled
                buy_price = mid - 0.1
                sell_price = mid + 0.1
                # if incoming tick is an ask tick and price <= buy_price, we got hit by someone selling to our buy
                if side==1 and price <= buy_price:
                    pos += 0.01
                    pnl -= buy_price*0.01
                # if incoming tick is a bid tick and price >= sell_price, our sell executed
                if side==0 and price >= sell_price:
                    pos -= 0.01
                    pnl += sell_price*0.01
    print(f"Final pos={pos} pnl={pnl}")
    return pnl

if __name__ == "__main__":
    import os
    default_csv = os.path.join(os.path.dirname(__file__), "orders.csv")
    import argparse
    parser = argparse.ArgumentParser()
    parser.add_argument("csvfile", nargs="?", default=default_csv)
    args = parser.parse_args()
    run(args.csvfile)