#!/usr/bin/env python3

import os
import re
from collections import defaultdict
import cfunits


RE = re.compile(r"^\d+ \d+ ([\-A-Za-z0-9]+) (.*)\((.*)\)$")

FIX = {
    "deg C": "degC",
    "degrees C": "degC",
    "Dobson": "DU",
    "kg m**-2/day": "kg m**-2 day**-1",
    "m**2 / m**2": "m**2 m**-2",
    "kg kg**-1 per day": "kg kg**-1 day**-1",
    "K per day": "K day**-1",
    "cm per day": "cm day**-1",
    "mm per day": "mm day**-1",
    "m s**-1 per day": "m s**-1 day**-1s",
    "degreeperday": "degC day**-1",
    "Joule": "J",
    "Jm-2": "J m**-2",
    "Nm**-3": "N m**-3",
    "Ws m**-2": "W s m**-2",
    "m s**-1 day**-1s": "m s**-1 day**-1 s",
    "m s**-1 deg C": "m s**-1 degC",
    "psuperday": "psu day**-1",
    "ms*-1": "m s**-1",
    "m s*-1": "m s**-1",
    "kg kg-1": "kg kg**-1",
    "kg/kg": "kg kg**-1",
    "kg2 kg-2": "kg**2 kg**-2",
    "m3 m-3": "m**3 m**-3",
    "m3/m3": "m**3 m**-3",
    "ms-1": "m s**-1",
    "cm/s": "cm s**-1",
    "mm6/m3": "mm**6 m**-3",
    "m**2 K s**-2": "K m**2 s**-2",
    "m s-**2": "m s**-2",
    # "M":"m",
    "M/S": "m s**-1",
    "bq m-2 s-1": "Bq m**-2 s**-1",
    "bq m-2": "Bq m**-2",
    "bq m-3": "Bq m**-3",
    "C": "degC",
    "J kg-1": "J kg**-1",
    "J kg-1 ": "J kg**-1",
    "J/kg": "J kg**-1",
    "m2 s-2": "m**2 s**-2",
    "m2/s2": "m**2 s**-2",
    " m2/s": "m**2 s**-1",
    "W/kg": "W kg**-1",
    "m2 s-3": "m**2 s**-3",
    "radians": "rad",
    "s-2": "s**-2",
    "K s-1": "K s**-1",
    "Deg C": "degC",
    " kg kg**-1 s**-1": "kg kg**-1 s**-1",
    "1/s": "s**-1",
    "kg kg-1 s-1": "kg kg**-1 s**-1",
    "kg/kg/s": "kg kg**-1 s**-1",
    "s-1": "s**-1",
    "K kg kg-1": "K kg kg**-1",
    "K2": "K**2",
    "N m-2": "N m**-2",
    " N m-2": "N m**-2",
    "N/m2": "N m**-2",
    "kg/m2/h": "kg m**-2 hour**-2",
    "mW/m2": "m W m**-2",
    "1/km": "km**-1",
    "mm s-1": "mm s**-1",
    "m2 2-1": "m**2 s**-1",
    "km2": "km**2",
    "10-7 s-2": "10**-7 s**-2",
    "0-1": "[0..1]",
    "0 - 1": "[0..1]",
    "0to1": "[0..1]",
    "(0 to 1)": "[0..1]",
    "(0-1)": "[0..1]",
    "(-1 to 1)": "[-1..1]",
    "-1 to 1": "[-1..1]",
    "min": "minute",
    "m3/s": "m**3 s**-1",
    "m3 kg-1 s-1": "m**3 kg**-1 s**-1",
    "m**2 s radian**-1": "m**2 s rad**-1",
    "m2/s": "m**2 s**-1",
    "m2 s-1": "m**2 s**-1",
    "m2 s**-1": "m**2 s**-1",
    "K m2 kg-1 s-1": "K m**2 kg**-1 s**-1",
    "K*m2 / kg / s": "K m**2 kg**-1 s**-1",
    "K*m2/kg/s": "K m**2 kg**-1 s**-1",
    "m2 kg-1 s-1": "m**2 kg**-1 s**-1",
    "kg m2": "kg m**2",
    " m2": "m**2",
    "m s-2": "m s**-2",
    "m/s": "m s**-1",
    "m s-1": "m s**-1",
    " m/s": "m s**-1",
    " m s-1": "m s**-1",
    "Pa m**2 s**-3": "m**2 Pa s**-3",
    "W m sr m-2": "W m sr m**-2",
    "kgm-2": "kg m**-2",
    "N m-2 ": "N m**-2",
    "d": "day",
    "W m-2": "W m**-2",
    "W m-2 ": "W m**-2",
    "W/m2": "W m**-2",
    "W m-2 K-1": "W m**-2 K**-1",
    " m": "m",
    " 1/m": "m**-1",
    "1/m": "m**-1",
    "m-1": "m**-1",
    "h": "hour",
    "kg-1": "kg**-1",
    "Bq kg-1": "Bq kg**-1",
    "1/kg2/s": "kg**-2 s**-1",
    "kg-2 s-1": "kg**-2 s**-1",
    "J m-2": "J m**-2",
    "kg s-2": "kg s**-2",
    "K m-1": "K m**-1",
    "K/m": "K m**-1",
    "kg m-1": "kg m**-1",
    "kg/m": "kg m**-1",
    "N m-2 s": "N m**-2 s",
    "Pa s-1": "Pa s**-1",
    "Pa/s": "Pa s**-1",
    "W/m3/sr": "W m**-3 sr**-1",
    " 1/m/sr": "m**-1 sr**-1",
    "1/m/sr": "m**-1 sr**-1",
    "s m-1": "s m**-1",
    "s/m": "s m**-1",
    "m-2": "m**-2",
    "kg m-2": "kg m**-2",
    "kg/m2": "kg m**-2",
    "kg m-2 s-1": "kg s**-1 m**-2",
    "kg/m2/s": "kg s**-1 m**-2",
    "m-2/s-2": "m**-2 s**2",
    "s2 m-2": "m**-2 s**2",
    "Bq s m-3": "Bq s m**-3",
    "m-3": "m**-3",
    "kg kg-1m-3": "kg kg**-1 m**-3",
    "kg m-3": "kg m**-3",
    "kg/m3": "kg m**-3",
    "Bq m-3": "Bq m**-3",
    "kg2 m-4": "kg**2 m**-4",
    "m kg-1 s-1": "m kg**-1 s**-1",
    "W/m/sr": "W m**-1 sr**-1",
    "W m-1 s-1": "W m**-1 s**-1",
    "klux": "klx",
    "m s-**2": "m s**-2",
    "W m-**3 sr**-1": "W m**-3 sr**-1",
    "m**2s**-2": "m**2 s**-2",
    "m2s-2": "m**2 s**-2",
    "w m**-2": "W m**-2",
    "w m-2": "W m**-2",
    "m-2 s-1": "m**-2 s**-1",
    "m-2s-1": "m**-2 s**-1",
    "s**-1 m**-2": "m**-2 s**-1",
    "s-1 m-2": "m**-2 s**-1",
    "K2 m-2 s-1": "K**2 m**-2 s**-1",
    "mol m-2 s-1": "mol m**-2 s**-1",
    "m Pa s**-2": "Pa m s**-2",
    "Pa s**-1 K": "K Pa s**-1",
    "Km kg-1 s-1": "K m kg**-1 s**-1",
    "hPa s-1": "hPa s**-1",
    "(10**-6 g) m**-3": "ug m**-3",
    "ug/m**3": "ug m**-3",
    "kg m-1 s-2": "kg m**-1 s**-2",
    "Bq m-2": "Bq m**-2",
    "Degree": "degree",
    "degrees": "degree",
    "m2 2**-2": "m**2 s**-2",
    "m**2/3 s**-1": "m**(2/3) s**-1",
    "m2/3 s-1": "m**(2/3) s**-1",
    "s-1 10-5": "10**-5 s**-1",
    "W s m**-2": "W m**-2 s",
    "mol m-2": "mol m**-2",
    "Deg true": "degree_true",
    "Deg. true": "degree_true",
    "deg. true": "degree_true",
    "Degree true": "degree_true",
    "degree true": "degree_true",
    "Gpm": "gpm",
    "Degree E": "degree_east",
    "deg E": "degree_east",
    "deg N": "degree_north",
    "deg": "degree",
    "m s-**2": "m s**-2",
    # C = CO2, S = SOX
    "kg C m**-2 s**-1": "kg m**-2 s**-1",
    "mg S/m**2": "mg m**-2",
}


class P(defaultdict):
    def __init__(self):
        super().__init__(set)


class Q(defaultdict):
    def __init__(self):
        super().__init__(P)


def F(u):
    u = FIX.get(u, u)
    # if not "(" in u:
    #     u = " ".join(sorted(u.split(" ")))
    return u


def C(u):
    x = cfunits.Units(u).formatted()
    if x is None:
        return "~~ /" + u + "/"
    return x


U = defaultdict(Q)

for root, _, files in os.walk("."):
    for file in files:
        full = os.path.join(root, file)

        if full.endswith(".table"):
            with open(full) as f:
                first = True
                for line in f.readlines():
                    line = line.strip()
                    if first:
                        if (
                            line.strip()
                            != "# This file was automatically generated by ./param.pl"
                        ):
                            break
                        # print(full)
                        first = False
                        continue
                    # print(line)
                    m = RE.match(line)
                    if m:
                        units = m.group(3)
                        param = m.group(2).strip()
                        if param.endswith("("):
                            param = param[:-1]
                            units = "(" + units
                        units = F(units)
                        U[C(units)][units][param].add(
                            full.replace("./definitions/", "")
                        )

        if full.endswith("units.def"):
            with open(full) as f:
                param = "?"
                units = "?"
                for line in f.readlines():
                    line = line.strip()
                    if line.startswith("#") and "file generated" not in line:
                        param = line[1:]

                    if line.endswith("= {"):
                        units = line[1:-5]
                        units = F(units)
                        U[C(units)][units][param].add(
                            full.replace("./definitions/", "")
                        )
                        param = "?"
                        units = "?"

for k, v in sorted(U.items()):
    print(k)
    for p, f in sorted(v.items()):
        print("   [{}]".format(p))
        for a, b in sorted(f.items()):
            # print('       [{}] {}'.format(a, ", ".join(sorted(b))))  # , list(f))
            print("       [{}]".format(a))

    print()
