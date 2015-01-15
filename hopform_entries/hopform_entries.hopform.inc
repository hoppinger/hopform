<?php

/**
 * Implements hook_hopform_action_info().
 */
function hopform_entries_hopform_action_info() {
  return array(
    'save'     => array('label' => 'Save',
                        'file' => drupal_get_path('module', 'hopform_entries') . '/actions/save.inc',
                        'description' => 'Saves the entries in the database with a option to upload CSV files to a ftp server.'),
  );
}