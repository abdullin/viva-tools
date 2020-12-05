import json

from dto import *


def proto_to_dict(x: Proto) -> Dict:
    p = {
        'kind': 'proto',
        'name': x.name,
        'inputs': x.inputs,
        'outputs': x.outputs,
        'attrs': x.attrs,
    }

    if x.gui:
        p['gui'] = (x.gui.x, x.gui.y)
    return p


def write_file(output: str, p: File):
    with open(output, 'w') as w:

        for f in p.datasets:
            j = {
                'kind': 'dataset',
                'name': f.name,
                'args': f.args,

            }
            if f.context:
                j['context'] = f.context
            if f.color:
                j['color'] = f.color
            if f.tree:
                j['tree'] = f.tree
            if f.com:
                j['com'] = f.com

            json.dump(j, w)
            w.write("\n")

        for x in p.prototypes:

            json.dump(proto_to_dict(x), w)
            w.write("\n")

        for x in p.objects:
            p = {
                'kind': 'obj',
                'proto': proto_to_dict(x.proto),
                'behavior': [proto_to_dict(x) for x in x.behavior]
            }
            json.dump(p, w)
            w.write("\n")


def compare_file(output: str, f: File) -> List[Assertion]:
    return []

