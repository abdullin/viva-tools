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
def main(create_missing, format, detailed):

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

            if ext not in [".ipg", ".idl", ".sd"]:
                continue
            output = base +target_ext
            print(output)
            data = parse_text(input)


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
                    print(delta.pretty())
                    if detailed:
                        print("Expect: " + a.expected_str)
                        print("Actual: " + a.actual_str)




if __name__ == "__main__":
    main()
