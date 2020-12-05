from dataclasses import dataclass
from typing import List, Tuple, Dict, Optional, Any


@dataclass
class Proto:
    name: str
    inputs: List
    outputs: List
    attrs: Dict
    gui: List[Tuple[int,int]]

@dataclass
class Transport:
    input: str
    output: str
    gui: List[Tuple[int,int]]
@dataclass
class Object:
    proto: Proto
    behavior: List[Proto]
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
