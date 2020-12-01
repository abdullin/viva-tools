#!/usr/bin/env python
from collections import defaultdict

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
                try:
                    parsed_typed = edn_format.loads(edn_format.dumps(p))
                except:
                    print(edn_format.dumps(p))
                    raise
                delta = dd.DeepDiff(l, parsed_typed)

                if delta:
                    print("Mismatch")
                    print(delta)


ds = re.compile("DataSet(.+)= \((.+)\); //_\sAttributes\s(.+)")
obj_def = re.compile("Object (\((?P<outputs>.+?)\))?(?P<name>.+?)(\((?P<inputs>[.\n,\w\W]+?)\))?[\s;]+(//_(?P<attributes>.*))?", re.MULTILINE)

def extract_pairs(line: str):
    if not line:
        return []
    clean = line.strip()
    result = []

    pairs = clean.split(",")
    for p in pairs:
        pair = p.strip().split(" ")
        if len(pair)!=2:
            raise ValueError(f"problem splitting pairs from '{line}'")
        result.append((pair[0].strip(), pair[1].strip()))
    return result

def extract_attributes(text:str):
    if not text:
        return None
    text = text.strip()

    attrs = {}

    if text.startswith("Attributes"):
        pairs = text[10:].strip().split(',')


        for pair in pairs:
            ps = pair.split("=")
            attrs[edn_format.Keyword(ps[0].strip())] = ps[1].strip('" ')
        return attrs

    if text.startswith('GUI'):
        coords = []
        pairs = text[3:].strip().split(', ')
        for pair in pairs:
            ps = pair.split(',')
            coords.append((int(ps[0]), int(ps[1])))
        attrs[edn_format.Keyword("gui")] = coords
        return attrs


    raise ValueError(text)

def parse_object_def(l, body):


    m = obj_def.search(l)

    try:

        nam = m.group('name').strip()
        outputs = extract_pairs(m.group('outputs'))
        inputs = extract_pairs(m.group('inputs'))
        attrs = extract_attributes(m.group('attributes'))
    except:
        print(f"Problem parsing: {l}")
        raise

    data = (
        edn_format.Keyword("proto"),
        nam,
        inputs,
        outputs,
        attrs,
    )
    if not body:
        return data


    prototypes = [parse_object_def(x, None) for x in body['proto']]
    return (
        edn_format.Keyword("object"),
        data,
        prototypes,
    )


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

            body = defaultdict(list)
            body_section = None
            declaration = ""

            for l in lines[i:]:
                s = l.strip()

                i+=1
                if l.startswith("{"):
                    body_section = 'doc'
                    continue
                if s == "//_ Object Prototypes":
                    body_section = 'proto'
                    continue
                if s == "//_ Behavior Topology":
                    body_section = 'behavior'
                    continue
                if body_section and l == "}":
                    break
                if not body_section and l.endswith(";"):
                    break


                if body_section:
                    clean = l.strip()
                    if clean:
                        body[body_section].append(clean)
                else:
                    declaration += l


            data = parse_object_def(declaration, body)
            file.append(data)
            continue


        raise ValueError("Unknown line", l)
    return file


if __name__ == "__main__":
    main()
