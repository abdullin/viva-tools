#!/usr/bin/env python
from collections import defaultdict

import click
import edn_format
import os
import re
import deepdiff as dd

from dto import *
from parse import parse_text
from to_edn import to_edn


def sep():
    print("=" * 80)

@click.command()
@click.option("--create-missing", is_flag=True)
def main(create_missing):
    root = "cases"
    for path, subdirs, files in os.walk(root):
        for name in files:
            input = os.path.join(path, name)

            base, ext = os.path.splitext(input)

            if ext not in [".ipg", ".idl", ".sd"]:
                continue
            output = base + ".edn"
            print(output)
            data = parse_text(input)
            parsed_edn = to_edn(data)


            if not os.path.exists(output):
                if create_missing:
                    print(f"Creating file {output}")
                    with open(output, 'w') as out:
                        for p in parsed_edn:
                            out.write(edn_format.dumps(p))
                            out.write("\n")
                else:
                    print(f"FILE NOT FOUND: {output}")
                    for p in parsed_edn:
                        print(edn_format.dumps(p))
                        return

            with open(output) as out:
                out_text = out.read()
                loaded_edn = edn_format.loads_all(out_text)

            for (l, p) in zip(loaded_edn, parsed_edn):
                try:
                    parsed_typed = edn_format.loads(edn_format.dumps(p))
                except:
                    print(edn_format.dumps(p))
                    raise
                delta = dd.DeepDiff(l, parsed_typed)

                if delta:
                    print("Mismatch")
                    print(delta)
                    print("Expect: " + edn_format.dumps(l))
                    print("Parsed: " + edn_format.dumps(p))





if __name__ == "__main__":
    main()
