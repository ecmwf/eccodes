# SPDX-FileCopyrightText: 2005- European Centre for Medium-Range Weather Forecasts (ECMWF)
# SPDX-License-Identifier: Apache-2.0

# Sphinx configuration, see https://www.sphinx-doc.org/en/master/usage/configuration.html

import datetime
import os
import re
import subprocess

# -- Run Doxygen -------------------------------------------------------------

# Doxygen XML for Breathe lands in _build/xml.
subprocess.check_call("doxygen Doxyfile", shell=True)

# -- Project information -----------------------------------------------------

project = "ecCodes"
author = "ECMWF"

year = datetime.datetime.now().year
copyright = f"2005-{year}, {author}"


def parse_version(ver_str):
    return re.sub("^((([0-9]+)\\.)+([0-9]+)).*", "\\1", ver_str)


here = os.path.abspath(os.path.dirname(__file__))

with open(os.path.join(here, "..", "VERSION"), "r") as f:
    release = f.readline().strip()  # full version string

version = parse_version(release)  # feature version

# -- General configuration ---------------------------------------------------

extensions = [
    "pydata_sphinx_theme",
    "breathe",
    "sphinx.ext.autosectionlabel",
    "sphinxcontrib.mermaid",
    "sphinx_copybutton",
    "sphinx_tabs.tabs",
]

templates_path = ["_templates"]
exclude_patterns = ["_build", ".venv", "Thumbs.db", ".DS_Store"]

source_suffix = ".rst"
master_doc = "index"
pygments_style = "sphinx"

highlight_language = "c"

autosectionlabel_prefix_document = True

# -- Options for HTML output -------------------------------------------------

html_theme = "pydata_sphinx_theme"
html_show_sourcelink = False
html_static_path = ["_static"]
html_sidebars = {
    "**": ["sidebar-nav-bs"],
}
html_theme_options = {
    "switcher": {
        "json_url": "https://sites.ecmwf.int/docs/dev-section/eccodes/versions.json",
        "version_match": release,
    },
    "show_toc_level": 2,
    "icon_links": [
        {
            "name": "GitHub",
            "url": "https://github.com/ecmwf/eccodes",
            "icon": "fa-brands fa-github",
        },
    ],
    "navbar_align": "left",
    "navbar_start": ["navbar-logo"],
    "navbar_center": ["navbar-nav"],
    "navbar_end": ["navbar-icon-links", "theme-switcher", "version-switcher"],
    "navbar_persistent": ["search-button"],
    "secondary_sidebar_items": ["page-toc"],
    # Local builds have no versions.json.
    "check_switcher": False,
    "logo": {"text": "ecCodes"},
    "footer_start": ["copyright"],
    "footer_center": ["sphinx-version"],
    "footer_end": ["theme-version"],
}

html_context = {
    # Follow the OS light/dark setting.
    "default_mode": "auto"
}

# -- Breathe configuration ---------------------------------------------------

breathe_projects = {"ECCODES": os.path.abspath("_build/xml")}
breathe_default_project = "ECCODES"
breathe_domain_by_extension = {"h": "c"}
