from dto import *

def input(ctx, x, y):
    pass

class Grid:
    """
    Grid of pins and junctions. It is used to store and lookup positions
    """
    def __init__(self):
        self.junctions = {}
        self.outputs = {}
        self.inputs = {}

    def add_junction(self, j: Junction, pos):
        self.junctions[j.id] = pos

    def locate_output(self, r: PinRef):
        if r.type == "Junction":
            return self.junctions[r.id]

        key = f"{r.type}:{r.id if r.id else ''}-{r.io_num}"
        value = self.outputs.get(key)
        if value:
            return value

        print([x for x in self.outputs.keys()])
        print([x for x in self.inputs.keys()])

        raise KeyError(f"Can't find key '{key}'. Matches are: {[x for x in self.outputs.keys()]}")


    def locate_input(self, r: PinRef):
        if r.type == "Junction":
            return self.junctions[r.id]

        return self.inputs[f"{r.type}:{r.id if r.id else ''}-{r.io_num}"]

    def add_output(self, r: Symbol, num: int, pos):
        s = f"{r.type}:{r.id if r.id else ''}-{num}"
        self.outputs[s] = pos

    def add_io(self, r:Header, pos):
        if r.is_input:
            s = f"Input:{r.id if r.id else ''}-0"
            self.outputs[s] = pos
        else:
            s = f"Output:{r.id if r.id else ''}-0"
            self.inputs[s]=pos


    def add_input(self, r:Symbol, num: int, pos):
        s = f"{r.type}:{r.id if r.id else ''}-{num}"
        self.inputs[s] = pos


