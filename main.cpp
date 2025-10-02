// main.cpp (fixed)
#include <iostream>
#include <string>
#include <thread>
#include <vector>
#include <sstream>
#include <chrono>
#include <atomic>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fstream>
#include <algorithm>
#include <cmath>

#include "../include/ring_buffer.h"
#include "../include/types.h"

using namespace std::chrono;

static std::atomic<bool> running{true};

// simple network reader thread: receives lines "ts_ns,price,size,side\n"
void net_reader_thread(SpscRing<Tick> &rb, const char* host, int port) {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) { perror("socket"); return; }
    sockaddr_in serv{};
    serv.sin_family = AF_INET;
    serv.sin_port = htons(port);
    inet_pton(AF_INET, host, &serv.sin_addr);

    if (connect(sock, (sockaddr*)&serv, sizeof(serv)) < 0) {
        perror("connect");
        close(sock);
        return;
    }
    // Read loop
    char buf[4096];
    std::string carry;
    while (running.load()) {
        ssize_t r = read(sock, buf, sizeof(buf));
        if (r <= 0) { break; }
        carry.append(buf, buf + r);
        size_t pos;
        while ((pos = carry.find('\n')) != std::string::npos) {
            std::string line = carry.substr(0,pos);
            carry.erase(0,pos+1);
            // parse
            std::istringstream ss(line);
            Tick tk;
            char comma;
            // expect: ts_ns,price,size,side
            if (!(ss >> tk.ts_ns >> comma >> tk.price >> comma >> tk.size >> comma >> tk.side)) continue;
            // push to ring buffer (busy-wait if full — demo-only)
            while (!rb.push(tk)) {
                // backpressure: yield a bit
                std::this_thread::sleep_for(std::chrono::microseconds(50));
            }
        }
    }
    close(sock);
}

// histogram helper
struct LatStats {
    std::vector<int64_t> samples_ns;
    void add(int64_t v){ samples_ns.push_back(v); }
    void print_summary(){
        if(samples_ns.empty()){ std::cout<<"no samples\n"; return; }
        std::sort(samples_ns.begin(), samples_ns.end());
        auto p = [&](double q){
            size_t idx = std::min(samples_ns.size()-1, (size_t)std::floor(q * samples_ns.size()));
            return samples_ns[idx];
        };
        int64_t p50 = p(0.5);
        int64_t p95 = p(0.95);
        int64_t p99 = p(0.99);
        std::cout<<"count="<<samples_ns.size()
                 <<" p50="<<p50<<"ns ("<< (p50/1000.0) <<"us)"
                 <<" p95="<<p95<<"ns ("<< (p95/1000.0) <<"us)"
                 <<" p99="<<p99<<"ns ("<< (p99/1000.0) <<"us)\n";
    }
};

int main(int argc, char** argv) {
    const char* host = "127.0.0.1";
    int port = 9000;
    if (argc >= 2) host = argv[1];
    if (argc >= 3) port = atoi(argv[2]);

    SpscRing<Tick> rb(1<<16); // 65536 slots

    std::thread reader(net_reader_thread, std::ref(rb), host, port);

    // simple order id counter
    int64_t order_id = 1;
    LatStats stats;

    // toy orderbook top-of-book values
    double best_bid = 0, best_ask = 0;

    // open order log CSV
    std::ofstream order_log("orders.csv");
    if (order_log.is_open()) {
        order_log << "tick,mid,bid,ask,order_id,side,price,size\n";
    }

    size_t processed = 0;

    while (running.load()) {
        Tick tk;
        if (rb.pop(tk)) {
            // measure latency: now - producer timestamp (simulate network+processing)
            // use system_clock so it's comparable with Python time.time() epoch
            auto now_ns = duration_cast<nanoseconds>(system_clock::now().time_since_epoch()).count();
            int64_t latency = now_ns - tk.ts_ns;
            stats.add(latency);

            // update simple top-of-book
            if (tk.side == 0) { // bid tick
                best_bid = tk.price;
            } else {
                best_ask = tk.price;
            }

            // simple market maker: if we have both sides, place passive orders at mid +/- spread
            if (best_bid > 0 && best_ask > 0) {
                double mid = (best_bid + best_ask) * 0.5;
                double qsize = 0.01; // fixed small size
                double spread = (best_ask - best_bid);
                double buy_price = mid - spread*0.2;
                double sell_price = mid + spread*0.2;
                // simulate immediate order placement (we just log)
                Order o1{order_id++, buy_price, qsize, 0};
                Order o2{order_id++, sell_price, qsize, 1};
                // Log orders periodically to CSV for backtest/inspection
                if (order_log.is_open() && (processed % 100 == 0)) {
                    order_log << processed << "," << mid << "," << best_bid << "," << best_ask
                              << "," << o1.id << "," << "BUY" << "," << o1.price << "," << o1.size << "\n";
                    order_log << processed << "," << mid << "," << best_bid << "," << best_ask
                              << "," << o2.id << "," << "SELL" << "," << o2.price << "," << o2.size << "\n";
                }
                // For console: print every N processed to keep console readable
                if ((processed & 0xFFF) == 0) {
                    std::cout << "processed="<<processed<< " mid="<<mid
                              <<" bid="<<best_bid<<" ask="<<best_ask
                              <<" lat_ns="<<latency<<" ("<<latency/1000.0<<"us)\n";
                }
            }
            processed++;
            // simple stop after a lot of messages (for demo)
            if (processed >= 200000) {
                running.store(false);
                break;
            }
        } else {
            // avoid pegging a core in this simple demo
            std::this_thread::sleep_for(std::chrono::microseconds(10));
        }
    }

    running.store(false);
    reader.join();

    if (order_log.is_open()) order_log.close();

    stats.print_summary();
    return 0;
}