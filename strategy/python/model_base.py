# model_base.py
from __future__ import annotations

from abc import ABC, abstractmethod
from enum import Enum, auto
from importlib import import_module

def build_from_spec(spec):
    """
    Build a Python object given a JSON-like spec:
    {
        "import": "models.return_models",
        "class": "LinearReturnModel",
        "init": {"w": [0.3, -0.1, 0.2], "b": 0.0}
    }
    """
    mod = import_module(spec["import"])
    cls = getattr(mod, spec["class"])
    kwargs = spec.get("init", {})
    return cls(**kwargs)

class Environment(Enum):
    """Enumerates all runtime environments for the system."""
    Research = auto()
    Backtest = auto()
    MonteCarlo = auto()
    Paper = auto()
    Live = auto()

    @staticmethod
    def from_str(value: str) -> Environment:
        """Convert string from C++ to Environment enum safely."""
        lookup = {
            "Research": Environment.Research,
            "Backtest": Environment.Backtest,
            "MonteCarlo": Environment.MonteCarlo,
            "Paper": Environment.Paper,
            "Live": Environment.Live,
        }
        return lookup.get(value, Environment.Research)

    def __str__(self) -> str:
        return self.name


class BaseWithEnv(ABC):
    """Shared base providing environment awareness for all components."""

    def __init__(self):
        self.env: Environment = Environment.Research

    def set_environment(self, env: str | Environment):
        """Called from C++ to propagate current environment."""
        if isinstance(env, str):
            self.env = Environment.from_str(env)
        elif isinstance(env, Environment):
            self.env = env
        else:
            raise TypeError(f"Invalid environment type: {type(env)}")

    def is_research(self) -> bool:
        return self.env == Environment.Research

    def is_live(self) -> bool:
        return self.env == Environment.Live



class ReturnModelBase(BaseWithEnv):
    """Predictive model producing expected return μ."""

    @abstractmethod
    def predict(self, features: dict) -> float:
        """Compute expected return μ."""
        raise NotImplementedError

    def update(self, features: dict, target: float):
        """Optional online learning update."""
        pass

class RiskModelBase(BaseWithEnv):
    """Risk model estimating volatility σ or another uncertainty metric."""

    @abstractmethod
    def estimate(self, features: dict) -> float:
        """Estimate volatility σ given current features."""
        raise NotImplementedError

    def update(self, features: dict, realized_return: float):
        """Optional adaptive update."""
        pass



class ExecutionModelBase(BaseWithEnv):
    """Transforms target weights into executable orders."""

    @abstractmethod
    def generate_orders(self, target_weight: float, context: dict):
        """Generate list of order dictionaries."""
        raise NotImplementedError


class StrategyBase(BaseWithEnv):
    """Orchestrates models and outputs target weights."""

    @abstractmethod
    def on_bar(self, features: dict) -> float:
        """Compute target portfolio weight for given features."""
        raise NotImplementedError

    def update(self, features: dict, realized_return: float):
        """Optional feedback update after realized return."""
        pass
