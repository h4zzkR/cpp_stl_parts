import re
import math

def solve(test_args):
    out = []
    lines = re.split('\n', test_args)
    for test in lines[1:]:
        for parts in test.split('\n'):
            parts = [i for i in parts.split(' ') if len(i) != 0]
            try:
                a = int(parts[0]); c = parts[1]; b = int(parts[2])
            except IndexError:
                pass
            if (c == "+"):
                out.append(str(a + b) + "\n")
            elif (c == "-"):
                out.append(str(a - b) + "\n")
            elif (c == "*"):
                out.append(str(a * b) + "\n")
            elif (c == "/"):
                if a * b < 0:
                    out.append(str(a // b + 1) + "\n")
                else:
                    out.append(str(a // b) + "\n")
                # else:
                #     out.append(str(math.floor(a / b)) + "\n")
            elif (c == "%"):
                d = 0
                if a * b < 0:
                    d = a // b + 1
                else:
                    d = a // b
                rem = a - (d * b)
                out.append(str(rem) + "\n")
            elif (c == ">"):
                out.append(str(int(a > b)) + "\n")
            elif (c == "<"):
                out.append(str(int(a < b)) + "\n")
            elif (c == ">="):
                out.append(str(int(a >= b)) + "\n")
            elif (c == "<="):
                out.append(str(int(a <= b)) + "\n")
            elif (c == "=="):
                out.append(str(int(a == b)) + "\n")
            elif (c == "!="):
                out.append(str(int(a != b)) + "\n")
    return out[0]