from dataclasses import dataclass, field
from typing import List, Dict, Optional

@dataclass
class Pos:
    x: int
    y: int

@dataclass
class Pin:
    is_input: bool
    data_type: str
    name: str
    parent: 'Symbol'
    net: 'Transport'


@dataclass(eq=False)
class Symbol:
    type: str # Input, Output, Junction, or something else
    pins: List[Pin]
    pos: Optional[Pos]
    attrs: Dict

    def __init__(self, type: str):
        self.type = type
        self.pins = []
        self.pos = None
        self.attrs = {}

    def add_input(self, data_type: str, name: str = None):
        p = Pin(True, data_type, name, self, None)
        self.pins.append(p)

    def add_output(self, data_type: str, name: str = None):
        p = Pin(False, data_type, name, self, None)
        self.pins.append(p)
        
    def get_pin(self, is_input: bool, num: int):
        pass

@dataclass
class Text:
    text: str
    pos: Pos


@dataclass
class Transport:
    left: Pin
    right: Pin
    pos: List[Pos]

    def __init__(self, left: Pin, right: Pin):
        self.left = left
        self.right = right
        self.pos =[]


@dataclass
class Sheet:
    type: str
    symbols: List[Symbol]
    net: List[Transport]
    texts: List[Text]

    def add_input(self, name: str, data_type: str) -> 'Symbol':
        sym = Symbol("Input")
        sym.add_output(data_type, name)
        self.symbols.append(sym)
        return sym


    def add_output(self, name: str, data_type: str) -> 'Symbol':
        sym = Symbol("Output")
        sym.add_input(data_type, name)
        self.symbols.append(sym)
        return sym

    def add_text(self, text: str, pos: Pos):
        self.texts.append(Text(text, pos))

    def add_junction(self, data_type: str) -> Symbol:
        sym = Symbol("Junction")
        sym.add_input(data_type)
        sym.add_output(data_type)
        sym.add_output(data_type)
        sym.add_output(data_type)
        self.symbols.append(sym)
        return sym

    def add_symbol(self, type: str):
        sym = Symbol(type)
        self.symbols.append(sym)
        return sym


    def connect(self, left_pin: Pin, right_pin: Pin) -> Transport:
        t = Transport(left_pin, right_pin)
        left_pin.net = t
        right_pin.net = t
        self.net.append(t)
        return t


def make_sheet(type: str) -> Sheet:
    return Sheet(type, [], [], [])




@dataclass
class Dataset:
    name: str
    args: List[str]
    context: int = 0
    color: int = 0
    tree: List[str] = None
    com: Optional[int] = None

def make_dataset(name: str, args: List[str]) ->Dataset:
    return Dataset(name, args)

@dataclass
class File:
    prototypes: List[Symbol]
    datasets: List[Dataset]
    sheets: List[Sheet]

    def add_prototype(self, type: str) -> Symbol:
        sym = Symbol(type)
        self.prototypes.append(sym)
        return sym

def make_file() -> File:
    return File([], [], [])



