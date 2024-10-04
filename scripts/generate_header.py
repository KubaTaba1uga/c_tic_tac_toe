import os
import sys


def generate_header(c_file_path):
    # Extract the base name of the C file to create a corresponding header file name
    base_name = os.path.splitext(os.path.basename(c_file_path))[0]
    header_filename = base_name + ".h"

    header_template = f"""#ifndef {base_name.upper()}_H
#define {base_name.upper()}_H
/*******************************************************************************
 * @file {header_filename}
 * @brief TO-DO
 *
 * TO-DO
 *
 ******************************************************************************/

/*******************************************************************************
 *    IMPORTS
 ******************************************************************************/

/*******************************************************************************
 *    PRIVATE API
 ******************************************************************************/

/*******************************************************************************
 *    PUBLIC API
 ******************************************************************************/
struct {base_name.capitalize()}Ops {{
    
}};

/*******************************************************************************
 *    MODULARITY BOILERCODE
 ******************************************************************************/
struct {base_name.capitalize()}Ops *get_{base_name.lower()}_ops(void);

#endif // {base_name.upper()}_H
"""
    with open(header_filename, "w") as file:
        file.write(header_template)

    print(f"Header file '{header_filename}' has been generated.")


if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("Usage: python generate_header.py <path_to_c_file>")
        sys.exit(1)

    c_file_path = sys.argv[1]
    if not os.path.isfile(c_file_path):
        print(f"Error: The file '{c_file_path}' does not exist.")
        sys.exit(1)

    generate_header(c_file_path)
