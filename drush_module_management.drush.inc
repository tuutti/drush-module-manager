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
    'options' => array(
      'yaml-file' => 'User provided dependencies.yaml',
    ),
  );
  return $items;
}

/**
 * Get list of modules.
 *
 * @param string $yaml_file
 *   Path to dependencie yaml file.
 * @param string $env
 *   Drupal environment variable
 *
 * @return array
 *   List of modules for given environment.
 *
 * @todo Refactor.
 */
function drush_module_management_get_modules($yaml_file, $env = NULL) {
  $dependencies = &drupal_static(__FUNCTION__ . $env);

  if (!file_exists($yaml_file)) {
    drush_log('Dependencies file is missing.', 'error');
    exit;
  }
  if (!$dependencies) {
    $dependencies = yaml_parse_file($yaml_file);
  }
  return !empty($dependencies[$env]) ? $dependencies[$env] : array();
}

/**
 * Callback for the drush-demo-command command.
 *
 * @todo Refactor.
 */
function drush_drush_module_management($yaml_file = NULL) {
  $site = drush_get_context('DRUSH_DRUPAL_SITE');

  if (!$yaml_file) {
    drush_log('Dependencies file is missing', 'error');
    exit;
  }

  if (basename($yaml_file) == $yaml_file) {
    // User provided file without path. Try to load file from current folder.
    $yaml_file = drupal_get_path('module', 'drush_module_management') . '/' . $yaml_file;
  }
  // Site should fallback to 'default'.
  if (empty($site)) {
    drush_log('Failed to load site.', 'error');
    exit;
  }
  $env = variable_get('env', NULL);

  $modules = drush_module_management_get_order($env, $site, $yaml_file);

  drush_module_management_change_module_state($modules);
}

/**
 * Get given modules in proper order.
 *
 * @param string $env
 *   Drupal environment variable.
 * @param string $site
 *   Drush site variable. Fallbacks to 'default'.
 *
 * @return array
 *   Array of modules.
 *
 * @todo Refactor.
 */
function drush_module_management_get_order($env, $site, $yaml_file) {
  $modules = array();
  // Allow users to specify global modules under the 'all' environment.
  foreach (drush_module_management_get_modules($yaml_file, 'all') as $module => $state) {
    $modules[$module] = $state;
  }
  $dependencies = drush_module_management_get_modules($yaml_file, $env);

  if (!empty($dependencies['all'])) {
    // Allow environment to override global modules.
    foreach ($dependencies['all'] as $module => $state) {
      $modules[$module] = $state;
    }
  }

  // Site spefic modules.
  if (!empty($dependencies[$site])) {
    // Allow site to override environment and global modules.
    foreach ($dependencies[$site] as $module => $state) {
      $modules[$module] = $state;
    }
  }

  return $modules;
}

/**
 * Enable / disable modules based on given state.
 *
 * @param array $list
 *   List of modules to enable / disable.
 *
 * @todo Refactor.
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
      if (!drupal_get_path('module', $module)) {
        // Trying to enable module tha does not exists.
        return drush_log(t('Trying to enable module that does not exists: @module', array(
          '@module' => $module,
        )), 'error');
      }
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
  drush_log($string);
}
