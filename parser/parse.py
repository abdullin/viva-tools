import re
from collections import defaultdict
from typing import Optional, List, Tuple

from dto import *

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
        return {}

    text = text.strip()
    if not text.startswith("Attributes"):
        return {}

    attrs = {}

    pairs = text[10:].strip().split(',')

    for pair in pairs:
        ps = pair.split("=")
        attrs[ps[0].strip()] = ps[1].strip('" ')
    return attrs


net = re.compile("(?P<left>.+)( = )(?P<right>.+);(\s*//_(?P<attributes>.*))?")

def extract_attr_gui(text) -> Optional[List[Tuple[int,int]]]:
    if not text:
        return None
    if not text.startswith("GUI"):
        return None

    coords = []
    pairs = text[3:].strip().split(', ')
    for pair in pairs:
        ps = pair.split(',')
        coords.append((int(ps[0]), int(ps[1])))
    return coords


def parse_transport_def(text) -> Transport:
    m = net.search(text)
    if not m:
        raise ValueError(f"Unexpected net: {text}")
    gui = extract_attr_gui(m.group('attributes'))

    return Transport(m.group('left'), m.group('right'), gui)



def parse_proto(l:str) -> Proto:
    m = obj_def.search(l)

    try:
        nam = m.group('name').strip()
        outputs = extract_pairs(m.group('outputs'))
        inputs = extract_pairs(m.group('inputs'))
        gui = extract_attr_gui(m.group('attributes'))
        attrs = extract_attributes(m.group('attributes'))
    except:
        print(f"Problem parsing proto: {l}")
        raise

    return Proto(nam, inputs, outputs, attrs, gui)


def parse_object_def(l, body) -> Object:
    proto = parse_proto(l)

    prototypes = [parse_proto(x) for x in body['proto']]
    net = [parse_transport_def(x) for x in body['behavior']]
    return Object(proto, prototypes, net)


def parse_dataset(l) -> Dataset:
    m = ds.search(l)

    name = m.group(1).strip(' "')
    args = [x.strip() for x in m.group(2).strip().split(",")]
    attribs = [x.strip() for x in m.group(3).strip().split(",")]

    context = 0
    color = 0
    tree = None
    com = None

    if len(attribs) >= 1:
        context = int(attribs[0])
    if len(attribs) >= 2:
        color = int(attribs[1])
    if len(attribs) >= 3:
        tree = attribs[2].split("\\")
    if len(attribs) >= 4:
        com = int(attribs[3])

    return Dataset(name, args, context,color, tree, com)



def parse_text(name) -> File:
    with open(name) as f:
        lines = f.readlines()


    datasets = []
    objects = []
    prototypes = []
    i = 0
    while i < len(lines):
        l = lines[i].strip("\r\n")

        if l.startswith("DataSet"):
            data = parse_dataset(lines[i])
            i += 1
            datasets.append(data)
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

            if body:
                objects.append(parse_object_def(declaration, body))
            else:
                prototypes.append(parse_proto(declaration))


            continue


        raise ValueError("Unknown line", l)
    return File(datasets, objects, prototypes)
