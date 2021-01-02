

class Names:
    def __init__(self):
        self.__map__ = {}

        self.old2new = {}
        self.new2old = {}



    def add_unique(self, name):
        if name in self.__map__:
            raise ValueError("Name already exists")

        uid = len(self.__map__)
        self.__map__[name]=uid
        return uid

    def add_rename(self, old, name, start_from = 0):
        for i in range(start_from, 100000):
            n = name if i==0 else f'{name}_{i}'
            if n in self.new2old:
                continue

            self.new2old[n] = old
            return n





    def add_input(self, name: str, unique: str):
        old = f'Input:{unique or ""}-0'
        new = f'i_{name}'
        return self.add_rename(old, new)


    def add_output(self, name: str, unique: str):
        old = f'Output:{unique or ""}-0'
        new = f'o_{name}'
        return self.add_rename(old, new)

    def add_junction(self, unique: str):
        old = f'Junction:{unique or ""}'
        new = f'j'
        return self.add_rename(old, new)
