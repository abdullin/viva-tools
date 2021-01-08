from dataclasses import dataclass
from typing import List, Tuple, Dict, Optional, Any, Union

from model import *

@dataclass
class Pin:
    """describes symbol pin: symbol type and pin name"""
    data_type: str
    name: str

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

@dataclass
class Proto:
    type: str
    id: str
    inputs: List[Pin]
    outputs: List[Pin]
    attrs: Dict
    pos: Optional[Pos]


@dataclass
class Transport:
    """Transport connects output of one symbol to an input of another"""
    left: PinRef
    right: PinRef
    gui: List[Pos]

@dataclass
class Assertion:
    expected: Any
    actual: Any

    expected_str: str
    actual_str: str
