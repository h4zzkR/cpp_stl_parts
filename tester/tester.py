import subprocess
import filenames
import compare
import solver
import time

def test1(test_args, number=0):

    first = subprocess.Popen([filenames.exe_1], stdin=subprocess.PIPE,
                             stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
    start_1 = time.monotonic()
    output1, errors1 = first.communicate(test_args)
    # (output2, errors2) = solver.solve(test_args)
    output2 = solver.solve(test_args)
    compare.compare(errors1, None, output1, output2, test_args, time.monotonic() - start_1, None, number)


def test(test_args, number):
    first = subprocess.Popen([filenames.exe_1], stdin=subprocess.PIPE,
                             stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
    start_1 = time.monotonic()
    output1, errors1 = first.communicate(test_args)
    second = subprocess.Popen([filenames.exe_2], stdin=subprocess.PIPE, # example
                              stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
    start_2 = time.monotonic()
    output2, errors2 = second.communicate(test_args)
    compare.compare(errors1, errors2, output1, output2, test_args,
                    time.monotonic() - start_1, time.monotonic() - start_2, number)
