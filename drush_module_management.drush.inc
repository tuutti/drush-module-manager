<?php
/**
 * @file
 * Drush module management.
 */

/**
 * Implements hook_drush_command().
 */
function drush_module_management_drush_command() {
  $items['drush-module-management'] = array(
    'description' => 'Manage hard dependencies',
    'aliases' => array('dmodm'),
  );
  return $items;
}

/**
 * Get list of modules.
 */
function drush_module_management_get_modules($env = NULL) {
  $dependencies = &drupal_static(__FUNCTION__);

  if (!$dependencies) {
    $dependencies = yaml_parse_file(__DIR__ . '/dependencies.yaml');
  }
  return !empty($dependencies[$env]) ? $dependencies[$env] : FALSE;
}

/**
 * Callback for the drush-demo-command command.
 */
function drush_drush_module_management() {
  $site = drush_get_context('DRUSH_DRUPAL_SITE');

  if (empty($site)) {
    drupal_set_message('Failed to load site.');
  }
  $env = variable_get('env', NULL);

  if (!$env) {
    drupal_set_message('Environment not found.');
  }

  $dependencies = drush_module_management_get_modules($env);

  // Site spefic modules.
  if (!empty($dependencies[$site])) {
    drush_module_management_change_module_state($dependencies[$site]);
  }
  // Environment specific modules.
  if (!empty($dependencies['all'])) {
    drush_module_management_change_module_state($dependencies['all']);
  }
}

/**
 * Enable / disable modules based on state.
 */
function drush_module_management_change_module_state($list) {
  $modules = array();
  foreach ($list as $module => $state) {
    if (empty($state)) {
      if (module_exists($module)) {
        $modules['disable'][] = $module;
      }
    }
    else {
      if (!module_exists($module)) {
        $modules['enable'][] = $module;
      }
    }
  }
  // Enable given modules.
  if (!empty($modules['enable'])) {
    module_enable($modules['enable']);

    drush_module_management_module_output($modules['enable'], TRUE);
  }
  // Disable given modules.
  if (!empty($modules['disable'])) {
    module_disable($modules['disable']);

    drush_module_management_module_output($modules['disable'], FALSE);
  }
}

/**
 * Output string from array.
 */
function drush_module_management_module_output($modules, $state) {
  $string = format_string('@state the following modules: @modules', array(
    '@state' => empty($state) ? 'Disabled' : 'Enabled',
    '@modules' => implode(',', $modules),
  ));
  drupal_set_message($string);
}
