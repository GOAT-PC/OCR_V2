from __future__ import annotations

import sys
from pathlib import Path

import pandas as pd

ROOT = Path(__file__).resolve().parents[2]
ALGO_DIR = ROOT / "01_algorithm"
if str(ALGO_DIR) not in sys.path:
    sys.path.insert(0, str(ALGO_DIR))

from model_loader import load_model
from predict_ocr import predict_ocr


def _forward_c(coeffs: list[float], ocr: float, t: float, p: float) -> float:
    b0, b1, b2, b3, b4, b5, b6, b7, b8, b9 = coeffs
    return (
        b0
        + b1 * ocr
        + b2 * t
        + b3 * p
        + b4 * (ocr**2)
        + b5 * ocr * t
        + b6 * ocr * p
        + b7 * (t**2)
        + b8 * t * p
        + b9 * (p**2)
    )


def test_predict_ocr_inverse_roundtrip() -> None:
    df = pd.read_csv(ROOT / "03_data" / "processed" / "OCR_cleaned_dataset.csv")
    row = df.iloc[100]
    ocr_true = float(row["OCR"])
    T = float(row["T"])
    P = float(row["P"])

    model = load_model()
    c_forward = _forward_c(model.coefficients, ocr_true, T, P)

    result = predict_ocr(c=c_forward, T=T, P=P)

    assert result["ocr_pred"] is not None
    low, high = model.ranges["OCR"]
    assert low <= result["ocr_pred"] <= high
    assert abs(result["ocr_pred"] - ocr_true) < 1e-6
