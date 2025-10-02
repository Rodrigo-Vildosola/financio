
## Phase 1: Core infrastructure

1. **Thread-safe queues** (requests to worker, events back).
2. **TradingWorker thread** wrapping IB API client, with clean start/stop.
3. **Event types for trading** (Connected, Disconnected, Tick, OrderStatus, Error).
4. **MainLayer** that owns queues, worker, and `TradingState`.

---

## Phase 2: Connectivity + Market Data

5. Implement `ReqConnect`, `ReqDisconnect`, `ReqSubscribe`, `ReqUnsubscribe`.
6. Handle `tickPrice` and feed `TradingState::quotes`.
7. Add ImGui panel in `MainLayer` showing connection status + latest prices.

---

## Phase 3: Orders

8. Define `ReqPlaceOrder`, `ReqCancelOrder`.
9. Handle `orderStatus`, `execDetails` in worker → events → update `TradingState::orders`.
10. Extend UI to place test orders (e.g. AAPL limit order) and see status.

---

## Phase 4: State management

11. Add `AccountState` (cash, margin, positions).
12. Handle account update events from IB.
13. Show account balances + open positions in UI.

---

## Phase 5: Simulation

14. Create `SimulationWorker` (parallel thread like TradingWorker).
15. Define simulation requests (`RunBacktest`, `Stop`, `MonteCarlo`).
16. Emit results (equity curve, stats) via events.
17. Show results in separate ImGui panels/tabs.

---

## Phase 6: Architecture cleanup

18. Split root `MainLayer` into:

    * `TradingService` (background thread + state).
    * `SimulationService`.
    * `MainUiLayer` with tabbed dashboards.
19. Keep state persistent even if UI panels are toggled.

---

## Phase 7: Advanced

20. Strategy module API: allow plug-in strategies that consume state + push orders.
21. Logging, persistence (write ticks/orders/trades to disk).
22. Configuration system (YAML/JSON).
23. Replay mode: feed stored data into simulation events.

