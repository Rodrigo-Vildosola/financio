from model_base import StrategyBase, build_from_spec

class FlexibleStrategy(StrategyBase):
    """
    Example general-purpose strategy that dynamically builds and combines
    multiple return and risk models. It outputs a target portfolio weight μ/σ².
    """

    def __init__(self, combine="mean", return_models=None, risk_models=None):
        super().__init__()

        # Dynamically instantiate all submodels
        self.return_models = [build_from_spec(rm) for rm in (return_models or [])]
        self.risk_models   = [build_from_spec(rk) for rk in (risk_models or [])]

        # Combination rule: how multiple models are merged
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
        """Propagate realized return to submodels for optional online updates."""
        for m in self.return_models:
            if hasattr(m, "update"):
                m.update(features, realized_return)
        for r in self.risk_models:
            if hasattr(r, "update"):
                r.update(features, realized_return)

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
