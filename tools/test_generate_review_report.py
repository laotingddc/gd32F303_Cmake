#!/usr/bin/env python3
"""generate_review_report.py 的基础测试。"""

import sys
import tempfile
import unittest
from pathlib import Path

THIS_DIR = Path(__file__).resolve().parent
if str(THIS_DIR) not in sys.path:
    sys.path.insert(0, str(THIS_DIR))

import generate_review_report as grr


class GenerateReviewReportTests(unittest.TestCase):
    def test_load_and_render_with_issues(self) -> None:
        xml_content = """<?xml version=\"1.0\" encoding=\"UTF-8\"?>
<results version=\"2\"><errors>
  <error id=\"unusedFunction\" severity=\"style\" msg=\"unused\" verbose=\"v1\"><location file=\"a.c\" line=\"11\"/></error>
  <error id=\"nullPointer\" severity=\"warning\" msg=\"null\" verbose=\"v2\"><location file=\"b.c\" line=\"5\"/></error>
</errors></results>
"""
        with tempfile.TemporaryDirectory() as td:
            xml_path = Path(td) / "cppcheck.xml"
            xml_path.write_text(xml_content, encoding="utf-8")
            issues = grr.load_issues(xml_path)
            self.assertEqual(len(issues), 2)
            self.assertEqual(issues[0]["severity"], "warning")

            html_output = grr.render_html(issues)
            self.assertIn("分级统计", html_output)
            self.assertIn("警告: 1，风格: 1", html_output)

    def test_render_with_no_issues(self) -> None:
        html_output = grr.render_html([])
        self.assertIn("未发现问题", html_output)


if __name__ == "__main__":
    unittest.main()
