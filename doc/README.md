# To generate doc in Debian

## Install packages

Install some needed extensions. Breathe adds the ability to get info
from a doxygen generated xml. Plantuml adds the possibility to use
planuml diagrams to the manual.

* python3-breathe (includes doxygen by dependencies)
* python3-sphinxcontrib.plantuml (includes plantuml processor by
  dependencies)

## Generate doc

Run `make html`

