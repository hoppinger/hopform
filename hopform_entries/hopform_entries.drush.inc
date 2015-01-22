<?php 

/**
 * Implements hook_drush_command().
 */
function hopform_entries_drush_command() {
  $items['hopform-entries-upload-csv'] = array(
    'description' => 'Create CSV files and upload them to the FTP servers.',
    'aliases' => array('hcsv'),
  );

  return $items;
}

/**
 * Callback for the hop-upload-csv-command
 */
function drush_hopform_entries_upload_csv() {
  $current_time = time();

  // load type = 'save' values from db
  $aids = db_select('hopform_form_actions', 'hfa')
    ->fields('hfa', array('aid'))
    ->condition('type', 'save')
    ->execute()->fetchCol();

  // check if has ftp upload enabled
  foreach ($aids as $aid) {
    $action = hopform_form_action_load($aid);
    $previous_time = !empty($action->settings['ftp_last_upload']) ? intval($action->settings['ftp_last_upload']) : 0;
    $difference = intval(round(floatval($current_time - $previous_time) / 3600.0));

    if (empty($action->settings['ftp_upload_required'])) {
      continue;
    }
    
    // check if upload frequency time is passed
    if ($action->settings['ftp_upload_frequency_hours'] > $difference) {
      continue;
    }

    // create form object by form_id
    $form = hopform_form_id_form($action->fid);
    
    // build csv file
    $file = hopform_entries_create_temporary_csv($form, 'hopform_form_export_');
    
    // upload file to ftp
    $upload_result = hopform_entries_upload_file($file, $action);

    if (!$upload_result) {
      watchdog('hopform_entries', 'Could not upload CSV for form @title to @hostname', array(
        '@title' => $form->title,
        '@hostname' => $action->settings['ftp_host'],
      ), WATCHDOG_ERROR, 'form/' . $form->fid . '/actions/' . $action->aid . '/edit');
    }

    $action->settings['ftp_last_upload'] = $current_time;
    hopform_form_action_save($action);
  }
}
