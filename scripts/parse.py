#! /usr/bin/env python3
import re
import sys
import json

GET_WORD = re.compile(r"WORD: (?P<word>.....)")
GET_COUNT_WORD = re.compile(r"Adding (?P<count>\d+) for (?P<word>.....)")

if __name__ == "__main__":
    lines: list[str] = sys.stdin.readlines()
    as_dict: dict[str, dict[str, int]] = {}
    word = ""

    for line in lines:
        if "WORD" in line:
            word = GET_WORD.search(line).group("word")
            as_dict[word] = {}
        else:
            matched = GET_COUNT_WORD.search(line)
            if matched is None:
                continue
            as_dict[word][matched.group("word")] = matched.group("count")

    for i, (k, v) in enumerate(as_dict.items()):
        duplicate_count = len(set(v.values()))
        print(f"For {k}: {duplicate_count} unique out of {len(v.values())}")

