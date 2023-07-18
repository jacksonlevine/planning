from mimos_api import mdma


def startup():
    x, y, z = mdma.getClientPosition()
    print(f"{x}, {y}, {z}")
