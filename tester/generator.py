
from random import randint
from random import choice
import sys


def generate(command_num):
    commands = {1: "+", 2 : "-", 3 : "*", 4 : "/", 5 : '>', 6:'<', 7:">=", 8:"<=", 9:"==", 10:"!=", 11 : "%"}
    # command_num = 12
    text = ""
    num = 0
    left = -1000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000
    right = 1000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000

    # command = commands[1]
    # num = randint(100, 50000)
    # text = f"{command} {str(num)}\n"
    # arrsize = 1
    # l = r = 0

    for _ in range(command_num):
        command = commands[randint(1,11)]
        if command == "/" or command == "%":
            a = randint(left, right)
            # a = randint(-100000000000000000000, 100000000000000000000)
            # b = randint(1, 100000)
            b = randint(left, right)
        else:
            a = randint(left, right)
            b = randint(left, right)

        text += str(a) + " " + command + " " + str(b) + '\n'
    return (text)


def generate_sequence(n, left, right):
    array = []
    for _ in range(n):
        array.append(randint(left, right))
    return array
