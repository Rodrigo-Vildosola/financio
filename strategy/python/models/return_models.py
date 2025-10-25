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

class AdaptiveLinearModel(ReturnModelBase):
    """
    Online linear model with SGD weight updates.

    Predicts μ = w·x + b
    Updates weights incrementally after each realized return.
    """

    def __init__(self, n_features=None, lr=0.05, decay=0.999):
        self.lr = lr                  # learning rate
        self.decay = decay            # weight decay / forgetting factor
        self.w = [0.0] * (n_features or 0)
        self.b = 0.0
        self.initialized = False

    def predict(self, features):
        x = features.get("x", [])
        if not self.initialized:
            self.w = [0.0] * len(x)
            self.initialized = True
        return sum(w * v for w, v in zip(self.w, x)) + self.b

    def update(self, features, target):
        """
        Online gradient update:
            error = target - prediction
            w ← decay * w + lr * error * x
            b ← b + lr * error
        """
        x = features.get("x", [])
        if not x:
            return

        y_hat = sum(w * v for w, v in zip(self.w, x)) + self.b
        error = target - y_hat

        # weight decay for stability
        self.w = [self.decay * w + self.lr * error * v for w, v in zip(self.w, x)]
        self.b += self.lr * error
