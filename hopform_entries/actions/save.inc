<?php

function hopform_entries_hopform_action_save_form($action, &$form, &$form_state) {
  $form['ftp_upload_required'] = array(
    '#type' => 'checkbox',
    '#title' => t('Upload form results to ftp'),
    '#description' => t('check this box if you want to upload the form results to a ftp server'),
    '#default_value' => $action->settings['ftp_upload_required'],
  );
  $form['ftp_address'] = array(
    '#type' => 'textfield',
    '#title' => t('Ftp address'),
    '#description' => t('Enter the ftp address. This can be a ip or url without "http://"'),
    '#states' => array(
      'invisible' => array(
         ':input[name="ftp_upload_required"]' => array('checked' => FALSE),
      ),
    ),
    '#default_value' => $action->settings['ftp_address'],
  );
  $form['ftp_user'] = array(
    '#type' => 'textfield',
    '#title' => t('Username'),
    '#description' => t('The username of the ftp user'),
    '#states' => array(
      'invisible' => array(
         ':input[name="ftp_upload_required"]' => array('checked' => FALSE),
      ),
    ),
    '#default_value' => $action->settings['ftp_user'],
  );
  $form['ftp_pass'] = array(
    '#type' => 'password',
    '#title' => t('Password'),
    '#description' => t('The password of the ftp user'),
    '#states' => array(
      'invisible' => array(
         ':input[name="ftp_upload_required"]' => array('checked' => FALSE),
      ),
    ),
    // '#default_value' => $action->settings['ftp_pass'],
  );
  $form['ftp_port'] = array(
    '#type' => 'textfield',
    '#title' => t('Port number'),
    '#description' => t('The port number of the ftp server'),
    '#default_value' => !empty($action->settings['ftp_port']) ? $action->settings['ftp_port'] : '21',
    '#states' => array(
      'invisible' => array(
         ':input[name="ftp_upload_required"]' => array('checked' => FALSE),
      ),
    ),
  );
  $form['ftp_remote_dir'] = array(
    '#type' => 'textfield',
    '#title' => t('Remote upload directory'),
    '#states' => array(
      'invisible' => array(
         ':input[name="ftp_upload_required"]' => array('checked' => FALSE),
      ),
    ),
    '#default_value' => $action->settings['ftp_remote_dir'],
  );
  $form['ftp_filename'] = array(
    '#type' => 'textfield',
    '#title' => t('File name'),
    '#description' => t('The prefix of the filename that is uploaded to the ftp server.'),
    '#states' => array(
      'invisible' => array(
         ':input[name="ftp_upload_required"]' => array('checked' => FALSE),
      ),
    ),
    '#default_value' => $action->settings['ftp_filename'],
  );

  $form['ftp_upload_frequency_hours'] = array(
    '#type' => 'select',
    '#options' => array(
      1 => t('Every hour'),
      24 => t('Every day'),
      168 => t('Every week'),
    ),
    '#title' => t('Ftp upload frequency'),
    '#description' => t('Set the upload interval'),
    '#states' => array(
      'invisible' => array(
         ':input[name="ftp_upload_required"]' => array('checked' => FALSE),
      ),
    ),
    '#default_value' => !empty($action->settings['ftp_upload_frequency_hours']) ? $action->settings['ftp_upload_frequency_hours'] : '24',
  );
}

function hopform_entries_hopform_action_save_form_validate($action, $form, &$form_state) {

  if($form_state['values']['ftp_upload_required'] === 1) {
    $flag = FALSE;
    $ftp_server = $form_state['values']['ftp_address'];
    $ftp_user = $form_state['values']['ftp_user'];
    $ftp_pass = $form_state['values']['ftp_pass'];

    if(empty($ftp_server)) {
      form_set_error('ftp_address', t('Please fill in a ftp address.'));
      $flag = TRUE;
    }
    if(empty($ftp_user)) {
      form_set_error('ftp_user', t('Please fill in a ftp user.'));
      $flag = TRUE;
    }

    if(empty($ftp_pass)) {
      form_set_error('ftp_pass', t('Please fill in a ftp password.'));
      $flag = TRUE;
    }

    if(empty($form_state['values']['ftp_port'])) {
      form_set_error('ftp_port', t('Please fill in a ftp port number.'));
      $flag = TRUE;
    }
    elseif(!is_numeric($form_state['values']['ftp_port'])) {
      form_set_error('ftp_port', t('Port number is not a valid value.'));
      $flag = TRUE;
    }

    if(empty($form_state['values']['ftp_remote_dir'])) {
      form_set_error('ftp_remote_dir', t('Please fill in a ftp remote directory.'));
      $flag = TRUE;
    }
    elseif( substr($form_state['values']['ftp_remote_dir'], -1) !== "/" ){
      form_set_error('ftp_remote_dir', t('remote directory is not ending with a \'/\''));
    }

    if(empty($form_state['values']['ftp_filename'])) {
      form_set_error('ftp_filename', t('Please fill in a filename.'));
      $flag = TRUE;
    }
    elseif(preg_match("/^[a-zA-Z0-9]+$/", $form_state['values']['ftp_filename']) != 1 ) {
      form_set_error('ftp_filename', t('Please use only letters and or numbers for the filename.'));
    }


    // if no form errors, go and check the ftp connection
    if($flag == FALSE) {
      // set up a connection or trow form error
      $conn_id = ftp_connect($ftp_server, $form_state['values']['ftp_port']) or form_set_error('ftp', t('Could not connect to ftp.'));

      // try to login
      // @ is included to suppress posible php warning.
      if (@ftp_login($conn_id, $ftp_user, $ftp_pass)) {
        drupal_set_message('Connection is working.');
      } else {
        form_set_error('ftp', t('Could not connect to ftp with given credentials.'));
      }

      // close the connection
      ftp_close($conn_id);
    }
  }
}

function hopform_entries_hopform_action_save_form_submit($action, $form, &$form_state) {
  if($form_state['values']['ftp_upload_required']) {
    $action->settings['ftp_upload_required']        = $form_state['values']['ftp_upload_required'];
    $action->settings['ftp_address']                = $form_state['values']['ftp_address'];
    $action->settings['ftp_user']                   = $form_state['values']['ftp_user'];
    $action->settings['ftp_pass']                   = $form_state['values']['ftp_pass'];
    $action->settings['ftp_port']                   = $form_state['values']['ftp_port'];
    $action->settings['ftp_remote_dir']             = $form_state['values']['ftp_remote_dir'];
    $action->settings['ftp_filename']               = $form_state['values']['ftp_filename'];
    $action->settings['ftp_upload_frequency_hours'] = $form_state['values']['ftp_upload_frequency_hours'];
  }
}

function hopform_entries_hopform_action_save_summary($action) {
  return t('Save entry (to a ftp server) ');
}

function hopform_entries_hopform_action_save_execute($action, $form, &$form_state, $f) {
  $result = hopform_form_build_values($f->form, $form_state);

  $entry = hopform_form_entry_new($f);
  $entry->data = $result;
  hopform_form_entry_save($entry);
}
