# `catr` - cat with range

`catr` is a command-line utility similar to the Unix `cat` command but with enhanced functionality. It allows users to
extract specific portions of text from a file by defining start and either end or length, offering extra flexibility and
precision in text extraction.

`catr` is licensed under GLWTS. For more information, please refer to the LICENSE file.

## Installation Guide for `catr`

Follow these steps to install the `catr` command-line tool on your system. This guide assumes you have `gcc` installed
and are using a Unix-like operating system.

### Prerequisites

- Ensure you have `gcc` installed on your system to compile the source code. You can check this by
  running `gcc --version` in your terminal.
- This script is designed to work with `zsh`. If you are using a different shell, you might need to adjust the script or
  the instructions accordingly.

### Installation Steps

1. **Clone the repository or download the source code:**
   If you have `git` installed, you can clone the repository using the following command:

```bash
git clone https://github.com/Andree37/catr.git
```

Alternatively, download the source code to your local machine.

2. **Run the installation script:**

Execute the installation script with the following command:

```bash
./install.sh
```

This script will compile the `catr` program, create a directory at `$HOME/bin` (if it doesn't already exist), and place
the `catr` executable there. It will also ensure that the `$HOME/bin` directory is added to your PATH by updating
the `.zshrc` file.

If you encounter any permissions errors while running the script, you might need to grant execution permissions to the
script:

```bash
chmod +x install.sh
```

and then rerun the script.

3. **Restart your terminal or source your profile:**

For the changes to take effect, either restart your terminal or source your profile with the following command:

```bash
source ~/.zshrc
```

If you are using a different shell, source the corresponding configuration file for your shell.

4. **Verify the installation:**
   Once the installation is complete and your terminal is restarted (or your profile is sourced), you can verify the
   installation of `catr` by running:

```bash
catr --help
```

This command should display the usage information for `catr`.

Congratulations! You have successfully installed `catr` on your system.

## Comparing `catr` with `cat`

While both `catr` and `cat` can display text from files, `catr` provides additional capabilities for more specific text
extraction without the need for combining multiple commands.

| Task                                                    | `catr` Usage              | `cat` Equivalent Usage                                 |
|---------------------------------------------------------|---------------------------|--------------------------------------------------------|
| Extract 100 characters range starting from character 50 | `catr file.txt 50 100`    | `cat file.txt \| tail -c +50 \| head -c 101`           |
| Extract characters range 50 to 100                      | `catr file.txt -r 50 100` | `cat file.txt \| head -c 100 \| tail -c +50`           |
| Extract lines range 3 to 5                              | `catr file.txt -rl 3 5`   | `cat test_input/lorem.txt \| head -n 5 \| tail -n +3'` |
| Extract 5 lines starting from line 3                    | `catr file.txt -l 3 5`    | `cat test_input/lorem.txt \| tail -n +5 \| head -n 3`  |

By providing a straightforward syntax for these tasks, `catr` enhances the usability and readability of command-line
text processing.
