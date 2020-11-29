import generator
import tester


if __name__ == "__main__":
    tests_number = 1000
    commands_cnt = 1

    init_text = str(commands_cnt) + "\n"

    for i in range(tests_number):
        test = generator.generate(commands_cnt)
        test = init_text + test
        # print(test)
        tester.test1(test, i)
