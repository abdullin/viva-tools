from collections import OrderedDict

import edn_format

from model import *
import dto as dto

kw = edn_format.Keyword


def write_file(output, f: File):
    parsed = to_edn(f)
    with open(output, 'w') as out:
        for p in parsed:
            out.write(edn_format.dumps(p))
            out.write("\n")


def compare_file(output: str, f: File) -> List[dto.Assertion]:
    with open(output) as out:
        out_text = out.read()
        loaded_edn = edn_format.loads_all(out_text)
    parsed_edn = to_edn(f)

    results = []

    for (l, p) in zip(loaded_edn, parsed_edn):
        parsed_typed = edn_format.loads(edn_format.dumps(p))

        results.append(dto.Assertion(
            l,
            parsed_typed,
            edn_format.dumps(l),
            edn_format.dumps(p)))
    return results


def to_edn(f: File):
    result = []

    for x in f.datasets:
        attrs = OrderedDict()
        if x.color:
            attrs[kw('color')] = x.color
        if x.context:
            attrs[kw('context')] = x.context
        if x.tree:
            attrs[kw('tree')] = x.tree
        if x.com:
            attrs[kw('com')] = x.com
        result.append((kw("DataSet"), x.name, x.args, attrs))

    for x in f.prototypes:
        result.append(prototype_to_edn(x))

    for x in f.sheets:
        mapper = {}

        # let's give unique IDs to all symbols

        for i in x.symbols:
            mapper[i] = len(mapper)

        ss = [symbol_to_edn(mapper, x) for x in x.symbols]



        ns = [net_to_edn(mapper, x) for x in x.net]
        texts = [text_to_edn(x) for x in x.texts]


        result.append((kw("sheet"), x.type, ss, ns, texts))


    return result



def pin_to_edn(x: Pin):
    return x.name, x.data_type



def symbol_to_edn(mapper: Dict, x: Symbol):
    attrs = {}
    for k in x.attrs:
        attrs[kw(k)] = x.attrs[k]

    uid = mapper[x]
    if x.pos is None:
        print(x.type)

    pos = (x.pos.x, x.pos.y)

    if x.type == "Input":
        return kw("input"), x.pins[0].name, x.pins[0].data_type, uid, pos, x.attrs

    if x.type == "Output":
        return kw("output"), x.pins[0].name, x.pins[0].data_type, uid, pos, x.attrs

    if x.type == "Junction":
        return kw("junction"), x.type, uid, pos

    inputs = [(i.name, i.data_type) for i in x.pins if i.is_input]
    outputs = [(o.name, o.data_type) for o in x.pins if not o.is_input]
    return kw("symbol"), x.type, uid, inputs, outputs, pos, attrs


def prototype_to_edn(x: Symbol):
    attrs = {}
    for k in x.attrs:
        attrs[kw(k)] = x.attrs[k]
    assert x.pos is None

    inputs = [(i.name, i.data_type) for i in x.pins if i.is_input]
    outputs = [(i.name, i.data_type) for i in x.pins if not i.is_input]
    return kw("proto"), x.type, inputs, outputs, attrs


def text_to_edn(x: Text):
    return kw("text"), x.text, (x.pos.x, x.pos.y)


def net_to_edn(mapper: Dict, x: Transport):
    pos = [(x.x, x.y) for x in x.pos]

    left = mapper[x.left.parent]
    left_num = x.left.name
    right = mapper[x.right.parent]
    right_num = x.right.name

    return kw("net"), (left, left_num), (right,right_num), pos
