import re
from collections import defaultdict
from typing import Optional, List, Tuple

from dto import *

ds = re.compile("DataSet(.+)= \((.+)\); //_\sAttributes\s(.+)")
obj_def = re.compile('Object (\((?P<outputs>[\n\w,\s\d\.:]+?)\))?\s*(?P<name>[\w\d\:\.\-\_>"]+)\s*(\((?P<inputs>[\n\w\s,]+?)\))?[\s;]*(//_(?P<attributes>.*))?', re.MULTILINE)


def extract_pins(line: str) -> List[Pin]:
    if not line:
        return []
    clean = line.strip()
    result = []

    pairs = clean.split(",")
    for i, p in enumerate(pairs):
        pair = p.strip().split(" ")
        if len(pair)!=2:
            raise ValueError(f"problem splitting pairs from '{line}'")
        type = pair[0].strip()
        name = pair[1].strip()
        p = Pin(type, name, i)
        result.append(p)
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
        if len(ps) == 1:
            attrs[ps[0].strip()] = True
            continue

        attrs[ps[0].strip()] = ps[1].strip('" ')
    return attrs


net = re.compile("(?P<left>.+)( = )(?P<right>.+);(\s*//_(?P<attributes>.*))?")

def extract_attr_gui(text) -> List[Pos]:
    if not text:
        return []
    if not text.startswith("GUI"):
        return []

    coords = []
    pairs = text[3:].strip().split(', ')
    for pair in pairs:
        ps = pair.split(',')
        coords.append(Pos(int(ps[0]), int(ps[1])))
    return coords

def parse_proto_text(text:str) ->Text:
    if not text.startswith("Object Text;  //_GUI "):
        raise ValueError(f"Unexpected text start: {text}")
    items = text[21:].strip().split(',',2)
    return Text(items[2].replace("\u0001","\n"), Pos(int(items[0]), int(items[1])))

def parse_symbol_reference(text: str) -> SymbolRef:
    id = None
    s = text.split(':')
    if len(s)==2:
        id = s[1]
    return SymbolRef(s[0], id)


def parse_net_reference(text: str) -> PinRef:
    io_num = 0
    id = None
    s1 = text.split('.')
    if len(s1) == 2:
        io_num = int(s1[1])
    s2 = s1[0].split(':')
    if len(s2) == 2:
        id = s2[1]
    type = s2[0]
    return PinRef(type, id, io_num)

def parse_transport_def(text) -> Transport:
    m = net.search(text)
    if not m:
        raise ValueError(f"Unexpected net: {text}")
    gui = extract_attr_gui(m.group('attributes'))
    input = parse_net_reference(m.group('left'))
    output = parse_net_reference(m.group('right'))

    return Transport(input, output, gui)



def parse_proto(l:str) -> Proto:
    m = obj_def.search(l)

    try:
        nam =  m.group('name').strip()
        assert nam != "("

        ref = parse_symbol_reference(nam)
        outputs = extract_pins(m.group('outputs'))
        inputs = extract_pins(m.group('inputs'))
        gui = extract_attr_gui(m.group('attributes'))
        attrs = extract_attributes(m.group('attributes'))
    except:
        print(f"Problem parsing proto: <{l}>")
        raise



    if gui:
        if len(gui) != 1:
            raise ValueError("Prototype is expected to have one coord pair")
        gui = gui[0]

    # reposition junction at the real center
    if ref.type == "Junction":
        gui.x += 1
        gui.y += 1

    return Proto(ref.type, ref.id, inputs, outputs, attrs, gui)


def parse_object_def(l, body) -> Object:
    proto = parse_proto(l)

    proto_strs = body['proto']
    # reassemble into lines
    proto_joined = []
    for p in proto_strs:
        if p.startswith("//_") or p.startswith(",") or p.startswith("("):
            proto_joined[-1] += "\n" + p
            continue
        proto_joined.append(p)


    prototypes = [parse_proto(x) for x in proto_joined]
    net = [parse_transport_def(x) for x in body['behavior']]
    texts = [parse_proto_text(x) for x in body['text']]
    return Object(proto, prototypes, texts, net)


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
            head = ""

            for l in lines[i:]:
                s = l.strip()

                i+=1
                if s.startswith("Object Text;"):
                    body["text"].append(s)
                    continue

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
                    head += l

            if body:
                objects.append(parse_object_def(head, body))
            else:
                prototypes.append(parse_proto(head))


            continue


        raise ValueError("Unknown line", l)
    return File(datasets, objects, prototypes)
