/* -----------------------------------------------------------------------------
 * This file is part of SWIG, which is licensed as a whole under version 3 
 * (or any later version) of the GNU General Public License. Some additional
 * terms also apply to certain portions of SWIG. The full details of the SWIG
 * license and copyrights can be found in the LICENSE and COPYRIGHT files
 * included with the SWIG source code as distributed by the SWIG developers
 * and at http://www.swig.org/legal.html.
 *
 * rust.cxx
 *
 * Rust language module for SWIG.
 * ----------------------------------------------------------------------------- */

#include "swigmod.h"

class RUST: public Language
{
public:
    virtual void main(int argc, char *argv[]);
    virtual int top(Node *n);
protected:
    File *f_begin;
    File *f_runtime;
    File *f_header;
    File *f_wrappers;
    File *f_init;
};

static const char *RUST_USAGE = "\
Rust Options (available with -rust):\n\
    -help            - Print this help.\n\
";

void RUST::main(int argc, char *argv[])
{
    // Check arguments
    for (int i = 1; i < argc; ++i)
    {
        if (argv[i])
        {
            if (strcmp(argv[i], "-help") == 0)
            {
                Printf(stdout, "%s\n", RUST_USAGE);
            }
        }
    }

    // Configure SWIG
    SWIG_library_directory("rust");
    Preprocessor_define("SWIGRUST 1", 0);
    SWIG_config_file("rust.swg");
    SWIG_typemap_lang("rust");
}

int RUST::top(Node *n)
{
    String *module = Getattr(n, "name");
    String *outfile = Getattr(n, "outfile");

    // Initialize I/O
    f_begin = NewFile(outfile, "w", SWIG_output_files());
    if (!f_begin)
    {
        FileErrorDisplay(outfile);
        SWIG_exit(EXIT_FAILURE);
    }

    f_runtime = NewString("");
    f_init = NewString("");
    f_header = NewString("");
    f_wrappers = NewString("");

    // Register file targets with the SWIG file handler
    Swig_register_filebyname("begin", f_begin);
    Swig_register_filebyname("header", f_header);
    Swig_register_filebyname("wrapper", f_wrappers);
    Swig_register_filebyname("runtime", f_runtime);
    Swig_register_filebyname("init", f_init);

    // Output module initialization code
    Swig_banner(f_begin);

    // Emit code for children
    Language::top(n);

    // Write all to the file
    Dump(f_runtime, f_begin);
    Dump(f_header, f_begin);
    Dump(f_wrappers, f_begin);
    Wrapper_pretty_print(f_init, f_begin);

    // Cleanup files
    Delete(f_runtime);
    Delete(f_header);
    Delete(f_wrappers);
    Delete(f_init);
    Delete(f_begin);

    return SWIG_OK;
}

extern "C"
Language *swig_rust(void)
{
    return new RUST();
}
