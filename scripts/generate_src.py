import os
import sys


def generate_source(header_file_path):
    # Extract the base name of the header file to create a corresponding source file name
    base_name = os.path.splitext(os.path.basename(header_file_path))[0]
    source_filename = base_name + ".c"

    source_template = f"""/*******************************************************************************
 * @file {source_filename}
 * @brief TO-DO
 *
 * TO-DO
 *
 ******************************************************************************/

/*******************************************************************************
 *    IMPORTS
 ******************************************************************************/
// C standard library

// App's internal libs
#include "{base_name}.h"

/*******************************************************************************
 *    PRIVATE DECLARATIONS & DEFINITIONS
 ******************************************************************************/

/*******************************************************************************
 *    PUBLIC API
 ******************************************************************************/
struct {base_name.capitalize()}Ops {base_name}_ops = {{ NULL }};

struct {base_name.capitalize()}Ops *get_{base_name.lower()}_ops(void){{
    return &{base_name}_ops;
}};    
/*******************************************************************************
 *    PRIVATE API
 ******************************************************************************/

"""

    with open(source_filename, "w") as file:
        file.write(source_template)

    print(f"Source file '{source_filename}' has been generated.")


if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("Usage: python generate_source.py <path_to_header_file>")
        sys.exit(1)

    header_file_path = sys.argv[1]
    if not os.path.isfile(header_file_path):
        print(f"Error: The file '{header_file_path}' does not exist.")
        sys.exit(1)

    generate_source(header_file_path)
