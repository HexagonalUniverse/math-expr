""" <tests/test.py>

    A script for running all the tests in the test-data yaml files.
"""

from parser import *
from runner import *


def __main():
    parser = TestParser()
    z = parser.parse_test_datafile("data/exp-unit.yaml")

    runner = TestRunner()
    runner(z)


if __name__ == "__main__":
    __main()
