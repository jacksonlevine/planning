import scipy

class MimosDonoApi:
    def setClientPosition(self, x: float, y: float, z: float) -> None: ...
    def getClientPosition(self) -> tuple: ...

mdma: MimosDonoApi