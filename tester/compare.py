import filenames


def compare(error1, error2, out_1, out_2, test, time_1, time_2 = "???", number = "???"):
    file_out = open(filenames.log, 'a+')
    # file_out.write("\n\nTest: " + test + '\n')

    # if len(error1) == 0 and len(error2) == 0:
    file_out.write('///////////////////////////////////////////////////////\n')
    if out_1 == out_2:
        print(f'{number} Test - OK')
        file_out.write(f'{number} OK, time_1: {time_1}, time_2: {time_2}')
    else:
        print('{number} Here it is')
        file_out.write("\n\nTest: " + test + '\n')
        file_out.write(filenames.exe_1 + '\n' + out_1 + '\n' +
                        filenames.exe_2 + '\n' + out_2 + '\n' + 'DIFFERENT ANSWERS')
        file_out.write('\n')
    # else:
    #     print(f'{number} Test - ERROR')
    #     file_out.write("\n\nTest: " + test + '\n')
    #     if len(error1) > 0:
    #         file_out.write('ERROR ' + filenames.exe_1 + ' ' + error1 + '\n')
    #         file_out.write(out_2 + '\n')
    #     if len(error2) > 0:
    #         file_out.write(out_2 + '\n')
