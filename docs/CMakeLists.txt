include(UseCombinedDocs)

# Set the documentation directory
set(DOCS_DIR ${PROJECT_SOURCE_DIR}/docs)

# Create Build Configurations for Doxygen and Sphinx with specified paths
create_combined_doc(
        SOURCE_DIR
        ${DOCS_DIR}
        BUILD_DIR
        ${DOCS_DIR}/output
        DOXY_CONF_FILE
        ${DOCS_DIR}/doxygen.conf
        SPHINX_CONF_FILE
        ${DOCS_DIR}/sphinx.conf
        SPHINX_INDEX_FILE
        ${DOCS_DIR}/index.rst
        SPHINX_THEME
        "sphinx_rtd_theme"
        TARGET_NAME
        Combined_Docs
        COMMENT
        "HTML documentation"
)