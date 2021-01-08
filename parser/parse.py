import re
from collections import defaultdict
from dto import *

import model as mod

ds = re.compile("DataSet(.+)= \((.+)\); //_\sAttributes\s(.+)")
obj_def = re.compile(
    'Object (\((?P<outputs>[\n\w,\s\d\.:"<>=*]+?)\))?\s*(?P<name>[\w\d\:\.\-\_>$"]+)[\s\n]*(\((?P<inputs>[\n\w\s,*"]+?)\))?[\s;]*(//_(?P<attributes>.*))?',
    re.MULTILINE)
pair_split = re.compile('(?P<type>([\w]+)|("[^"]+"))\s+(?P<name>([\w]+)|("[^"]+"))')

net = re.compile("(?P<left>.+)\.(?P<left_num>\d+)( = )(?P<right>.+)\.(?P<right_num>\d+);(\s*//_(?P<attributes>.*))?")

def extract_pins(line: str) -> List[Pin]:
    if not line:
        return []
    clean = line.strip()
    result = []

    pairs = clean.split(",")
    for p in pairs:

        match = pair_split.search(p)
        if not match:
            raise ValueError(f"problem splitting pair '{p}' from '{line}'")
        type = match.group('type').strip("\" ")
        name = match.group('name').strip("\" ")

        p = Pin(type, name)
        result.append(p)
    return result


def extract_attributes(text: str) -> Dict:
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




def extract_attr_gui(text) -> List[mod.Pos]:
    if not text:
        return []
    if not text.startswith("GUI"):
        return []

    coords = []
    pairs = text[3:].strip().split(', ')
    for pair in pairs:
        ps = pair.split(',')
        coords.append(mod.Pos(int(ps[0]), int(ps[1])))
    return coords


def parse_proto_text(text: str, linum: int) -> Tuple[str, mod.Pos]:
    if not text.startswith("Object Text;  //_GUI "):
        raise ValueError(f"Unexpected text start: {text}")
    items = text[21:].strip().split(',', 2)
    return items[2].replace("\u0001", "\n"), mod.Pos(int(items[0]), int(items[1]))


def parse_symbol_reference(text: str) -> SymbolRef:
    id = None
    s = text.split(':')
    if len(s) == 2:
        id = s[1]
    return SymbolRef(s[0], id)


def parse_net_reference(text: str, num: str) -> PinRef:
    io_num = int(num)
    id = None
    text = text.strip('"')
    split = text.split(':')
    if len(split) == 2:
        id = split[1]
    type = split[0]
    return PinRef(type, id, io_num)


def parse_transport_def(text) -> Transport:
    m = net.search(text)
    if not m:
        raise ValueError(f"Unexpected net: {text}")
    gui = extract_attr_gui(m.group('attributes'))
    # output from something is our input and vise versa

    "transport goes INPUT -> OUTPUT"
    output = parse_net_reference(m.group('left'), m.group('left_num'))
    input = parse_net_reference(m.group('right'), m.group('right_num'))

    if gui:
        gui.reverse()

    return Transport(input, output, gui)


def parse_proto(l: str, linum: int = 0) -> Proto:
    m = obj_def.search(l)

    try:
        nam = m.group('name').strip(' "')
        assert nam != "("

        ref = parse_symbol_reference(nam)
        outputs = extract_pins(m.group('outputs'))

        in_s = m.group('inputs')
        inputs = extract_pins(in_s)
        gui = extract_attr_gui(m.group('attributes'))
        attrs = extract_attributes(m.group('attributes'))
    except:
        print(f"Problem parsing proto @{linum}: <{l}>")
        raise

    if ref.symbol_type == "Junction":
        if len(gui) != 1:
            raise ValueError(f"Junction at {linum} should have gui: {l}")

    if gui:
        if len(gui) != 1:
            raise ValueError("Prototype is expected to have one coord pair")
        gui = gui[0]

    return Proto(ref.symbol_type, ref.id, inputs, outputs, attrs, gui)


def parse_object_def(l, body) -> mod.Sheet:
    proto = parse_proto(l)

    proto_strs = body['proto']
    # reassemble into lines
    proto_joined = []
    for i, p in proto_strs:
        if p.startswith("//_") or p.startswith(",") or p.startswith("("):
            li, lp = proto_joined[-1]
            lp += "\n" + p
            proto_joined[-1] = (li, lp)
            continue
        proto_joined.append((i, p))

    # print(proto_joined)

    prototypes = [parse_proto(x, i) for i, x in proto_joined]

    lookup = {}

    class LookupLeaf:
        def __init__(self, s: mod.Symbol):
            self.symbol = s
            self.inputs = [x for x in s.pins if x.is_input]
            self.outputs = [x for x in s.pins if not x.is_input]

    sheet = mod.make_sheet(proto.type)

    for x in prototypes:
        if x.type == "Input":
            # reposition input pos to the transport location
            x.pos.x += 2
            x.pos.y += 1

            sym = sheet.add_input(x.outputs[0].name, x.outputs[0].data_type)
            sym.pos = x.pos
            lookup[f'Input:{x.id or ""}'] = LookupLeaf(sym)
            continue
        if x.type == "Output":
            # move pos to the real transport location
            x.pos.x += 2
            x.pos.y += 1

            sym = sheet.add_output(x.inputs[0].name, x.inputs[0].data_type)
            sym.pos = x.pos
            lookup[f'Output:{x.id or ""}'] = LookupLeaf(sym)
            continue
        if x.type == "Junction":
            x.pos.x += 1
            x.pos.y += 1
            if x.attrs:
                raise ValueError("Junctions have attrs!")

            sym = sheet.add_junction(x.inputs[0].data_type)
            sym.pos = x.pos
            lookup[f'Junction:{x.id or ""}'] = LookupLeaf(sym)
            continue

        sym = sheet.add_symbol(x.type)
        sym.pos = x.pos

        for i in x.inputs:
            sym.add_input(i.data_type, i.name)
        for o in x.outputs:
            sym.add_output(o.data_type, o.name)
        lookup[f'{x.type}:{x.id or ""}'] = LookupLeaf(sym)

    net = [parse_transport_def(x) for i, x in body['behavior']]

    for n in net:
        "Convert references to the model"
        left = lookup[f'{n.left.symbol_type}:{n.left.id or ""}']
        right = lookup[f'{n.right.symbol_type}:{n.right.id or ""}']

        left_pin = left.outputs[n.left.io_num]
        right_pin = right.inputs[n.right.io_num]

        sheet.connect(left_pin, right_pin).pos = n.gui

    for i, x in body['text']:
        txt, pos = parse_proto_text(x, i)
        sheet.add_text(txt, pos)

    return sheet


def parse_dataset(l) -> mod.Dataset:
    m = ds.search(l)

    name = m.group(1).strip(' "')
    args = [x.strip() for x in m.group(2).strip().split(",")]

    sym = mod.make_dataset(name, args)
    attribs = [x.strip() for x in m.group(3).strip().split(",")]

    if len(attribs) >= 1:
        sym.context = int(attribs[0])
    if len(attribs) >= 2:
        sym.color = int(attribs[1])
    if len(attribs) >= 3:
        sym.tree = attribs[2].split("\\")
    if len(attribs) >= 4:
        sym.com = int(attribs[3])

    return sym


def parse_text(name, detailed=False) -> mod.File:
    with open(name) as f:
        lines = f.readlines()

    file = mod.make_file()

    i = 0
    while i < len(lines):
        l = lines[i].strip("\r\n")

        if l.startswith("DataSet"):
            data = parse_dataset(lines[i])
            i += 1

            file.datasets.append(data)
            continue

        if l.startswith("Object "):
            body = defaultdict(list)
            body_section = None
            head = ""

            for l in lines[i:]:
                s = l.strip()

                i += 1
                if s.startswith("Object Text;"):
                    body["text"].append((i, s))
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
                if body_section and s == "}":
                    break
                if not body_section and l.endswith(";"):
                    break

                if body_section:
                    clean = l.strip()
                    if clean:
                        body[body_section].append((i, clean))
                else:
                    head += l

            if detailed:
                print(f"HEAD: {head}")

                for k in body:
                    print(f"BODY[{k}]: {body[k]}")

            if body:
                file.sheets.append(parse_object_def(head, body))
            else:
                proto = parse_proto(head)

                sym = file.add_prototype(proto.type)
                for p in proto.inputs:
                    sym.add_input(p.name, p.data_type)
                for p in proto.outputs:
                    sym.add_output(p.name, p.data_type)

            continue

        if l.strip() == '':
            i += 1
            continue

        raise ValueError("Unknown line", l)
    return file
