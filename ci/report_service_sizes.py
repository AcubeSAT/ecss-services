#!/usr/bin/env python3
"""Print ServicePool / service sizes and each class's direct members from DWARF."""

import re
import shutil
import subprocess
import sys
from pathlib import Path

DIE_START = re.compile(r"^ <(\d+)><([0-9a-f]+)>: Abbrev Number: \d+\s+\((DW_TAG_\w+)\)")
DIE_END = re.compile(r"^ <(\d+)><[0-9a-f]+>: Abbrev Number: 0")
ATTR = re.compile(r"^\s+<[0-9a-f]+>\s+DW_AT_(\w+)\s*:\s*(.*)$")
TYPE_REF = re.compile(r"<0x([0-9a-f]+)>")
NAME = re.compile(r"\): (.+)$")
PRIMARY = re.compile(r"(?:^ServicePool$|.+Service$)")


def _short(name, limit=56):
    return name if len(name) <= limit else name[: limit - 3] + "..."


def _name(raw):
    match = NAME.search(raw)
    return match.group(1) if match else raw.strip()


def parse_readelf(text):
    dies = {}
    current = None
    for line in text.splitlines():
        start = DIE_START.match(line)
        if start:
            current = {
                "level": int(start.group(1)),
                "offset": int(start.group(2), 16),
                "tag": start.group(3),
                "attrs": {},
            }
            dies[current["offset"]] = current
            continue
        if DIE_END.match(line):
            current = None
            continue
        attr = ATTR.match(line)
        if current is not None and attr:
            current["attrs"][attr.group(1)] = attr.group(2).strip()
    return dies


def _follow(dies, offset, seen=None):
    seen = seen or set()
    if offset in seen or offset not in dies:
        return None
    seen.add(offset)
    die = dies[offset]
    if "byte_size" in die["attrs"]:
        return die
    ref = TYPE_REF.search(die["attrs"].get("type", ""))
    if ref:
        return _follow(dies, int(ref.group(1), 16), seen)
    return die


def type_info(dies, raw_type):
    ref = TYPE_REF.search(raw_type or "")
    if not ref:
        return 0, ""
    die = _follow(dies, int(ref.group(1), 16))
    if die is None:
        return 0, ""
    size = die["attrs"].get("byte_size", "0")
    try:
        size = int(size, 0)
    except ValueError:
        size = 0
    return size, _name(die["attrs"].get("name", ""))


def class_members(dies, children):
    members = []
    for child in children:
        attrs = child["attrs"]
        if child["tag"] == "DW_TAG_inheritance":
            size, name = type_info(dies, attrs.get("type", ""))
            members.append(("(base) " + (name or "?"), size, name))
        elif child["tag"] == "DW_TAG_member" and "data_member_location" in attrs:
            size, type_name = type_info(dies, attrs.get("type", ""))
            members.append((_name(attrs.get("name", "?")), size, type_name))
    members.sort(key=lambda item: (-item[1], item[0]))
    return members


def index_classes(dies):
    ordered = sorted(dies.values(), key=lambda item: item["offset"])
    classes = {}
    for index, class_die in enumerate(ordered):
        if class_die["level"] != 1 or class_die["tag"] != "DW_TAG_class_type":
            continue
        name = _name(class_die["attrs"].get("name", ""))
        if not name:
            continue
        children = []
        for die in ordered[index + 1:]:
            if die["level"] <= 1:
                break
            if die["level"] == 2:
                children.append(die)
        try:
            size = int(class_die["attrs"].get("byte_size", "0"), 0)
        except ValueError:
            size = 0
        classes[name] = (size, class_members(dies, children))
    return classes


def format_report(classes, label=""):
    primary = {name: value for name, value in classes.items() if PRIMARY.search(name)}
    lines = ["Static service object sizes" + (f" ({label})" if label else ""), ""]
    pool_size, pool_members = primary.get("ServicePool", (0, []))
    if pool_size:
        lines.append(f"ServicePool  {pool_size:>8} bytes")
        lines.append("")

    services = [(name, value) for name, value in primary.items() if name != "ServicePool"]
    services.sort(key=lambda item: (-item[1][0], item[0]))
    if services:
        lines.append("Per-service")
        total = 0
        for name, (size, members) in services:
            lines.append(f"  {name:<40} {size:>8}")
            total += size
            for member, member_size, type_name in members:
                lines.append(f"    {member:<38} {member_size:>8}  {_short(type_name)}")
        lines.append(f"  {'(sum of services)':<40} {total:>8}")
        if pool_size:
            lines.append(f"  {'(ServicePool overhead)':<40} {pool_size - total:>8}")
        lines.append("")

    extras = [
        (name, size, type_name)
        for name, size, type_name in pool_members
        if not PRIMARY.search(type_name) and not name.startswith("(base)")
    ]
    if extras:
        lines.append("ServicePool members")
        for name, size, type_name in extras:
            lines.append(f"  {name:<40} {size:>8}  {_short(type_name)}")
    return "\n".join(lines) + "\n"


def read_dwarf(object_path, readelf):
    result = subprocess.run(
        [readelf, "--debug-dump=info", str(object_path)],
        check=True,
        capture_output=True,
        text=True,
    )
    return index_classes(parse_readelf(result.stdout))


def find_readelf():
    for name in ("arm-none-eabi-readelf", "readelf"):
        path = shutil.which(name)
        if path:
            return path
    raise SystemExit("readelf not found")


def find_object(root):
    matches = sorted(Path(root).rglob("ServicePool.cpp.o"), key=lambda path: path.stat().st_mtime)
    if not matches:
        raise SystemExit(f"ServicePool.cpp.o not found under {root}")
    return matches[-1]


def self_test():
    classes = {
        "ServicePool": (1060, [
            ("storage", 1036, "StorageService"),
            ("counters", 20, "Map<short int, 8>"),
            ("seq", 2, "short int"),
        ]),
        "StorageService": (1036, [
            ("packetStores", 1028, "Map<PacketStore, 4>"),
            ("(base) Service", 2, "Service"),
            ("mode", 1, "unsigned char"),
        ]),
    }
    report = format_report(classes, "self-test")
    assert "ServicePool" in report and "1060" in report
    assert "packetStores" in report
    assert "counters" in report
    assert "data" not in report
    print(report)
    print("self-test ok")
    return 0


def main(argv):
    if "--self-test" in argv:
        return self_test()

    label = ""
    object_path = None
    search = None
    args = argv[1:]
    while args:
        if args[0] == "--label":
            label = args[1]
            args = args[2:]
        elif args[0] == "--object":
            object_path = Path(args[1])
            args = args[2:]
        elif args[0] == "--search":
            search = Path(args[1])
            args = args[2:]
        else:
            raise SystemExit(f"unknown argument: {args[0]}")

    if object_path is None and search is not None:
        object_path = find_object(search)
    if object_path is None:
        raise SystemExit("usage: report_service_sizes.py --object FILE|--search DIR [--label NAME]")

    classes = read_dwarf(object_path, find_readelf())
    sys.stdout.write(format_report(classes, label))
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv))
