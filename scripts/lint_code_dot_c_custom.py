import os
import re

# Define the expected section headers as regex patterns
expected_sections = [
    r"/\*{78}",  # Section header delimiter with exactly 78 asterisks
    r"\bIMPORTS\b",  # IMPORTS section
    r"\bPRIVATE DECLARATIONS & DEFINITIONS\b",  # Private declarations section
    r"\bMODULARITY BOILERCODE\b",  # Modularity section
    r"\bINIT BOILERCODE\b",  # Init boilerplate section
    r"\bPUBLIC API\b",  # Public API section
    r"\bPRIVATE API\b",  # Private API section
]

# Directory containing the .c files
src_dir = "src"


def check_file_layout(file_path):
    with open(file_path, "r") as file:
        content = file.read()

    # Check each expected section header
    for i, pattern in enumerate(expected_sections):
        if not re.search(pattern, content):
            raise ValueError(f"File {file_path} is missing the section: {pattern}")


def check_src_directory_layout(src_dir):
    # Check all .c files in the src directory
    for root, _, files in os.walk(src_dir):
        for file in files:
            if file.endswith(".c"):
                file_path = os.path.join(root, file)
                try:
                    check_file_layout(file_path)
                    print(f"{file} contains all required sections.")
                except ValueError as e:
                    print(e)


# Run the layout check for all .c files in src directory
if __name__ == "__main__":
    check_src_directory_layout(src_dir)
