from model_base import ExecutionModelBase
import math


class VWAPExecution(ExecutionModelBase):
    """
    Simplified execution model that generates a single order aiming to
    achieve the target portfolio weight over a VWAP-style schedule.
    """

    def __init__(self, window=10):
        super().__init__()
        self.window = int(window)
        self.history = []

    def generate_orders(self, target_weight: float, context: dict):
        """
        Produce a list of simulated orders.
        Each order is a dict: {"symbol": str, "weight": float, "action": str}.
        """
        symbol = context.get("symbol", "UNKNOWN")
        timestamp = context.get("timestamp", 0)
        notional = context.get("portfolio_notional", 1e6)

        self.history.append(target_weight)
        if len(self.history) > self.window:
            self.history.pop(0)

        avg_weight = sum(self.history) / len(self.history)
        diff = target_weight - avg_weight

        action = "buy" if diff > 0 else "sell" if diff < 0 else "hold"

        order = {
            "symbol": symbol,
            "timestamp": timestamp,
            "target_weight": round(target_weight, 6),
            "order_size": abs(diff) * notional,
            "action": action,
            "env": str(self.env)
        }
        return [order]
