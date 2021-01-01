#!/usr/bin/env python
from collections import defaultdict

import click
import edn_format
import os
import deepdiff as dd

from parse import parse_text


def sep():
    print("=" * 80)

@click.command()
@click.option("--create-missing", is_flag=True)
@click.option("--format", type=str, default="edn")
@click.option("--detailed", is_flag=True)
@click.option("--max-diffs", default=1000)
@click.option("--filter", type=str, default=None)
@click.option("--canary", is_flag=True, default=False)
def main(create_missing, format, detailed, max_diffs=1000, filter=None, canary=False):

    if format == "edn":
        import to_edn as target
        target_ext = ".edn"
    elif format == "json":
        import to_json as target
        target_ext = ".json"
    else:
        raise ValueError("unexpected format " + format)


    root = "cases"
    for path, subdirs, files in os.walk(root):
        for name in files:
            input = os.path.join(path, name)

            base, ext = os.path.splitext(input)

            if filter and filter not in input:
                continue


            if not canary and 'dexter-2018-hd.idl' in input:
                continue
            if not canary and 'dexter_main.ipg' in input:
                continue

            if ext not in [".ipg", ".idl", ".sd"]:
                continue
            output = base +target_ext
            print(output)
            data = parse_text(input, detailed)

            if not os.path.exists(output):
                if create_missing:
                    print(f"Creating file {output}")
                    target.write_file(output, data)

                else:
                    print(f"FILE NOT FOUND: {output}")
            assertions = target.compare_file(output, data)
            for a in assertions:
                delta = dd.DeepDiff(a.expected, a.actual)
                if delta:
                    pretty = delta.pretty()
                    lines = pretty.splitlines()
                    for l in lines[:max_diffs]:
                        print(l)
                    if len(lines)> max_diffs:
                        print(f"... skipped more of {len(lines)-max_diffs}")
                    if detailed:
                        print("Expect: " + a.expected_str)
                        print("Actual: " + a.actual_str)




if __name__ == "__main__":
    main()
