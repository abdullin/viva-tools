from collections import OrderedDict

import edn_format

from dto import *

kw = edn_format.Keyword


def write_file(output, f: File):
    parsed = to_edn(f)
    with open(output, 'w') as out:
        for p in parsed:
            out.write(edn_format.dumps(p))
            out.write("\n")


def compare_file(output: str, f: File) -> List[Assertion]:
    with open(output) as out:
        out_text = out.read()
        loaded_edn = edn_format.loads_all(out_text)
    parsed_edn = to_edn(f)

    results = []

    for (l, p) in zip(loaded_edn, parsed_edn):
        parsed_typed = edn_format.loads(edn_format.dumps(p))

        results.append(Assertion(
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
        result.append(proto_to_edn(x))

    for x in f.objects:
        bs = [symbol_to_edn(x) for x in x.behavior]
        inputs = [header_to_edn(x) for x in x.inputs]
        outputs = [header_to_edn(x) for x in x.outputs]
        ns = [net_to_edn(x) for x in x.net]
        texts = [text_to_edn(x) for x in x.texts]
        js = [junction_to_edn(x) for x in x.junctions]

        hi = [pin_to_edn(i) for i in x.h_inputs]
        ho = [pin_to_edn(i) for i in x.h_outputs]

        result.append((kw("module"), (x.name, hi, ho, x.attrs), inputs + outputs + bs + js + ns + texts))

    return result


def junction_to_edn(j: Junction):
    return kw("junction"), j.type, j.id, (j.pos.x, j.pos.y)


def pin_to_edn(x: Pin):
    return x.name, x.type


def header_to_edn(x: Header):
    return kw("input" if x.is_input else "output"), x.name, x.type, x.id, (x.pos.x, x.pos.y), x.attrs


def symbol_to_edn(x: Symbol):
    attrs = {}
    for k in x.attrs:
        attrs[kw(k)] = x.attrs[k]

    inputs = [pin_to_edn(i) for i in x.inputs]
    outputs = [pin_to_edn(i) for i in x.outputs]
    return kw("symbol"), x.type, x.id, inputs, outputs, (x.pos.x, x.pos.y), attrs


def proto_to_edn(x: Proto):
    attrs = {}
    for k in x.attrs:
        attrs[kw(k)] = x.attrs[k]
    if x.pos:
        attrs[kw('gui')] = (x.pos.x, x.pos.y)

    inputs = [pin_to_edn(i) for i in x.inputs]
    outputs = [pin_to_edn(i) for i in x.outputs]
    name = SymbolRef(x.type, x.id).to_str()
    return kw("proto"), name, inputs, outputs, attrs


def text_to_edn(x: Text):
    return kw("text"), x.text, (x.pos.x, x.pos.y)


def net_ref_to_edn(x: PinRef):
    return x.type, x.id, x.io_num


def net_to_edn(x: Transport):
    pos = [(x.x, x.y) for x in x.gui]
    left = net_ref_to_edn(x.left)
    right = net_ref_to_edn(x.right)
    return kw("net"), left, right, pos
