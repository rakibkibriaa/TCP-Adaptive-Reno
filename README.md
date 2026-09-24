# TCP Adaptive Reno in NS-3

An implementation of **TCP Adaptive Reno (TCP-AReno)**, a congestion control algorithm that tries to get the best of both worlds: the high throughput of aggressive protocols like HighSpeed TCP and Westwood+, and the friendliness of classic TCP NewReno. The algorithm is implemented as a new congestion control model inside the NS-3 network simulator and evaluated on a dumbbell topology against TCP NewReno, TCP HighSpeed and TCP Westwood+.

Built as part of the **CSE322: Computer Networking Sessional** coursework.

> Reference paper: H. Shimonishi, T. Hama, T. Murase, *"TCP-Adaptive Reno for Improving Efficiency-Friendliness Tradeoffs of TCP Congestion Control Algorithm"*, IEEE GLOBECOM 2005.

---

## Why Adaptive Reno?

TCP NewReno treats every packet loss as a congestion signal and halves its window. On fast, long-distance links with random (non-congestion) losses, this leaves most of the bandwidth unused. Protocols such as HighSpeed TCP and Westwood+ fix the throughput problem, but they are aggressive enough to starve coexisting NewReno flows, which has held back their deployment.

TCP-AReno sits in between. It uses **RTT measurements to estimate how congested the network actually is**, then:

- **Grows the window quickly** when the network looks underutilized, using a fast "probe" component on top of a Reno-like base.
- **Reduces the window gently** after a loss that does not look congestion-related.
- **Behaves just like NewReno** (halving the window, linear growth) when the network is genuinely congested.

---

## How It Works

`TcpAdaptiveReno` inherits from `TcpWestwoodPlus` (reusing its bandwidth estimator) and overrides three hooks while adding two helpers.

### 1. `PktsAcked`
Called on every ACK. Tracks the number of acked segments, records the current and minimum RTT, and updates the Westwood+ bandwidth estimate via `EstimateBW()`.

### 2. `EstimateCongestionLevel`
Estimates the RTT expected at the moment of a congestion loss, smoothed over past loss events ($a = 0.85$):

$$RTT_{cong}^{j} = a \cdot RTT_{cong}^{j-1} + (1-a) \cdot RTT^{j}$$

and turns it into a congestion level $c \in [0, 1]$:

$$c = \min\left(\frac{RTT - RTT_{min}}{RTT_{cong} - RTT_{min}},\ 1\right)$$

### 3. `EstimateIncWnd`
Computes how fast the probe part of the window should grow, based on estimated bandwidth $B$ ($M = 1000$, $\alpha = 10$):

$$W_{inc}^{max} = \frac{B}{M} \cdot MSS$$

$$\beta = 2W_{inc}^{max}\left(\frac{1}{\alpha} - \frac{\frac{1}{\alpha} + 1}{e^{\alpha}}\right), \qquad \gamma = 1 - 2W_{inc}^{max}\left(\frac{1}{\alpha} - \frac{\frac{1}{\alpha} + \frac{1}{2}}{e^{\alpha}}\right)$$

$$W_{inc}(c) = \frac{W_{inc}^{max}}{e^{c\alpha}} + c\beta + \gamma$$

When congestion is high ($c \to 1$), $W_{inc}$ approaches zero and the flow falls back to plain Reno growth.

### 4. `CongestionAvoidance`
The congestion window is split into a Reno-like **base** part and a fast **probe** part:

$$W_{base} = W_{base} + \frac{1 \cdot MSS}{W}, \qquad W_{probe} = \max\left(W_{probe} + \frac{W_{inc}}{W},\ 0\right), \qquad W = W_{base} + W_{probe}$$

### 5. `GetSsThresh`
Called on a loss event. Updates the RTT history, then reduces the window according to the congestion level instead of always halving it:

$$W_{base} = \frac{W}{1 + c}, \qquad W_{probe} = 0$$

With $c = 1$ this is exactly Reno's halving; with $c \approx 0$ the window barely shrinks. The new `ssthresh` is floored at `2 * segmentSize`.

---

## Simulation Setup

A dumbbell topology with two flows sharing one bottleneck link. Flow 1 always runs **TCP NewReno**; flow 2 runs the algorithm under test.

```
  Sender 0 (NewReno) ──┐                            ┌── Receiver 0
                       Router ══ bottleneck ══ Router
  Sender 1 (X)       ──┘                            └── Receiver 1
```

| Parameter | Value |
|---|---|
| Access links | 1 Gbps, 1 ms |
| Bottleneck delay | 100 ms |
| Bottleneck rate | 1 – 301 Mbps (step 50), default 50 Mbps |
| Packet loss rate | 10⁻⁶ – 10⁻² (`RateErrorModel` on bottleneck devices) |
| Router queue | DropTail, sized to the bandwidth-delay product |
| Packet size | 1024 bytes |
| Simulation time | 60 s |

Throughput is measured using `FlowMonitor`, and congestion window traces come from the `CongestionWindow` trace source. As a bonus, **Jain's Fairness Index** is computed for each pair of coexisting flows.

---

## Repository Structure

```
TCP-Adaptive-Reno/
├── Code/
│   ├── 1905098_tcp-adaptive-reno.h    # TcpAdaptiveReno class declaration
│   ├── 1905098_tcp-adaptive-reno.cc   # Congestion control implementation
│   ├── 1905098.cc                     # Dumbbell topology simulation
│   ├── 1905098.sh                     # Runs all experiments and generates plots
│   ├── 1905098_1.gnuplot              # Throughput plots
│   ├── 1905098_2.gnuplot              # Congestion window vs time plots
│   └── 1905098_3.gnuplot              # Fairness index plots
└── Spec/
    ├── Paper.pdf                      # Original TCP-AReno paper
    └── Specifications.pdf             # Assignment specification
```

---

## Getting Started

### Prerequisites
- NS-3 (3.3x, with the CMake-based `./ns3` build system)
- `gnuplot`
- A C++ toolchain supported by your NS-3 version

### 1. Add the model to NS-3

Copy the model files into the internet module:

```bash
cp Code/1905098_tcp-adaptive-reno.h  <ns3-root>/src/internet/model/
cp Code/1905098_tcp-adaptive-reno.cc <ns3-root>/src/internet/model/
```

Register them in `src/internet/CMakeLists.txt`:

```cmake
set(source_files
    ...
    model/1905098_tcp-adaptive-reno.cc
)

set(header_files
    ...
    model/1905098_tcp-adaptive-reno.h
)
```

### 2. Patch two NS-3 files

The model and simulation need access to a few members that NS-3 keeps private:

- **`src/internet/model/tcp-westwood-plus.h`**: move `EstimateBW()`, `m_currentBW` and `m_fType` from `private` to `protected` so `TcpAdaptiveReno` can use them.
- **`src/point-to-point-layout/model/point-to-point-dumbbell.h`**: make `m_routerDevices` `public` so the error model can be attached to the bottleneck devices.

### 3. Build

```bash
cd <ns3-root>
./ns3 build
```

### 4. Run the experiments

Copy the simulation, script and gnuplot files into `scratch/`, then run the script from there:

```bash
cp Code/1905098.cc Code/1905098.sh Code/*.gnuplot <ns3-root>/scratch/
cd <ns3-root>/scratch
bash 1905098.sh
```

To run a single simulation manually:

```bash
./ns3 run "scratch/1905098.cc --algo=ns3::TcpAdaptiveReno --bottleneckRate=50 --error=-6"
```

| Argument | Description |
|---|---|
| `--algo` | Algorithm for flow 2: `ns3::TcpAdaptiveReno`, `ns3::TcpWestwoodPlus`, `ns3::TcpHighSpeed` |
| `--bottleneckRate` | Bottleneck data rate in Mbps |
| `--error` | Exponent of the packet loss rate (e.g. `-6` → 10⁻⁶) |

---

## Generated Plots

The script produces one folder per comparison (`VS_TCP_ADAPTIVE_RENO/`, `VS_TCP_HIGHSPEED/`, `VS_TCP_WEST_WOOD_PLUS/`), each containing:

| Plot | What it shows |
|---|---|
| `throughput_vs_bottleneck_data_rate.png` | How each flow scales with link capacity |
| `throughput_vs_packet_loss_rate.png` | How each flow copes with random losses |
| `congestion_window_vs_time.png` | Window evolution of both flows at 51 Mbps |
| `fairness_vs_bottleneck_data_rate.png` | Jain's Fairness Index across link capacities |
| `fairness_vs_packet_loss_rate.png` | Jain's Fairness Index across loss rates |

---

## References

1. H. Shimonishi, T. Hama, T. Murase, *"TCP-Adaptive Reno for Improving Efficiency-Friendliness Tradeoffs of TCP Congestion Control Algorithm,"* IEEE GLOBECOM 2005.
2. [NS-3 Network Simulator](https://www.nsnam.org/)
3. NS-3 source models: `TcpCongestionOps`, `TcpNewReno`, `TcpWestwoodPlus`, `TcpSocketState`
