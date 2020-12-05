from dataclasses import dataclass
from typing import List, Tuple, Dict, Optional, Any

@dataclass
class Pos:
    x: int
    y: int


@dataclass
class Proto:
    name: str
    inputs: List
    outputs: List
    attrs: Dict
    gui: Optional[Pos]

@dataclass
class Transport:
    input: str
    output: str
    gui: List[Pos]


@dataclass
class Text:
    text: str
    gui: Pos

@dataclass
class Object:
    proto: Proto
    behavior: List[Proto]
    texts: List[Text]
    net: List[Transport]

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
