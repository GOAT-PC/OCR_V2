from __future__ import annotations

import json
from dataclasses import dataclass
from pathlib import Path
from typing import Dict, List


@dataclass(frozen=True)
class OCRQuadraticModel:
    """Quadratic forward model container: c = f(OCR, T, P)."""

    coefficients: List[float]
    ranges: Dict[str, List[float]]
    coefficient_source_file: str
    range_source_file: str
    target_column: str
    feature_columns: List[str]


def _default_coef_file() -> Path:
    return Path(__file__).resolve().parent / "model_artifacts" / "ocr_quadratic_coefficients.json"


def _default_range_file() -> Path:
    return Path(__file__).resolve().parent / "model_artifacts" / "ocr_training_ranges.json"


def load_model(
    coefficient_file: str | Path | None = None,
    range_file: str | Path | None = None,
) -> OCRQuadraticModel:
    """Load pre-fitted final model artifacts (no test/legacy RSM usage)."""

    coef_path = Path(coefficient_file) if coefficient_file else _default_coef_file()
    range_path = Path(range_file) if range_file else _default_range_file()

    if not coef_path.exists():
        raise FileNotFoundError(f"Coefficient file not found: {coef_path}")
    if not range_path.exists():
        raise FileNotFoundError(f"Range file not found: {range_path}")

    coef_payload = json.loads(coef_path.read_text(encoding="utf-8"))
    range_payload = json.loads(range_path.read_text(encoding="utf-8"))

    term_order = coef_payload["term_order"]
    coef_map = coef_payload["coefficients"]

    ordered_coefficients = [float(coef_map[t]) for t in term_order]

    return OCRQuadraticModel(
        coefficients=ordered_coefficients,
        ranges=range_payload["ranges"],
        coefficient_source_file=str(coef_path),
        range_source_file=str(range_path),
        target_column="c",
        feature_columns=["OCR", "T", "P"],
    )
