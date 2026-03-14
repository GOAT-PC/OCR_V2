from __future__ import annotations

import json
from pathlib import Path
from typing import Dict

import numpy as np
import pandas as pd

ROOT = Path(__file__).resolve().parents[1]
DATA_FILE = ROOT / "03_data" / "processed" / "OCR_cleaned_dataset.csv"
ARTIFACT_DIR = ROOT / "01_algorithm" / "model_artifacts"
COEF_FILE = ARTIFACT_DIR / "ocr_quadratic_coefficients.json"
RANGE_FILE = ARTIFACT_DIR / "ocr_training_ranges.json"
REPORT_FILE = ARTIFACT_DIR / "ocr_model_report.json"


def build_design_matrix(ocr: np.ndarray, t: np.ndarray, p: np.ndarray) -> np.ndarray:
    # [1, OCR, T, P, OCR^2, OCR*T, OCR*P, T^2, T*P, P^2]
    return np.column_stack(
        [
            np.ones_like(ocr),
            ocr,
            t,
            p,
            ocr**2,
            ocr * t,
            ocr * p,
            t**2,
            t * p,
            p**2,
        ]
    )


def evaluate(y_true: np.ndarray, y_pred: np.ndarray) -> Dict[str, float]:
    err = y_true - y_pred
    mae = float(np.mean(np.abs(err)))
    rmse = float(np.sqrt(np.mean(err**2)))
    ss_res = float(np.sum(err**2))
    ss_tot = float(np.sum((y_true - np.mean(y_true)) ** 2))
    r2 = 1.0 - ss_res / ss_tot if ss_tot > 0 else 0.0
    return {"MAE": mae, "RMSE": rmse, "R2": float(r2)}


def fit_and_save() -> dict:
    ARTIFACT_DIR.mkdir(parents=True, exist_ok=True)

    df = pd.read_csv(DATA_FILE)
    required = ["OCR", "T", "P", "c"]
    missing = [c for c in required if c not in df.columns]
    if missing:
        raise ValueError(f"Missing required columns: {missing}")

    data = df[required].apply(pd.to_numeric, errors="coerce").dropna().copy()

    ocr = data["OCR"].to_numpy(float)
    t = data["T"].to_numpy(float)
    p = data["P"].to_numpy(float)
    c = data["c"].to_numpy(float)

    X = build_design_matrix(ocr, t, p)
    beta, *_ = np.linalg.lstsq(X, c, rcond=None)
    y_pred = X @ beta

    ranges = {
        "OCR": [float(ocr.min()), float(ocr.max())],
        "T": [float(t.min()), float(t.max())],
        "P": [float(p.min()), float(p.max())],
        "c": [float(c.min()), float(c.max())],
    }

    term_names = [
        "1",
        "OCR",
        "T",
        "P",
        "OCR^2",
        "OCR*T",
        "OCR*P",
        "T^2",
        "T*P",
        "P^2",
    ]

    coef_payload = {
        "model_type": "quadratic_forward_c=f(OCR,T,P)",
        "target": "c",
        "features": ["OCR", "T", "P"],
        "term_order": term_names,
        "coefficients": {name: float(val) for name, val in zip(term_names, beta.tolist())},
        "data_file": str(DATA_FILE),
        "sample_count": int(len(data)),
    }

    range_payload = {
        "data_file": str(DATA_FILE),
        "sample_count": int(len(data)),
        "ranges": ranges,
    }

    metrics = evaluate(c, y_pred)
    formula = (
        "c = b0 + b1*OCR + b2*T + b3*P + b4*OCR^2 + b5*OCR*T + "
        "b6*OCR*P + b7*T^2 + b8*T*P + b9*P^2"
    )

    report_payload = {
        "data_file": str(DATA_FILE),
        "sample_count": int(len(data)),
        "formula": formula,
        "coefficients": {f"b{i}": float(v) for i, v in enumerate(beta.tolist())},
        "term_order": term_names,
        "metrics": metrics,
        "coefficient_file": str(COEF_FILE),
        "range_file": str(RANGE_FILE),
        "ranges": ranges,
    }

    COEF_FILE.write_text(json.dumps(coef_payload, ensure_ascii=False, indent=2), encoding="utf-8")
    RANGE_FILE.write_text(json.dumps(range_payload, ensure_ascii=False, indent=2), encoding="utf-8")
    REPORT_FILE.write_text(json.dumps(report_payload, ensure_ascii=False, indent=2), encoding="utf-8")

    return report_payload


if __name__ == "__main__":
    report = fit_and_save()
    print(json.dumps(report, ensure_ascii=False, indent=2))
