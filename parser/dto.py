from dataclasses import dataclass
from typing import List, Tuple, Dict, Optional, Any
from enum import Enum

@dataclass
class Pos:
    x: int
    y: int



@dataclass
class Pin:
    """describes symbol pin: symbol type and pin name"""
    type: str
    name: str
    num: int


class PinType(Enum):
    IN = 1
    OUT = 2
    ANY = 9

@dataclass
class PinRef:
    """Identifies a pin on the specific symbol instance"""
    type: str # object type,
    id: str # object ID (scoped by the type)
    io_num: int # input/output pin number

    def to_str(self) -> str:

        out = self.type
        if self.id:
            out += ":" + self.id
        return out + "." + str(self.io_num)

    def __hash__(self):
        return hash((self.type, self.id, self.io_num))

    def __eq__(self, other: 'PinRef'):
        return self.id == other.id and self.type == other.type and self.io_num == other.io_num
@dataclass
class SymbolRef:
    """uniquely identifies symbol reference: symbol type and id"""
    type: str
    id: str

    def to_str(self)-> str:
        out = self.type
        if self.id:
            out += ":" + self.id
        return out

    def __hash__(self):
        return hash((self.type, self.id))



@dataclass
class Proto:
    type: str
    id: str
    inputs: List[Pin]
    outputs: List[Pin]
    attrs: Dict
    pos: Optional[Pos]

    def get_ref(self) -> SymbolRef:
        return SymbolRef(self.type, self.id)

@dataclass
class Symbol:
    type: str
    id: str
    inputs: List[Pin]
    outputs: List[Pin]
    attrs: Dict
    pos: Pos

@dataclass
class Transport:
    """Transport connects output of one symbol to an input of another"""
    input: PinRef
    output: PinRef
    gui: List[Pos]

@dataclass
class Header:
    is_input: bool
    type: str
    id: str
    name: str
    pos: Pos
    attrs: Dict




@dataclass
class Junction:
    type: str
    id: str
    pos: Pos

@dataclass
class Text:
    text: str
    pos: Pos

@dataclass
class Object:
    name: str
    h_inputs: List[Pin]
    h_outputs: List[Pin]
    attrs: Dict
    behavior: List[Symbol]
    texts: List[Text]
    net: List[Transport]
    inputs: List[Header]
    outputs: List[Header]
    junctions: List[Junction]

@dataclass
class Dataset:
    name: str
    args: List[str]
    context: int
    color: int
    tree: List[str]
    com: Optional[int]

@dataclass
class File:
    datasets: List[Dataset]
    objects: List[Object]
    prototypes: List[Proto]

@dataclass
class Assertion:
    expected: Any
    actual: Any

    expected_str: str
    actual_str: str
