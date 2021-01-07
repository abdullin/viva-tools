from dataclasses import dataclass
from typing import List, Tuple, Dict, Optional, Any, Union


@dataclass
class Pos:
    x: int
    y: int

@dataclass
class Pin:
    """describes symbol pin: symbol type and pin name"""
    data_type: str
    name: str
    num: int

@dataclass
class PinRef:
    """Identifies a pin on the specific symbol instance"""
    symbol_type: str # object type,
    id: str # object ID (scoped by the type)
    io_num: int # input/output pin number

@dataclass
class SymbolRef:
    """uniquely identifies symbol reference in a sheet: symbol type and id"""
    symbol_type: str
    id: str

    def to_str(self)-> str:
        out = self.symbol_type
        if self.id:
            out += ":" + self.id
        return out


@dataclass
class Proto:
    type: str
    id: str
    inputs: List[Pin]
    outputs: List[Pin]
    attrs: Dict
    pos: Optional[Pos]

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
    left: PinRef
    right: PinRef
    gui: List[Pos]
@dataclass
class Conn:
    left: Union['Header','Symbol','Junction']
    left_num: int
    right: Union['Header', 'Symbol', 'Junction']
    right_num: int
    gui: List[Pos]

@dataclass
class Header:
    """
       type: the data type of this pin
    """
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
class Sheet:
    name: str
    h_inputs: List[Pin]
    h_outputs: List[Pin]
    attrs: Dict
    behavior: List[Symbol]
    texts: List[Text]
    net: List[Conn]
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
    objects: List[Sheet]
    prototypes: List[Proto]

@dataclass
class Assertion:
    expected: Any
    actual: Any

    expected_str: str
    actual_str: str
