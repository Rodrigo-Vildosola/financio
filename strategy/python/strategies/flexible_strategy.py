from model_base import StrategyBase, build_from_spec

class FlexibleStrategy(StrategyBase):
    """
    Example general-purpose strategy that dynamically builds and combines
    multiple return and risk models. It outputs a target portfolio weight μ/σ².
    """

    def __init__(self, return_models=None, risk_models=None, combine="mean"):
        super().__init__()
        self.return_models = return_models or []
        self.risk_models = risk_models or []
        self.combine = combine

    def on_bar(self, features: dict) -> float:
        """Compute target portfolio weight from model predictions."""
        mus = [m.predict(features) for m in self.return_models]
        sigmas = [r.estimate(features) for r in self.risk_models]

        mu = self._combine(mus)
        sigma = self._combine(sigmas) if sigmas else 1.0
        sigma = max(sigma, 1e-8)

        # Risk-adjusted target weight
        return float(mu / (sigma ** 2))

    def update(self, features: dict, realized_return: float):
        pass
    
    # --- internal helpers ---
    def _combine(self, vals):
        if not vals:
            return 0.0
        if self.combine == "mean":
            return sum(vals) / len(vals)
        if self.combine == "sum":
            return sum(vals)
        if self.combine == "max":
            return max(vals)
        if self.combine == "min":
            return min(vals)
        return vals[0]
