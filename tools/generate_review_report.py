#!/usr/bin/env python3
"""将 cppcheck XML 报告转换为中文 HTML 报告。"""

from __future__ import annotations

import argparse
import datetime as dt
import html
import pathlib
import sys
import xml.etree.ElementTree as ET

SEVERITY_CN = {
    "error": "错误",
    "warning": "警告",
    "style": "风格",
    "performance": "性能",
    "portability": "可移植性",
    "information": "信息",
    "debug": "调试",
}

SEVERITY_ORDER = {
    "error": 0,
    "warning": 1,
    "performance": 2,
    "portability": 3,
    "style": 4,
    "information": 5,
    "debug": 6,
}


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description="生成中文代码审查 HTML 报告")
    parser.add_argument("--input", required=True, help="cppcheck XML 报告路径")
    parser.add_argument("--output", required=True, help="输出 HTML 路径")
    return parser.parse_args()


def load_issues(xml_path: pathlib.Path) -> list[dict[str, str]]:
    try:
        tree = ET.parse(xml_path)
    except ET.ParseError as exc:
        raise ValueError(f"XML 解析失败: {xml_path} ({exc})") from exc

    root = tree.getroot()
    issues: list[dict[str, str]] = []

    for error in root.findall(".//error"):
        location = error.find("location")
        file_name = location.get("file", "未知文件") if location is not None else "未知文件"
        line = location.get("line", "-") if location is not None else "-"
        severity = error.get("severity", "unknown")

        issues.append(
            {
                "severity": severity,
                "severity_cn": SEVERITY_CN.get(severity, severity),
                "id": error.get("id", "unknown"),
                "message": error.get("msg", ""),
                "verbose": error.get("verbose", ""),
                "file": file_name,
                "line": line,
            }
        )

    issues.sort(key=lambda item: (SEVERITY_ORDER.get(item["severity"], 99), item["file"], item["line"]))
    return issues


def build_severity_summary(issues: list[dict[str, str]]) -> str:
    counts: dict[str, int] = {}
    for issue in issues:
        key = issue["severity"]
        counts[key] = counts.get(key, 0) + 1

    if not counts:
        return "无"

    parts = []
    for key in sorted(counts.keys(), key=lambda sev: SEVERITY_ORDER.get(sev, 99)):
        parts.append(f"{SEVERITY_CN.get(key, key)}: {counts[key]}")
    return "，".join(parts)


def render_html(issues: list[dict[str, str]]) -> str:
    generated_at = dt.datetime.now(dt.timezone.utc).astimezone().strftime("%Y-%m-%d %H:%M:%S %Z")
    severity_summary = build_severity_summary(issues)

    rows = []
    for issue in issues:
        rows.append(
            "<tr>"
            f"<td>{html.escape(issue['severity_cn'])}</td>"
            f"<td>{html.escape(issue['id'])}</td>"
            f"<td>{html.escape(issue['file'])}</td>"
            f"<td>{html.escape(issue['line'])}</td>"
            f"<td>{html.escape(issue['message'])}</td>"
            f"<td>{html.escape(issue['verbose'])}</td>"
            "</tr>"
        )

    issue_rows = "\n".join(rows) if rows else "<tr><td colspan='6'>未发现问题 🎉</td></tr>"

    return f"""<!doctype html>
<html lang=\"zh-CN\">
<head>
  <meta charset=\"utf-8\" />
  <meta name=\"viewport\" content=\"width=device-width, initial-scale=1\" />
  <title>代码审查报告</title>
  <style>
    body {{ font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', sans-serif; margin: 24px; }}
    h1 {{ margin-bottom: 6px; }}
    .meta {{ color: #666; margin-bottom: 20px; }}
    table {{ border-collapse: collapse; width: 100%; }}
    th, td {{ border: 1px solid #ddd; padding: 8px; font-size: 14px; vertical-align: top; }}
    th {{ background: #f5f5f5; text-align: left; }}
    tr:nth-child(even) {{ background: #fafafa; }}
    .summary {{ margin-bottom: 10px; }}
  </style>
</head>
<body>
  <h1>代码审查报告</h1>
  <div class=\"meta\">生成时间：{html.escape(generated_at)}</div>
  <div class=\"summary\">问题总数：<strong>{len(issues)}</strong></div>
  <div class=\"summary\">分级统计：<strong>{html.escape(severity_summary)}</strong></div>
  <table>
    <thead>
      <tr>
        <th>严重级别</th>
        <th>规则 ID</th>
        <th>文件</th>
        <th>行号</th>
        <th>问题描述</th>
        <th>详细信息</th>
      </tr>
    </thead>
    <tbody>
      {issue_rows}
    </tbody>
  </table>
</body>
</html>
"""


def main() -> int:
    args = parse_args()
    input_path = pathlib.Path(args.input)
    output_path = pathlib.Path(args.output)

    if not input_path.exists():
        print(f"输入文件不存在: {input_path}", file=sys.stderr)
        return 1

    try:
        issues = load_issues(input_path)
    except ValueError as exc:
        print(str(exc), file=sys.stderr)
        return 1

    output_path.parent.mkdir(parents=True, exist_ok=True)
    output_path.write_text(render_html(issues), encoding="utf-8")
    print(f"已生成报告: {output_path} (问题数: {len(issues)})")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
