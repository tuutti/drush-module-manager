Dependencies:
- Drush

Usage:

Set $conf['env'] to settings.php or via variable_set('env', 'anything');

Edit dependencies.yaml according to your needs.

This can be used to enable/disable modules on different environments (during automatic deploy for example) without having to mess with database updates.

Run `drush dmodm` or `drush drush-module-management`
