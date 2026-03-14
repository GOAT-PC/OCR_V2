from __future__ import annotations

import argparse
import json
import math
from typing import Any, Dict, List

from model_loader import load_model


def _in_range(value: float, bounds: List[float]) -> bool:
    return bounds[0] <= value <= bounds[1]


def _build_range_message(c: float, t: float, p: float, ranges: Dict[str, List[float]]) -> str:
    problems: List[str] = []
    if not _in_range(c, ranges["c"]):
        problems.append(f"c={c} 超出范围 [{ranges['c'][0]}, {ranges['c'][1]}]")
    if not _in_range(t, ranges["T"]):
        problems.append(f"T={t} 超出范围 [{ranges['T'][0]}, {ranges['T'][1]}]")
    if not _in_range(p, ranges["P"]):
        problems.append(f"P={p} 超出范围 [{ranges['P'][0]}, {ranges['P'][1]}]")
    if not problems:
        return ""
    return "输入超出训练范围：" + "；".join(problems) + "。"


def predict_ocr(c: float, T: float, P: float) -> Dict[str, Any]:
    """Inverse solve OCR from forward model c = f(OCR, T, P)."""

    model = load_model()
    b0, b1, b2, b3, b4, b5, b6, b7, b8, b9 = model.coefficients
    ranges = model.ranges

    range_message = _build_range_message(c=c, t=T, p=P, ranges=ranges)
    input_in_range = range_message == ""

    # c = b0 + b1*OCR + b2*T + b3*P + b4*OCR^2 + b5*OCR*T + b6*OCR*P + b7*T^2 + b8*T*P + b9*P^2
    # => a*OCR^2 + b*OCR + d = 0
    a = b4
    b = b1 + b5 * T + b6 * P
    d = b0 + b2 * T + b3 * P + b7 * (T**2) + b8 * T * P + b9 * (P**2) - c

    eps = 1e-12
    if abs(a) < eps:
        if abs(b) < eps:
            msg = "方程退化，无法反解 OCR。"
            if range_message:
                msg = f"{msg} {range_message}"
            return {
                "ocr_pred": None,
                "in_range": False,
                "message": msg,
                "range": ranges,
            }
        roots = [(-d) / b]
    else:
        disc = b * b - 4 * a * d
        if disc < -eps:
            msg = "二次方程无实根，无法反解 OCR。"
            if range_message:
                msg = f"{msg} {range_message}"
            return {
                "ocr_pred": None,
                "in_range": False,
                "message": msg,
                "range": ranges,
            }
        sqrt_disc = math.sqrt(max(disc, 0.0))
        roots = [(-b + sqrt_disc) / (2 * a), (-b - sqrt_disc) / (2 * a)]

    ocr_bounds = ranges["OCR"]
    in_range_roots = [r for r in roots if _in_range(r, ocr_bounds)]

    if in_range_roots:
        center = (ocr_bounds[0] + ocr_bounds[1]) / 2
        ocr_pred = min(in_range_roots, key=lambda x: abs(x - center))
        msg = "反解成功，已优先选择训练范围内实根。"
        if range_message:
            msg = f"{msg} {range_message}"
        return {
            "ocr_pred": float(ocr_pred),
            "in_range": bool(input_in_range and _in_range(ocr_pred, ocr_bounds)),
            "message": msg,
            "range": ranges,
        }

    ocr_pred = min(roots, key=lambda x: min(abs(x - ocr_bounds[0]), abs(x - ocr_bounds[1])))
    msg = "存在实根但不在 OCR 训练范围内，已返回最接近范围边界的实根。"
    if range_message:
        msg = f"{msg} {range_message}"

    return {
        "ocr_pred": float(ocr_pred),
        "in_range": False,
        "message": msg,
        "range": ranges,
    }


def _main() -> None:
    parser = argparse.ArgumentParser(description="Predict OCR from c/T/P")
    parser.add_argument("--c", type=float, required=True)
    parser.add_argument("--T", type=float, required=True)
    parser.add_argument("--P", type=float, required=True)
    args = parser.parse_args()

    print(json.dumps(predict_ocr(c=args.c, T=args.T, P=args.P), ensure_ascii=False, indent=2))


if __name__ == "__main__":
    _main()
