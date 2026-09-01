#!/usr/bin/env python3
"""Report static ServicePool / service object sizes from a GCC class dump.

The embedded compiler writes this dump when compiled with ``-fdump-lang-class``.
The same script can be pointed at any GCC class dump (e.g. datalink).
"""

from __future__ import annotations

import argparse
import re
import sys
from pathlib import Path

CLASS_HEADER = re.compile(r"^Class (.+)$")
CLASS_SIZE = re.compile(r"^\s+size=(\d+) align=(\d+)\s*$")
DEFAULT_PRIMARY = re.compile(r"(?:^ServicePool$|.+Service$)")


def parse_class_dump(text: str) -> dict[str, tuple[int, int]]:
    """Return {class_name: (size, align)} for each Class entry. Last write wins."""
    sizes: dict[str, tuple[int, int]] = {}
    name: str | None = None
    for line in text.splitlines():
        header = CLASS_HEADER.match(line)
        if header:
            name = header.group(1)
            continue
        if name is None:
            continue
        size_match = CLASS_SIZE.match(line)
        if size_match:
            sizes[name] = (int(size_match.group(1)), int(size_match.group(2)))
            name = None
    return sizes


def split_primary(sizes: dict[str, tuple[int, int]], primary: re.Pattern[str]) -> tuple[dict[str, tuple[int, int]], dict[str, tuple[int, int]]]:
    matched = {name: value for name, value in sizes.items() if primary.search(name)}
    others = {name: value for name, value in sizes.items() if name not in matched}
    return matched, others


def format_report(
    sizes: dict[str, tuple[int, int]],
    label: str = "",
    primary: re.Pattern[str] | None = None,
    top_related: int = 20,
) -> str:
    if primary is None:
        primary = DEFAULT_PRIMARY
    matched, others = split_primary(sizes, primary)
    lines: list[str] = []
    header = "Static service object sizes"
    if label:
        header = f"{header} ({label})"
    lines.append(header)
    lines.append("")

    if not matched and not others:
        lines.append("No class sizes found in dump.")
        return "\n".join(lines) + "\n"

    pool = matched.get("ServicePool")
    if pool:
        lines.append(f"ServicePool  {pool[0]:>8} bytes  (align {pool[1]})")
        lines.append("")

    services = [(name, value) for name, value in matched.items() if name != "ServicePool"]
    services.sort(key=lambda item: (-item[1][0], item[0]))
    if services:
        lines.append("Per-service")
        service_total = 0
        for name, (size, align) in services:
            lines.append(f"  {name:<40} {size:>8}  (align {align})")
            service_total += size
        lines.append(f"  {'(sum of services)':<40} {service_total:>8}")
        if pool:
            lines.append(f"  {'(ServicePool overhead)':<40} {pool[0] - service_total:>8}")
        lines.append("")

    related = sorted(others.items(), key=lambda item: (-item[1][0], item[0]))[:top_related]
    if related:
        lines.append("Largest related types (member blame)")
        for name, (size, align) in related:
            display = name if len(name) <= 72 else name[:69] + "..."
            lines.append(f"  {display:<72} {size:>8}  (align {align})")

    return "\n".join(lines) + "\n"


def self_test() -> int:
    dump = """Class Service
   size=2 align=2
   base size=2 base align=2
Service (0x0x0) 0

Class Vec<int, 8>
   size=36 align=4
   base size=36 base align=4
Vec<int, 8> (0x0x1) 0

Class HousekeepingService
   size=52 align=4
   base size=52 base align=4
HousekeepingService (0x0x2) 0

Class TestService
   size=4 align=2
   base size=4 base align=2
TestService (0x0x3) 0

Class ServicePool
   size=68 align=4
   base size=68 base align=4
ServicePool (0x0x4) 0
"""
    sizes = parse_class_dump(dump)
    assert sizes["ServicePool"] == (68, 4)
    assert sizes["HousekeepingService"] == (52, 4)
    assert sizes["TestService"] == (4, 2)
    assert sizes["Vec<int, 8>"] == (36, 4)
    primary, related = split_primary(sizes, DEFAULT_PRIMARY)
    assert "Service" not in primary
    assert "Service" in related
    report = format_report(sizes, label="self-test")
    assert "ServicePool" in report and "68" in report
    assert "HousekeepingService" in report
    assert "Vec<int, 8>" in report
    assert "(ServicePool overhead)" in report
    per_service = report.split("Largest related types")[0]
    assert re.search(r"^\s+Service\s+", per_service, re.M) is None
    print(report)
    print("self-test ok")
    return 0


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--dump", type=Path, help="GCC -fdump-lang-class output file")
    parser.add_argument("--output", type=Path, help="Write the report here (also printed)")
    parser.add_argument("--label", default="", help="Subsystem / project label")
    parser.add_argument("--primary", default=DEFAULT_PRIMARY.pattern, help="Regex of primary types to list first")
    parser.add_argument("--top", type=int, default=20, help="How many non-primary types to list")
    parser.add_argument("--self-test", action="store_true")
    args = parser.parse_args()

    if args.self_test:
        return self_test()
    if args.dump is None:
        parser.error("--dump is required unless --self-test is set")

    sizes = parse_class_dump(args.dump.read_text())
    report = format_report(sizes, label=args.label, primary=re.compile(args.primary), top_related=args.top)
    sys.stdout.write(report)
    if args.output is not None:
        args.output.parent.mkdir(parents=True, exist_ok=True)
        args.output.write_text(report)
    return 0


if __name__ == "__main__":
    sys.exit(main())
