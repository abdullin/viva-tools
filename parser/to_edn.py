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
        bs = [proto_to_edn(x) for x in x.behavior]
        ns = [net_to_edn(x) for x in x.net]
        result.append((kw("object"), proto_to_edn(x.proto), bs + ns))

    return result


def proto_to_edn(x: Proto):
    attrs = {}
    for k in x.attrs:
        attrs[kw(k)] = x.attrs[k]
    if x.gui:
        attrs[kw('gui')] = x.gui
    return (kw("proto"), x.name, x.inputs, x.outputs, attrs)


def net_to_edn(x: Transport):
    attrs = {}
    if x.gui:
        attrs[kw('gui')] = x.gui
    return (kw("net"), x.input, x.output, attrs)
