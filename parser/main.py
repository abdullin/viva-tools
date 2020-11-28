#!/usr/bin/env python

import edn_format
import os
import re
import deepdiff as dd

def sep():
    print("=" * 80)

def main():
    root = "cases"
    for path, subdirs, files in os.walk(root):
        for name in files:
            input = os.path.join(path, name)

            base, ext = os.path.splitext(input)

            if ext not in [".ipg", ".idl"]:
                continue
            output = base + ".edn"
            print(output)
            parsed_edn = parse_text(input)

            for p in parsed_edn:
                #print(edn_format.dumps(p))
                pass




            with open(output) as out:
                out_text= out.read()
                loaded_edn = edn_format.loads_all(out_text)

            for (l, p) in zip(loaded_edn, parsed_edn):
                parsed_typed = edn_format.loads(edn_format.dumps(p))
                delta = dd.DeepDiff(l, parsed_typed)

                if delta:
                    print("Mismatch")
                    print(delta)



ds = re.compile("DataSet(.+)= \((.+)\); //_\sAttributes\s(.+)")


def parse_text(name):

    with open(name) as f:
        lines = f.readlines()


    file = []

    for i in range(0, len(lines)):
        l = lines[i].strip("\r\n")

        if l.startswith("DataSet"):
            m = ds.search(l)

            args = [x.strip() for x in m.group(2).strip().split(",")]
            attribs = [x.strip() for x in m.group(3).strip().split(",")]

            attr_dict = {}

            if (len(attribs)>0):
                ctx = int(attribs[0])
                attr_dict[edn_format.Keyword("context")] = ctx
            if (len(attribs)>=2):
                val = int(attribs[1])
                if val != 0:
                    attr_dict[edn_format.Keyword("color")] = val
            if (len(attribs)>=3):
                val = attribs[2].split("\\")
                attr_dict[edn_format.Keyword("tree")] = val

            if (len(attribs)>=4):
                val = attribs[3]
                attr_dict[edn_format.Keyword("com")] = int(val)

            name = m.group(1).strip(' "')

            data = [
                edn_format.Keyword("DataSet"),
                name,
                args,
                attr_dict
            ]
            file.append(data)
    return file











if __name__ == "__main__":
    main()