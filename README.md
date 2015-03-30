This can be used to enable/disable modules on different (multi) sites and environments (during automatic deploy for example).

### Dependencies:
- Drush
- php-yaml (php5-yaml on Ubuntu/Debian)

### Usage:
Set `$conf['env']` to settings.php or via `variable_set('env', 'anything');` (optional)

Run `drush dmodm --yaml-file /absolute/path/to/your/module-dependencies.yaml` to load with absolute path or `drush dmodm --yaml-file module-dependencies.yaml` to load file from the module directory.

### Examples
````yaml
all: # <- Global modules
  views: 1

local: # <- Environment

  all: # <- Modules that will be enabled on every (multi) site
    views_ui: 1

  multisite1: # <- Modules that will be enabled only on that site
    devel: 1
    context: 1

prod:

  all:
    devel: 0
    maillog: 0

  multisite1:
    context: 1
````
