from model_base import RiskModelBase
import math


class BasicVolatility(RiskModelBase):
    """Estimates volatility as stddev of last N returns."""
    def __init__(self, window=20):
        self.window = window
        self.history = []

    def estimate(self, features):
        if not self.history:
            return 0.0
        m = sum(self.history) / len(self.history)
        var = sum((x - m)**2 for x in self.history) / len(self.history)
        return math.sqrt(var)

    def update(self, features, realized_return):
        self.history.append(realized_return)
        if len(self.history) > self.window:
            self.history.pop(0)


class ExponentialVolatility(RiskModelBase):
    """EWMA volatility estimator with decay λ."""
    def __init__(self, lambda_=0.94):
        self.lambda_ = lambda_
        self.sigma2 = 0.0
        self.initialized = False

    def estimate(self, features):
        return math.sqrt(self.sigma2)

    def update(self, features, realized_return):
        r2 = realized_return ** 2
        if not self.initialized:
            self.sigma2 = r2
            self.initialized = True
        else:
            self.sigma2 = self.lambda_ * self.sigma2 + (1 - self.lambda_) * r2


class VolatilityRiskModel(RiskModelBase):
    def __init__(self, lambda_=0.94):
        super().__init__()
        self.lambda_ = lambda_
        self.var = 0.0

    def estimate(self, features: dict) -> float:
        return math.sqrt(self.var + 1e-8)

    def update(self, features: dict, realized_return: float):
        self.var = self.lambda_ * self.var + (1 - self.lambda_) * (realized_return ** 2)
