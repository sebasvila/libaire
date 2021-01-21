import subprocess, os

# Configuration file for the Sphinx documentation builder.
#
# This file only contains a selection of the most common options. For a full
# list see the documentation:
# https://www.sphinx-doc.org/en/master/usage/configuration.html


# -- Check if this is an RDT build
read_the_docs_build = os.environ.get('READTHEDOCS', None) == 'True'


# -- Path setup --------------------------------------------------------------

# If extensions (or modules to document with autodoc) are in another directory,
# add these directories to sys.path here. If the directory is relative to the
# documentation root, use os.path.abspath to make it absolute, like shown here.
#
# import os
# import sys
# sys.path.insert(0, os.path.abspath('.'))


# -- Project information -----------------------------------------------------

project = 'libaire'
copyright = '2020, Sebas Vila, Pau Lafoz, Paco del Aguila and others'
author = 'Sebas Vila, Pau Lafoz, Paco del Aguila and others'

# The full version, including alpha/beta/rc tags
release = '1.0'


# -- General configuration ---------------------------------------------------

# Add any Sphinx extension module names here, as strings. They can be
# extensions coming with Sphinx (named 'sphinx.ext.*') or your custom
# ones.
extensions = [
    'breathe',
    'sphinx.ext.mathjax',
    'sphinxcontrib.plantuml',
]

# Add any paths that contain templates here, relative to this directory.
templates_path = ['_templates']

# List of patterns, relative to source directory, that match files and
# directories to ignore when looking for source files.
# This pattern also affects html_static_path and html_extra_path.
exclude_patterns = []


# -- Options for HTML output -------------------------------------------------

# The theme to use for HTML and HTML Help pages.  See the documentation for
# a list of builtin themes.
#
if read_the_docs_build:
    import sphinx_rtd_theme
    html_theme = 'sphinx_rdt_theme',
    extensions.append(html_theme)
else:
    html_theme = 'alabaster'

# Add any paths that contain custom static files (such as style sheets) here,
# relative to this directory. They are copied after the builtin static files,
# so a file named "default.css" will overwrite the builtin "default.css".
html_static_path = ['_static']

# enforce master_doc to cope with old versions
master_doc = 'index'

# config breathe plugin 
breathe_projects = { "libaire": "../doxyxml/" }
breathe_default_project = "libaire"
breathe_domain_by_extension = {
        "h" : "c",
        "c" : "c",
        }

# specific for RDT builds
if read_the_docs_build:

    # execute doxygen on RDT server
    subprocess.call('cd ..; doxygen doxygen.conf', shell=True)

    # define plantuml on RDT server
    plantuml = 'java -Djava.awt.headless=true -jar /usr/share/plantuml/plantuml.jar'
    # if you need a local plantuml java executable
    #plantuml = 'java -Djava.awt.headless=true -jar {}'.format(local_plantuml_path)

    # config RTD theme
    html_theme_options = {
        'analytics_id': 'UA-XXXXXXX-1',  #  Provided by Google in your dashboard
        'analytics_anonymize_ip': False,
        'logo_only': False,
        'display_version': True,
        'prev_next_buttons_location': 'bottom',
        'style_external_links': False,
        'vcs_pageview_mode': '',
        'style_nav_header_background': 'white',
        # Toc options
        'collapse_navigation': True,
        'sticky_navigation': True,
        'navigation_depth': 4,
        'includehidden': True,
        'titles_only': False
    }

    
