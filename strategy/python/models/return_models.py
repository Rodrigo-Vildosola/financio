from model_base import ReturnModelBase
import math

class LinearReturnModel(ReturnModelBase):
    """Simple linear model using fixed weights."""
    def __init__(self, w=None, b=0.0):
        self.w = w or []
        self.b = b

    def predict(self, features):
        x = features.get("x", [])
        return sum(w * v for w, v in zip(self.w, x)) + self.b


class MomentumReturnModel(ReturnModelBase):
    """Predicts μ based on short- vs long-term momentum."""
    def __init__(self, short_key="momentum_5", long_key="momentum_20", scale=1.0):
        self.short_key = short_key
        self.long_key = long_key
        self.scale = scale

    def predict(self, features):
        s = features["named"].get(self.short_key, 0.0)
        l = features["named"].get(self.long_key, 0.0)
        return self.scale * math.tanh(s - l)
