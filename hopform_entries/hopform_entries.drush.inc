<?php 

/**
 * Implements hook_drush_command().
 */
function hopform_entries_drush_command() {
  $items['hop-upload-csv'] = array(
    'description' => 'create csv file and upload it to the ftp server.',
    'aliases' => array('hcsv'),
  );

  return $items;
}

/**
 * Callback for the hop-upload-csv-command
 */
function drush_hopform_entries_hop_upload_csv() {
  
  // load type = 'save' values from db
  $result = db_select('hopform_form_actions', 'hfa')
    ->fields('hfa', array('aid'))
    ->condition('type', 'save')
    ->execute();

  // check if has ftp upload enabled
  while($record = $result->fetchAssoc()) {
    $action = hopform_form_action_load($record['aid']);
    $as = $action->settings;

    if($as['ftp_upload_required'] == 1) {
      // check if upload frequency time is passed
      if(($as['ftp_upload_frequency_hours'] * 3600) + variable_get('last_hopform_drush_run', time()) > time()) {
        
        // create form object by form_id
        $form = hopform_form_id_form($action->fid);
        
        // build csv file
        $file = hopform_entries_create_csv($form, $as['ftp_filename']);
        
        // upload file to ftp
        hopform_entries_upload_file( $file, $as['ftp_address'], $as['ftp_user'], $as['ftp_pass'], $as['ftp_port'], $as['ftp_remote_dir'] );
      }
    }
  }
  
  // update last call to this function
  variable_set('last_hopform_drush_run', time());
}