"""This turns a .d file into a cmake dependency file.
It takes a single file .d file as input, and generates a .d.cmake.tmp
file as output with the same basename.
"""

import sys


def main():

    with open(sys.argv[1]) as content_file:
        content = str(content_file.read())
        content = content.split(": ", 1)[1]
        content = content.replace("\n", " ")
        files = content.split(" ")
        target_file_contents = "set(FILE_DEPS\n"
        for f in files:
            target_file_contents += f + "\n"
        target_file_contents += ")"

        with open(sys.argv[1] + ".cmake.tmp", "w+") as write_file:
            write_file.write(target_file_contents)


if __name__ == "__main__":
    sys.exit(main())
