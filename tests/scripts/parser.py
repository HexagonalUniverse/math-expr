""" <tests/scripts/parser.py>

    Manages the parsing of the input test data files. """

import yaml
from dataclasses import dataclass
from os import path
from project_manager import ProjectManager


@dataclass(init=True, repr=True, eq=False, order=False, frozen=True, slots=True)
class TestCase(object):
    """Holds information about an individual test-case."""

    input: str | list[str] | None
    exp_output: str | None

    def compare_output(self, output: str) -> bool:
        if self.exp_output is None:
            return True
        return self.__normalize_output(self.exp_output) == self.__normalize_output(output)

    @staticmethod
    def __normalize_output(string: str):
        return string.strip()

    def __eq__(self, other: str):
        return self.compare_output(other)


@dataclass(repr=True, slots=True)
class ProgramTests(object):
    """Holds information about tests concerning an executable program."""

    __program_name: str
    __program_path: str
    test_cases: tuple[TestCase]

    def __init__(self, program_filepath: str, test_cases: list[TestCase] | tuple[TestCase]) -> None:
        if not isinstance(program_filepath, str):
            raise TypeError
        elif not isinstance(test_cases, list | tuple):
            raise TypeError

        self.__program_name: str = path.basename(program_filepath)
        self.__program_path: str = program_filepath
        self.test_cases: tuple[TestCase] = tuple(test_cases)

    @property
    def program_name(self) -> str:
        return self.__program_name

    @property
    def program_path(self) -> str:
        return self.__program_path

    @staticmethod
    def from_tests(program_filepath: str, tests: dict[str]):
        test_cases: list[TestCase] = []
        for test_case in tests:

            # Retrieving the input
            try:
                input_seq: str = test_case["input"]
            except KeyError:
                input_seq: None = None

            # Retrieving the output
            try:
                output_seq: str = test_case["output"]
            except KeyError:
                output_seq: None = None

            test_cases.append(TestCase(input_seq, output_seq))

        return ProgramTests(program_filepath, test_cases)


class TestParser(object):

    __slots__: list[str] = ["__p_manager"]

    __p_manager: ProjectManager

    def __init__(self):
        self.__p_manager: ProjectManager = ProjectManager()

    def executable_abspath(self, executable_filename: str) -> str:
        return path.abspath(
            path.join(
                self.__p_manager["exe_dir"], executable_filename
            )
        )

    def validate_executable(self, filename: str, executable_filepath: str) -> None:
        if not path.exists(executable_filepath):
            raise FileNotFoundError(f"{filename}: The executable \"{executable_filepath}\" couldn't be found.")
        return None

    def parse_program_tests(self, filename: str, test_data: dict[str, str | dict[str]]) -> ProgramTests | None:
        the_program: str = self.executable_abspath(test_data["program"])

        # Parsing and validating the program associated.
        try:
            self.validate_executable(filename, the_program)

        except FileNotFoundError as e:
            print(e)
            return None

        # Parsing and validating the test-cases.
        return ProgramTests.from_tests(the_program, test_data["tests"])

    def parse_test_datafile(self, filename: str) -> list[ProgramTests]:

        program_tests: list[ProgramTests] = list()
        with open(filename, "r") as datafile:
            data: dict = yaml.safe_load(datafile)
            # print("version:", data["version"])

            # print("test-executables:", data["test-executables"])
            for test_data in data["test-executables"]:
                program_test: ProgramTests | None = self.parse_program_tests(filename, test_data)

                if program_test is not None:
                    program_tests.append(program_test)

        return program_tests
