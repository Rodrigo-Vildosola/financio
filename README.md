# Financio App

## Overview

**Financio** is an experimental, high-performance trading and investment application built using **IMGUI**, **GLFW**, **WebGPU**, and the **IBKR TWS API**.
The purpose is to automate savings, analyze portfolio performance, backtest investment strategies, and execute them in **live** or **paper trading** mode.
The system provides a flexible **strategy engine** capable of interpreting and running modular trading strategies.


## Architecture

The project uses a layered engine architecture designed for real-time financial analysis and visualization.

### Core Components

* **eng::Application**

  * Manages the main loop, layer stack, event dispatching, and rendering.
  * Initializes the renderer and UI systems.

* **RootLayer**

  * The main operational layer handling trading events, user interactions, and rendering of charts and logs.
  * Starts and manages the trading worker thread connected to Interactive Brokers (IBKR).

* **Worker System**

  * Manages asynchronous trading requests and events (connect, subscribe, historical data, etc.).
  * Decouples UI rendering from I/O and trading communication.

* **RendererAPI**

  * Abstract rendering interface using WebGPU.
  * Responsible for graphics context initialization, frame handling, and statistics.

* **UILayer**

  * Provides integration with **ImGui** and **ImPlot** for UI and data visualization.


## Current Functionality

### Implemented Features

1. **IBKR Connection**

   * Establishes a connection with the Interactive Brokers Trader Workstation (TWS) or Gateway.
   * Example connection parameters:

     ```
     Host: 127.0.0.1
     Port: 4002
     Client ID: 1
     ```

2. **Market Data Subscription**

   * Subscribes to live tick data for a given symbol.
   * Example: `AAPL` market data via the “SMART” exchange in USD.

3. **Historical Data Requests**

   * Requests historical bars (OHLCV) for a symbol and plots them using **ImPlot**.
   * Example: Two days of 5-minute bars for AAPL.

4. **UI Panels**

   * **Trading Panel:** Buttons for data requests and plotting.
   * **Trading Log:** Scrollable log of incoming events (ticks, errors, connection status).
   * **Renderer Stats:** Draw call, vertex, and mesh statistics.

5. **Event System**

   * Handles asynchronous events from trading operations.
   * Event types include connection, error, tick price, and historical bar.

6. **Layer and Event Dispatch**

   * Keyboard, mouse, and application events are routed through an event dispatcher.
   * Layers can attach, detach, and respond independently.


## Planned Features

### 1. Strategy Engine

* A modular engine capable of reading strategy definitions (in JSON, Lua, or custom DSL).
* Executes strategies in both backtesting and live environments.
* Supports signals, rules, and parameterized logic.

### 2. Portfolio Simulation and Backtesting

* Test portfolios using historical data.
* Measure performance metrics such as Sharpe ratio, drawdown, and volatility.
* Compare multiple strategies side by side.

### 3. Live and Paper Trading

* Execution engine will route orders to IBKR via the same API connection.
* Paper mode will simulate fills and track positions.

### 4. Analytics and Visualization

* Extend ImPlot usage for portfolio performance charts, allocation breakdowns, and correlation matrices.
* Add configurable dashboards for strategies.

### 5. Persistence Layer

* Local data storage for:

  * Historical data caching.
  * Strategy definitions.
  * Simulation results.

### 6. Scripting Interface

* Strategies will start as JSON rule sets, then gain Python support via pybind11.
The engine will expose core functions like get_price() and place_order() for Python scripts, allowing custom logic while keeping execution controlled and sandboxed.

## Roadmap Summary

| Milestone                | Goal                                       | Status  |
| ------------------------ | ------------------------------------------ | ------- |
| Core Engine & Rendering  | Setup WebGPU, IMGUI, GLFW framework        | Done    |
| IBKR Connectivity        | Establish event-based API interface        | Done    |
| Historical Data Plotting | Visualize with ImPlot                      | Done    |
| Strategy Engine          | Load, execute, and simulate strategies     | Planned |
| Portfolio Simulation     | Multi-asset backtesting and analytics      | Planned |
| Live Trading Mode        | Order routing via IBKR                     | Planned |
| UI Enhancements          | Dashboards, settings, multi-strategy views | Planned |

