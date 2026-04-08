#!/usr/bin/env python3

import random

def gen_rand_data(bpv, length, pattern="zeros"):
    data = []
    values = []
    for i in range(length):
        value = 0
        if pattern == "zeros":
            value = 0
        elif pattern == "ones":
            value = 1
        elif pattern == "sequential":
            value = i % (1 << bpv)
        elif pattern == "random":
            value = random.randint(0, (1 << bpv) - 1)
        values.append(value)
        data.append(value)
    return values, data

def to_binary_string(values, bpv):
    binary = [format(value, f'0{bpv}b') for value in values]
    concat = ''.join(binary)
    # Fill the last byte with zeros if necessary
    while len(concat) % 8 != 0:
        concat += '0'
    bytes8 = [concat[i:i+8] for i in range(0, len(concat), 8)]
    cxx_bytes = ', '.join([f'0b{byte}' for byte in bytes8])
    return cxx_bytes

def to_values_string(values):
    values = [f"{value}ULL" for value in values]
    return values


def cxx_snippet(bpv, test_values, test_data):
    output = \
        "        {\n" + \
            "            {" + ', '.join(str(i) for i in to_values_string(test_values)) + "},\n" + \
            "            {\n" + \
            f"                {to_binary_string(test_data, bpv)}\n" + \
            "            }\n" + \
            "        },\n"
    return output

if __name__ == "__main__":
    all_output = ""

    for bpv in range(1, 65):
        output = "{" + \
            f"{bpv},\n" + \
            "    {\n"

        for length in [1, 8, 11]:
            test_values, test_data = gen_rand_data(bpv, length, "sequential")
            output += cxx_snippet(bpv, test_values, test_data)

        # if bpv in [15, 16, 23, 24, 25, 32]:
        if bpv in [23]:
            for length in [23]:
                test_values, test_data = gen_rand_data(bpv, length, "random")
                output += cxx_snippet(bpv, test_values, test_data)


        output += \
            "    }\n" + \
            "},\n"
        all_output += output


    with open("bit_packing_data.cc", "w") as f:
        f.write(all_output)
