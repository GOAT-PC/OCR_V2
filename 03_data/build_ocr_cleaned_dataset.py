from __future__ import annotations

from dataclasses import dataclass
from pathlib import Path
from typing import Callable

import pandas as pd

ROOT = Path(__file__).resolve().parent
TABLE_DIR = ROOT / "table"
OUTPUT_CSV = ROOT / "processed" / "OCR_cleaned_dataset.csv"


@dataclass
class ParsedFile:
    file_name: str
    raw_rows: int
    parsed_rows: int
    cleaned_rows: int
    frame: pd.DataFrame


def _to_numeric_clean(df: pd.DataFrame) -> pd.DataFrame:
    out = df.copy()
    for col in ["OCR", "T", "P", "c"]:
        out[col] = pd.to_numeric(out[col], errors="coerce")
    out = out.dropna(subset=["OCR", "T", "P", "c"])
    return out


def parse_ocr_data_xlsx(path: Path) -> ParsedFile:
    # 稳定规则：该文件前18行为仪器信息，真实表头在 Excel 第19行（header=18）
    df = pd.read_excel(path, sheet_name="OCR数据", header=18)
    raw_rows = len(df)

    col_ocr = next(c for c in df.columns if isinstance(c, str) and "浓度" in c and "未校准" not in c and "传感器 1" in c)
    col_t = next(c for c in df.columns if isinstance(c, str) and "温度 - 传感器 1" in c)
    col_c = next(c for c in df.columns if isinstance(c, str) and "声速 - 传感器 1" in c)
    col_p = next(c for c in df.columns if isinstance(c, str) and "PRE - 传感器 1" in c)

    out = df[[col_ocr, col_t, col_p, col_c]].copy()
    out.columns = ["OCR", "T", "P", "c"]

    out = _to_numeric_clean(out)
    # OCR数据.xlsx 的 PRE 数值为 bar，统一转成 MPa
    out["P"] = out["P"] / 10.0
    return ParsedFile(path.name, raw_rows, len(out), len(out), out)


def parse_position_based_ocr_t_c_p(path: Path) -> ParsedFile:
    # 该类文件首行为数据而非表头，固定按前4列解析：OCR,T,c,P
    df = pd.read_excel(path, sheet_name=0, header=None)
    raw_rows = len(df)
    out = df.iloc[:, :4].copy()
    out.columns = ["OCR", "T", "c", "P"]
    out = out[["OCR", "T", "P", "c"]]
    out = _to_numeric_clean(out)
    return ParsedFile(path.name, raw_rows, len(out), len(out), out)


def parse_named_conc_temp_vel_press(path: Path) -> ParsedFile:
    df = pd.read_excel(path, sheet_name=0)
    raw_rows = len(df)
    out = df[["浓度", "温度", "压力", "声速"]].copy()
    out.columns = ["OCR", "T", "P", "c"]
    out = _to_numeric_clean(out)
    return ParsedFile(path.name, raw_rows, len(out), len(out), out)


def parse_static_single_tube(path: Path) -> ParsedFile:
    df = pd.read_excel(path, sheet_name=0)
    raw_rows = len(df)
    out = df[["浓度", "温度", "压力", "声速"]].copy()
    out.columns = ["OCR", "T", "P", "c"]
    out = _to_numeric_clean(out)
    return ParsedFile(path.name, raw_rows, len(out), len(out), out)


def build_dataset() -> tuple[pd.DataFrame, list[ParsedFile], pd.DataFrame]:
    parsers: list[tuple[str, Callable[[Path], ParsedFile]]] = [
        ("OCR数据.xlsx", parse_ocr_data_xlsx),
        ("OCR数据1.xlsx", parse_position_based_ocr_t_c_p),
        ("OCR数据表.xlsx", parse_position_based_ocr_t_c_p),
        ("OCR线性关系猜想验证.xlsx", parse_named_conc_temp_vel_press),
        ("单管静置OCR实验数据.xlsx", parse_static_single_tube),
    ]

    parsed: list[ParsedFile] = []
    frames: list[pd.DataFrame] = []

    for file_name, parser in parsers:
        pf = parser(TABLE_DIR / file_name)
        pf.frame = pf.frame.copy()
        pf.frame["__source_file"] = file_name
        parsed.append(pf)
        frames.append(pf.frame)

    combined = pd.concat(frames, ignore_index=True)

    pre_clean_count = len(combined)

    combined = combined[(combined["OCR"] != 0) & (combined["OCR"] != 12)]
    combined = combined.dropna(subset=["OCR", "T", "P", "c"])

    dedup_cols = ["OCR", "T", "P", "c"]
    kept = combined.drop_duplicates(subset=dedup_cols, keep="first").copy()

    contribution = (
        kept.groupby("__source_file", as_index=False)
        .size()
        .rename(columns={"size": "contribution_rows"})
    )
    all_sources = pd.DataFrame({"__source_file": [name for name, _ in parsers]})
    contribution = all_sources.merge(contribution, on="__source_file", how="left").fillna(0)
    contribution["contribution_rows"] = contribution["contribution_rows"].astype(int)
    contribution = contribution.sort_values("__source_file")

    final_df = kept[dedup_cols].sort_values(["OCR", "T", "P", "c"]).reset_index(drop=True)
    final_df.to_csv(OUTPUT_CSV, index=False)

    summary = pd.DataFrame(
        {
            "pre_clean_total": [pre_clean_count],
            "post_clean_total": [len(final_df)],
        }
    )
    return final_df, parsed, contribution.merge(summary, how="cross")


if __name__ == "__main__":
    final_df, parsed, stats = build_dataset()

    print("Source files parsed:")
    for p in parsed:
        print(f"- {p.file_name}: raw_rows={p.raw_rows}, parsed_rows={p.parsed_rows}")

    print("\nContribution rows (after global dedup):")
    print(stats[["__source_file", "contribution_rows"]].drop_duplicates().to_string(index=False))

    print("\nTotals:")
    print(stats[["pre_clean_total", "post_clean_total"]].drop_duplicates().to_string(index=False))

    print("\nHead(10):")
    print(final_df.head(10).to_string(index=False))

    print("\nMin/Max:")
    print(final_df.agg(["min", "max"]).to_string())
