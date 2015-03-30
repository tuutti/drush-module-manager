This can be used to enable/disable modules on different (multi)sites and environments (during automatic deploy for example).

### Dependencies:
- Drush
- php-yaml (php5-yaml on Ubuntu/Debian)

### Usage:
Set `$conf['env']` to settings.php or via `variable_set('env', 'anything');` to use environment specific modules (optional)

Run:
- `drush dmodm --yaml-file /absolute/path/to/your/module-dependencies.yaml` to use absolute path
- `drush dmodm --yaml-file module-dependencies.yaml` to load from the module directory

### Examples

**Same modules on every site and environment.**
````yaml
all: # <- Global modules
  views: 1
````

**Environment specific modules (no multisite).**
````yaml
all: # <- Global modules
  views: 1
local: # <- Local (environment) specific modules
  all:
    views_ui: 1
prod: # <- Prod (environment) specific modules
  all:
    devel: 0
    maillog: 0
````

**Evironment and site specific modules.**
````yaml
all: # <- Global modules
  views: 1
local: # <- Local (environment) specific modules
  all: # <- Modules that will be enabled on every (multi) site
    views_ui: 1
  multisite1: # <- Modules that will be enabled only on this site
    webform: 1 
    context: 1
prod: # <- Prod (environment) specific modules
  all: # <- Modules that will be enabled on every (multi) site
    devel: 0
    maillog: 0
  multisite1: # <- Modules that will be enabled only on this site
    admin_menu: 1 
    entity: 1
````

Site specific settings will always override environment and environment will always override global.

### Capistrano example
````ruby
# List the Drupal multi-site folders. Use "default" if no multi-sites are installed.
set :domains, ['default'] 
````

````ruby
 desc 'Update module dependencies'
  task :dmodm do
    on release_roles :all do
      fetch(:domains).each do |domain|
      execute "#{fetch(:drush)} dmodm -y --uri=#{domain}"
    end
  end
end
````
