from abc import ABC, abstractmethod

class ReturnModelBase(ABC):
    """Base class for predictive models producing expected return μ."""

    @abstractmethod
    def predict(self, features: dict) -> float:
        """Compute expected return μ."""
        pass

    def update(self, features: dict, target: float):
        """Optional online learning update."""
        pass


class RiskModelBase(ABC):
    """Base class for risk models."""

    @abstractmethod
    def estimate(self, features: dict) -> float:
        """Compute volatility sigma."""
        pass

    def update(self, features: dict, realized_return: float):
        """Optional update."""
        pass
