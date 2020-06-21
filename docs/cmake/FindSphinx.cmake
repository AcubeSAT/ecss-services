#Look for an executable called sphinx-build
find_program(SPHINX_EXECUTABLE
        NAMES sphinx-build
        HINTS $ENV{SPHINX_DIR}
        HINTS ${SPHINX_ROOT}/bin
        DOC "Path to sphinx-build executable"
        )

include(FindPackageHandleStandardArgs)

find_package_handle_standard_args(
        Sphinx DEFAULT_MSG
        SPHINX_EXECUTABLE
)

mark_as_advanced(
        SPHINX_EXECUTABLE
)