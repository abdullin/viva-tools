from collections import OrderedDict

import edn_format

from dto import *

kw = edn_format.Keyword


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
