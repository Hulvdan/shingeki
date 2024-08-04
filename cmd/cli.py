"""cli для работы с проектом.

Как использовать:

(из корня проекта)

- poetry run python cmd\cli.py generate
- poetry run python cmd\cli.py cmake_vs_files
- poetry run python cmd\cli.py build
- poetry run python cmd\cli.py run
- poetry run python cmd\cli.py stoopid_windows_visual_studio_run
- poetry run python cmd\cli.py format
- poetry run python cmd\cli.py format file1 file2 ...
- poetry run python cmd\cli.py lint
"""

import glob
import logging
import os
import re
import shutil
import subprocess
import sys
import tempfile
from contextlib import contextmanager
from functools import wraps
from pathlib import Path
from time import time
from typing import Any, NoReturn, TypeVar

import fnvhash
import pyjson5 as json
import sdl2
import typer
from OpenGL.GL import shaders
from PIL import Image

T = TypeVar("T")

global_timing_manager_instance = None


# При вызове exit отображаем затраченное время.
old_exit = exit  # noqa


def timed_exit(code: int) -> NoReturn:
    global global_timing_manager_instance
    if global_timing_manager_instance is not None:
        global_timing_manager_instance.__exit__(None, None, None)  # noqa
        console_handler.flush()

    old_exit(code)


def hook_exit():
    global exit  # noqa
    exit = timed_exit  # noqa


app = typer.Typer(callback=hook_exit, result_callback=timed_exit)


# ======================================== #
#                Constants                 #
# ======================================== #
LOG_FILE_POSITION = False

PROJECT_DIR = Path(__file__).parent.parent
CMD_DIR = Path("cmd")
SOURCES_DIR = Path("sources")
CMAKE_DEBUG_GAME_BUILD_DIR = Path(".cmake") / "vs17" / "game" / "Debug"
CMAKE_DEBUG_TESTS_BUILD_DIR = Path(".cmake") / "vs17" / "tests" / "Debug"

CLANG_FORMAT_PATH = "C:/Program Files/LLVM/bin/clang-format.exe"
CLANG_TIDY_PATH = "C:/Program Files/LLVM/bin/clang-tidy.exe"
FLATC_PATH = CMD_DIR / "flatc.exe"
MSBUILD_PATH = r"c:\Program Files\Microsoft Visual Studio\2022\Community\MSBuild\Current\Bin\amd64\MSBuild.exe"


REPLACING_SPACES_PATTERN = re.compile("\s+")
SHADERS_ERROR_PATTERN = re.compile(r"\d+\((\d+)\) : error (.*)")


# ======================================== #
#                 Logging                  #
# ======================================== #
class CustomLoggingFormatter(logging.Formatter):
    _grey = "\x1b[30;20m"
    _green = "\x1b[32;20m"
    _yellow = "\x1b[33;20m"
    _red = "\x1b[31;20m"
    _bold_red = "\x1b[31;1m"

    @staticmethod
    def _get_format(color: str | None) -> str:
        reset = "\x1b[0m"
        if color is None:
            color = reset

        suffix = ""
        if LOG_FILE_POSITION:
            suffix = " (%(filename)s:%(lineno)d)"

        return f"{color}[%(levelname)s] %(message)s{suffix}{reset}"

    _FORMATS = {
        logging.NOTSET: _get_format(None),
        logging.DEBUG: _get_format(None),
        logging.INFO: _get_format(_green),
        logging.WARNING: _get_format(_yellow),
        logging.ERROR: _get_format(_red),
        logging.CRITICAL: _get_format(_bold_red),
    }

    def format(self, record):
        log_fmt = self._FORMATS.get(record.levelno)
        formatter = logging.Formatter(log_fmt)
        return formatter.format(record)


log = logging.getLogger(__file__)
log.setLevel(logging.DEBUG)
console_handler = logging.StreamHandler()
console_handler.setLevel(logging.DEBUG)
console_handler.setFormatter(CustomLoggingFormatter())
log.addHandler(console_handler)


# ======================================== #
#            Library Functions             #
# ======================================== #
def better_json_dump(data, path):
    with open(path, "wb") as out_file:
        json.dump(data, out_file, indent="\t", ensure_ascii=False)  # noqa


def replace_double_spaces(string: str) -> str:
    return re.sub(REPLACING_SPACES_PATTERN, " ", string)


def run_command(cmd: list[str] | str) -> None:
    if isinstance(cmd, str):
        cmd = replace_double_spaces(cmd.replace("\n", " ").strip())

    log.debug(f"Executing command: {cmd}")

    p = subprocess.run(
        cmd,
        shell=True,
        stdout=sys.stdout,
        stderr=sys.stderr,
        text=True,
        encoding="utf-8",
    )

    if p.returncode:
        log.critical(f"Failed to execute: {cmd}")
        exit(p.returncode)


def hash32(value: str) -> int:
    return fnvhash.fnv1a_32(value.encode(encoding="ascii"))


def assert_contains(value: T, container) -> T:
    if value not in container:
        log.critical("value '{}' not found inside '{}'".format(value, container))
        exit(1)

    return value


# ======================================== #
#  Всякая хрень для индивидуальных задач   #
# ======================================== #
def listfiles_with_hashes_in_dir(path: str | Path) -> dict[str, int]:
    files = glob.glob(str(Path(path) / "**" / "*"), recursive=True, include_hidden=True)

    res = {}

    for file in files:
        with open(file, "rb") as in_file:
            res[str(Path(file).relative_to(path))] = hash(in_file.read())

    return res


# ========================================
# Индивидуальные задачи
# ========================================
def do_build() -> None:
    do_build_game()
    do_build_tests()


def do_build_game() -> None:
    run_command(
        rf'"{MSBUILD_PATH}" .cmake\vs17\game.sln -v:minimal -property:WarningLevel=3 -t:game'
    )


def do_build_tests() -> None:
    run_command(
        rf'"{MSBUILD_PATH}" .cmake\vs17\game.sln -v:minimal -property:WarningLevel=3 -t:tests'
    )


def do_run() -> None:
    run_command(str(CMAKE_DEBUG_GAME_BUILD_DIR / "game.exe"))


def do_test() -> None:
    run_command(str(CMAKE_DEBUG_TESTS_BUILD_DIR / "tests.exe"))


def do_format(specific_files: list[str]) -> None:
    if specific_files:
        run_command([CLANG_FORMAT_PATH, "-i", *specific_files])
        return

    glob_patterns = [
        Path(SOURCES_DIR) / "**" / "*.cpp",
        Path(SOURCES_DIR) / "**" / "*.h",
    ]

    files_to_format = []
    for pattern in glob_patterns:
        files_to_format.extend(
            glob.glob(str(pattern), recursive=True, include_hidden=True)
        )

    run_command([CLANG_FORMAT_PATH, "-i", *files_to_format])


def do_lint() -> None:
    # NOTE: Создание .clang-tidy файла, чтобы линтинг не ругался на внутренности raylib.
    file_path = Path(".cmake") / ".clang-tidy"
    if not file_path.exists():
        with open(file_path, "w") as out_file:
            out_file.write("Checks: '-*'")

    run_command(
        rf"""
            "{CLANG_TIDY_PATH}"
            --use-color
            src\raylib_game.cpp
        """
        # Убираем абсолютный путь к проекту из выдачи линтинга.
        # Тут куча экранирования происходит, поэтому нужно дублировать обратные слеши.
        + r'| sed "s/{}//"'.format(
            str(PROJECT_DIR).replace(os.path.sep, os.path.sep * 3) + os.path.sep * 3
        )
    )


def do_cmake_vs_files() -> None:
    run_command(
        r"""
            cmake
            -S .
            -B .cmake\vs17
        """
        # -DCMAKE_UNITY_BUILD=ON
        # -DCMAKE_UNITY_BUILD_BATCH_SIZE=0
    )


def do_cmake_ninja_files() -> None:
    run_command(
        r"""
            cmake
            -G Ninja
            -B .cmake\ninja
            -D CMAKE_CXX_COMPILER=cl
            -D CMAKE_C_COMPILER=cl
            -DCMAKE_UNITY_BUILD=ON
            -DCMAKE_UNITY_BUILD_BATCH_SIZE=0
            --log-level=ERROR
        """
    )


def do_compile_commands_json() -> None:
    run_command(
        r"""
            ninja
            -C .cmake\ninja
            -f build.ninja
            -t compdb
            > compile_commands.json
        """
    )


def do_stop_vs_ahk() -> None:
    run_command(r".nvim-personal\stop_vs.ahk")


def do_run_vs_ahk() -> None:
    run_command(r".nvim-personal\launch_vs.ahk")


def do_generate():
    with open(Path("src") / "assets" / "unnamed_mesh1.vox") as in_file:
        data = json.loads(in_file.read())

    def int_to_triple_int(value: int) -> tuple[int, int, int]:
        return (
            (value & 0xFF0000) // 0x10000,
            (value & 0xFF00) // 0x100,
            value & 0xFF,
        )

    colors = [int_to_triple_int(i) for i in data["palette"]]
    voxels = []

    for layer in data["layers"]:
        voxels.extend(layer["voxels"])

    with open(
        Path("src") / "resources" / "screens" / "gameplay" / "level.txt", "w"
    ) as out_file:
        out_file.write(str(len(colors)))
        out_file.write("\n")
        for color in colors:
            out_file.write("{} {} {}\n".format(color[0], color[1], color[2]))

        out_file.write(str(len(voxels)))
        out_file.write("\n")
        for voxel in voxels:
            out_file.write("{} {} {} {}\n".format(*voxel))


# ========================================
# CLI Commands
# ========================================
def timing(f):
    @wraps(f)
    def wrap(*args, **kw):
        started_at = time()
        result = f(*args, **kw)
        elapsed = time() - started_at
        log.info("Running '{}' took: {:.2f} sec".format(f.__name__, elapsed))
        return result

    return wrap


@app.command("cmake_vs_files")
@timing
def action_cmake_vs_files():
    do_cmake_vs_files()


@app.command("build_game")
def action_build_game():
    do_cmake_vs_files()
    do_generate()
    do_build_game()


@app.command("run")
def action_run():
    do_cmake_vs_files()
    do_generate()
    do_build_game()

    do_run()


@app.command("stoopid_windows_visual_studio_run")
def action_stoopid_windows_visual_studio_run():
    do_stop_vs_ahk()

    do_generate()
    do_build_game()

    do_run_vs_ahk()


@app.command("test")
def action_test():
    do_generate()
    do_build_tests()
    do_test()


@app.command("format")
def action_format(filepaths: list[str] = typer.Argument(default=None)):
    do_cmake_ninja_files()
    do_compile_commands_json()
    do_format(filepaths)


@app.command("lint")
def action_lint():
    do_cmake_ninja_files()
    do_compile_commands_json()
    do_lint()


@app.command("generate")
def action_generate():
    do_generate()


# ========================================
# Main
# ========================================
@contextmanager
def timing_manager():
    started_at = time()
    yield
    log.info("Running took: {:.2f} sec".format(time() - started_at))
    console_handler.flush()


def main() -> None:
    test_value = hash32("test")
    assert test_value == 0xAFD071E5, test_value

    # Всегда исполняем файл относительно корня проекта.
    os.chdir(PROJECT_DIR)

    caught_exc = None

    # При выпадении exception-а отображаем затраченное время.
    global global_timing_manager_instance
    global_timing_manager_instance = timing_manager()

    with global_timing_manager_instance:
        try:
            app()
        except Exception as e:
            caught_exc = e

    if caught_exc is not None:
        raise caught_exc


if __name__ == "__main__":
    main()
