""" <tests/scripts/project_manager.py

    (...) """
from os import path
from pathlib import Path


class ProjectManager(object):
    """Manages the paths and interactions within the project."""

    __slots__: list[str] = ["rpath_project_dir", "a_paths"]

    # Tracks address, from <tests/scripts/>, where the test data is located at, to the project's main dir.
    rpath_project_dir: str

    a_paths: dict[str, str]

    def __init__(self) -> None:
        # From <Project/tests/>.
        self.rpath_project_dir: str = "../"

        # initializing it as relative paths;
        self.a_paths: dict[str, str] = {
            "exe_dir": "bin/exe/",
        }

        # calculating the absolute paths.
        for key in self.a_paths:
            self.a_paths[key] = path.abspath(
                path.join(self.rpath_project_dir, self.a_paths[key])
            )

    def __getitem__(self, item: str):
        return self.a_paths[item]
