<?php

function hopform_hopform_action_message_form($action, &$form, &$form_state) {
  $form['type'] = array(
    '#type' => 'select',
    '#title' => t('Message type'),
    '#default_value' => !empty($action->settings['type']) ? $action->settings['type'] : 'status',
    '#options' => _hopform_hopform_action_message_type_options(),
  );

  $form['message'] = array(
    '#type' => 'textarea',
    '#title' => t('Message'),
    '#default_value' => !empty($action->settings['message']) ? $action->settings['message'] : '',
    '#description' => t('Enter the message that should be shown to the user here.'),
    '#required' => TRUE,
  );
}

function hopform_hopform_action_message_form_submit($action, $form, &$form_state) {
  $action->settings['type'] = $form_state['values']['type'];
  $action->settings['message'] = $form_state['values']['message'];
}

function hopform_hopform_action_message_summary($action) {
  $options = _hopform_hopform_action_message_type_options();

  return t('@message_type: %message', array(
    '@message_type' => $options[!empty($action->settings['type']) ? $action->settings['type'] : 'status'],
    '%message' => !empty($action->settings['message']) ? $action->settings['message'] : ''
  ));
}

function hopform_hopform_action_message_execute($action, $form, &$form_state, $f) {
  if (!empty($action->settings['message'])) {
    drupal_set_message(
      check_plain($action->settings['message']), 
      !empty($action->settings['type']) ? $action->settings['type'] : 'status'
    );
  }
}

function _hopform_hopform_action_message_type_options() {
  return array(
    'status' => t('Status'),
    'warning' => t('Warning'),
    'error' => t('Error'),
  );
}