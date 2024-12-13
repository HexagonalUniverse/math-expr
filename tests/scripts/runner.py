""" <tests/scripts/runner.py>

    Manages the execution of tests. """

import subprocess
from parser import TestCase, ProgramTests
from dataclasses import dataclass


@dataclass(init=True, frozen=True, repr=True, eq=True, slots=True, order=False)
class TestCaseStats(object):
    """Test-case run output data structure."""
    success: bool
    return_code: int
    runtime: float
    stdout: str

    @property
    def process_success(self) -> bool:
        return self.return_code == 0


@dataclass(init=True, frozen=True, repr=False, eq=True, slots=True, order=False)
class ProgramTestStats(object):
    """(...)"""

    program_input: ProgramTests
    tc_stats: list[TestCaseStats]

    def __repr__(self) -> str:
        successes: int = sum(map(lambda tcs: 1 if tcs.success else 0, self.tc_stats))
        if successes != len(self.tc_stats):
            color: str = "\033[0;31m"
        else:
            color: str = "\033[0;32m"

        header: str = f"{self.program_input.program_name} | {color}{successes} / {len(self.tc_stats)}\033[0m\n"

        body: str = ""
        for i, tcs in enumerate(self.tc_stats):
            time_took: str = f"Took {tcs.runtime * 1000:03.2f}\t[ms]"
            process_success: str = "" if tcs.process_success else f"(Exit-code: {tcs.return_code})"

            color: str = ""
            if not tcs.success:
                color: str = "\033[0;31m"

            enumeration: str = color + f"\t#{i + 1:02d}: " + "\033[0m"

            test_case_line: str = enumeration + time_took + "\t" + process_success + "\n"

            if not tcs.success:
                diff: str = f"\t\t- For \"{self.program_input.test_cases[i].input}\" expected \"{self.program_input.test_cases[i].exp_output}\", but got \"{tcs.stdout}\"\n"
            else:
                diff: str = ""

            body += test_case_line + diff

        return header + body


class TestRunner(object):

    tc_stats: list[list[TestCaseStats]]

    def __init__(self) -> None:
        self.tc_stats: list[list[TestCaseStats]] = list()

    @staticmethod
    def __run_test_case(program_filepath: str, test_case: TestCase) -> TestCaseStats:
        """Attempts running a single instance. Returns the program's subprocess stdout output."""
        import subprocess
        from time import time

        start_time: float = time()

        # subprocess.run parameters
        timeout: float = 1
        text: bool = True
        shell: bool = False
        stdout: int = subprocess.PIPE

        # If the input is None, then makes it an empty list for proper treatment below.
        input_seq: str | list[str] | None = test_case.input
        if input_seq is None:
            input_seq: list[str] = list()

        process_result: subprocess.CompletedProcess
        if isinstance(input_seq, list):
            # In case the input-sequence is a list, passes it as arguments to the program.
            input_seq: list[str]
            process_result = subprocess.run(
                [program_filepath, * input_seq], stdout=stdout,
                text=text, timeout=timeout, shell=shell
            )

        elif isinstance(input_seq, str):
            # Otherwise, if it is a string, feed in by regular input.
            input_seq: str
            process_result = subprocess.run(
                [program_filepath], input=input_seq, stdout=stdout,
                text=text, timeout=timeout, shell=shell,
            )
        else:
            raise ValueError

        end_time: float = time()

        # print(f"Runned {program_filepath}: {process_result.stdout} [{type(process_result.stdout)}")

        return TestCaseStats(
            test_case.compare_output(process_result.stdout),
            process_result.returncode, end_time - start_time, process_result.stdout
        )

    def run_program_test(self, p_test: ProgramTests) -> ProgramTestStats:
        """Attempts running the given test-data, returning the occurrences of success."""

        tc_stats: list[TestCaseStats] = list()
        for test_case in p_test.test_cases:
            out: TestCaseStats = self.__run_test_case(p_test.program_path, test_case)
            tc_stats.append(out)

        return ProgramTestStats(p_test, tc_stats)

    def __call__(self, tests: list[ProgramTests] | tuple[ProgramTests]):
        if not isinstance(tests, list | tuple):
            raise TypeError("not a list")

        self.tc_stats.clear()

        # validating the list content.
        for test in tests:
            if not isinstance(test, ProgramTests):
                raise TypeError("not program-test", type(test))

        for test in tests:
            print("result:", self.run_program_test(test))


def run_test(executable, test_case):
    input_data = test_case['input']
    expected_output = test_case['expectedOutput']
    result = subprocess.run([executable], input=input_data, capture_output=True, text=True)
    assert result.stdout.strip() == expected_output


