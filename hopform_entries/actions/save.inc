<?php

function hopform_entries_hopform_action_save_form($action, &$form, &$form_state) {
  $form['ftp'] = array(
    '#type' => 'fieldset',
    '#title' => 'FTP Upload',
    '#description' => 'The saved entries can be uploaded to an FTP server in CSV format.',
  );

  $form['ftp']['ftp_upload_required'] = array(
    '#type' => 'checkbox',
    '#title' => t('Upload form results to a FTP server'),
    '#description' => t('Check this box if you want to upload the form results to an FTP server.'),
    '#default_value' => $action->settings['ftp_upload_required'],
  );

  $form['ftp']['ftp_host'] = array(
    '#type' => 'textfield',
    '#title' => t('Hostname'),
    '#description' => t('Enter the hostname of the server to which you want to upload. This can be an IP or domain name.'),
    '#states' => array(
      'invisible' => array(
         ':input[name="ftp_upload_required"]' => array('checked' => FALSE),
      ),
    ),
    '#default_value' => $action->settings['ftp_host'],
  );

  $form['ftp']['ftp_port'] = array(
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

  $form['ftp']['ftp_user'] = array(
    '#type' => 'textfield',
    '#title' => t('Username'),
    '#description' => t('The username of the user that you want to use to authenticate against the FTP server.'),
    '#states' => array(
      'invisible' => array(
         ':input[name="ftp_upload_required"]' => array('checked' => FALSE),
      ),
    ),
    '#default_value' => $action->settings['ftp_user'],
  );

  $form['ftp']['ftp_pass'] = array(
    '#type' => 'password',
    '#title' => t('Password'),
    '#description' => t('The password of the user that you want to use to authenticate against the FTP server. This only needs to be filled in when you want to change the password.'),
    '#states' => array(
      'invisible' => array(
         ':input[name="ftp_upload_required"]' => array('checked' => FALSE),
      ),
    ),
    '#default_value' => '',
  );

  $form['ftp']['ftp_path'] = array(
    '#type' => 'textfield',
    '#title' => t('Path'),
    '#description' => t('The path to the file where you want to upload to the FTP server. <strong>Warning: If this file already exists, it will be overwritten.</strong>'),
    '#states' => array(
      'invisible' => array(
         ':input[name="ftp_upload_required"]' => array('checked' => FALSE),
      ),
    ),
    '#default_value' => $action->settings['ftp_path'],
  );

  $form['ftp']['ftp_upload_frequency_hours'] = array(
    '#type' => 'select',
    '#options' => array(
      1 => t('Every hour'),
      2 => t('Every two hours'),
      6 => t('Every six hours'),
      12 => t('Every twelve hours'),
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
  if(empty($form_state['values']['ftp_upload_required'])) {
    return;
  }

  foreach (array(
    'ftp_host',
    'ftp_port',
    'ftp_user',
    'ftp_pass',
    'ftp_path'
  ) as $field) {
    $element = $form['ftp'][$field];

    $key_exists = NULL;
    $value = drupal_array_get_nested_value($form_state['values'], $element['#parents'], $key_exists);

    if (!($field == 'ftp_pass' && !empty($action->settings['ftp_pass']))) {
      if (!$key_exists || empty($value)) {
        form_error($element, $t('!name field is required.', array('!name' => $element['#title'])));
        continue;
      }
    }

    if ($field == 'ftp_port') {
      if (!is_numeric($value)) {
        form_error($element, $t('!name field should be a number.', array('!name' => $element['#title'])));
      }
    }

    if ($field == 'ftp_path') {
      if (!preg_match('/^(?:[0-9a-zA-Z\_\-.]+\/)*(?:[0-9a-zA-Z\_\-.]+)$/', $value)) {
        form_error($element, $t('!name field should be a valid path.', array('!name' => $element['#title'])));
      }
    }
  }

  if (!form_get_errors()) {
    $conn_id = ftp_connect($form_state['values']['ftp_host'], $form_state['values']['ftp_port']);
    if (!$conn_id) {
      form_error($form['ftp'], t('Could not connect to the specified FTP server'));
      return;
    }

    if (!empty($form_state['values']['ftp_pass'])) {
      $pass = $form_state['values']['ftp_pass'];
    } else {
      $pass = $action->settings['ftp_pass'];
    }

    $login_result = @ftp_login($conn_id, $form_state['values']['ftp_user'], $pass);

    if (!$login_result) {
      ftp_close($conn_id);
      form_error($form['ftp'], t('Could not authenticate the specified FTP server with the given credentials.'));
      return;
    }

    $pasv_result = ftp_pasv($conn_id, TRUE);
    if (!$pasv_result) {
      ftp_close($conn_id);
      form_error($form['ftp'], t('Could not switch to passive mode on the specified FTP server.'));
      return;
    }

    $dirname = dirname($form_state['values']['ftp_path']);
    if ($dirname != '.') {
      $directory_result = ftp_rawlist($conn_id, $dirname);
      if (!$directory_result) {
        ftp_close($conn_id);
        form_error($form['ftp'], t('The specified directory does not exist.'));
        return;
      }
    }

    ftp_close($conn_id);
    drupal_set_message(t('We tested you FTP connection and it seems to be working.'), 'status', FALSE);
  }
}

function hopform_entries_hopform_action_save_form_submit($action, $form, &$form_state) {
  $enabled = (bool) $form_state['values']['ftp_upload_required'];
  $action->settings['ftp_upload_required'] = $enabled;

  foreach (array(
    'ftp_host',
    'ftp_port',
    'ftp_user',
    'ftp_pass',
    'ftp_path',
    'ftp_upload_frequency_hours',
  ) as $field) {
    if ($enabled) {
      if (!($field == 'ftp_pass' && empty($form_state['values'][$field]))) {
        $action->settings[$field] = $form_state['values'][$field];
      }
    } else {
      if (array_key_exists($field, $action->settings)) {
        unset($action->settings[$field]);
      }
    }
  }
}

function hopform_entries_hopform_action_save_summary($action) {
  if (empty($action->settings['ftp_upload_required'])) {
    return t('Save entry');
  }

  $frequency_options = array(
    1 => t('hourly'),
    24 => t('daily'),
    168 => t('weekly'),
  );

  $frequency = $action->settings['ftp_upload_frequency_hours'];
  if (!empty($frequency_options[$frequency])) {
    $display_frequency = $frequency_options[$frequency];
  } else {
    $display_frequency = t('every @count hours', array('@count' => $frequency));
  }

  return t('Save entry and upload %frequency to %hostname', array(
    '%frequency' => $display_frequency,
    '%hostname' => $action->settings['ftp_host'],
  ));
}

function hopform_entries_hopform_action_save_execute($action, $form, &$form_state, $f) {
  $result = hopform_form_build_values($f->form, $form_state);

  $entry = hopform_form_entry_new($f);
  $entry->data = $result;
  hopform_form_entry_save($entry);
}
