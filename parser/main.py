#!/usr/bin/env python
import click
import edn_format
import os
import re
import deepdiff as dd


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
            parsed_edn = parse_text(input)


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
                #print(edn_format.dumps(p))
                parsed_typed = edn_format.loads(edn_format.dumps(p))
                delta = dd.DeepDiff(l, parsed_typed)

                if delta:
                    print("Mismatch")
                    print(delta)


ds = re.compile("DataSet(.+)= \((.+)\); //_\sAttributes\s(.+)")
obj_def = re.compile("Object \((.+?)\)(.+?)\(([.\n,\w\W]+?)\)[\s;]*//_ Attributes\s(.*)", re.MULTILINE)

def extract_pairs(line: str):
    pairs = line.strip().split(",")
    result = []
    for p in pairs:
        pair = p.strip().split(" ")
        if len(pair)!=2:
            raise ValueError(f"problem splitting pairs from '{line}'")
        result.append((pair[0].strip(), pair[1].strip()))
    return result

def parse_object_def(l, body):


    print(l)
    sep()
    print(body)

    m = obj_def.search(l)

    nam = m.group(2).strip()
    outputs = extract_pairs(m.group(1))
    inputs = extract_pairs(m.group(3))

    attr_pairs = m.group(4).split(',')
    attrs = {}
    for pair in attr_pairs:
        ps = pair.split("=")
        attrs[edn_format.Keyword(ps[0].strip())] = ps[1].strip('" ')

    data = (
        edn_format.Keyword("Object"),
        nam,
        inputs,
        outputs,
        attrs,
    )

    return data


def parse_dataset(l):
    m = ds.search(l)

    args = [x.strip() for x in m.group(2).strip().split(",")]
    attribs = [x.strip() for x in m.group(3).strip().split(",")]

    attr_dict = {}

    if (len(attribs) > 0):
        ctx = int(attribs[0])
        attr_dict[edn_format.Keyword("context")] = ctx
    if (len(attribs) >= 2):
        val = int(attribs[1])
        if val != 0:
            attr_dict[edn_format.Keyword("color")] = val
    if (len(attribs) >= 3):
        val = attribs[2].split("\\")
        attr_dict[edn_format.Keyword("tree")] = val

    if (len(attribs) >= 4):
        val = attribs[3]
        attr_dict[edn_format.Keyword("com")] = int(val)

    name = m.group(1).strip(' "')

    data = (
        edn_format.Keyword("DataSet"),
        name,
        args,
        attr_dict
    )

    return data


def parse_text(name):
    with open(name) as f:
        lines = f.readlines()

    file = []

    i = 0
    while i < len(lines):
        l = lines[i].strip("\r\n")

        if l.startswith("DataSet"):
            data = parse_dataset(lines[i])
            i += 1
            file.append(data)
            continue

        if l.startswith("Object "):
            behavior_started = False
            declaration = ""
            body = ""

            for l in lines[i:]:

                i+=1
                if l.startswith("{"):
                    behavior_started = True
                    continue
                if behavior_started and l == "}":
                    break
                if not behavior_started and l.endswith(";"):
                    break


                if behavior_started:
                    body += l
                else:
                    declaration += l


            data = parse_object_def(declaration, body)
            file.append(data)
            continue


        raise ValueError("Unknown line", l)
    return file


if __name__ == "__main__":
    main()
