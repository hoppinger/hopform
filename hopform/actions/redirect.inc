<?php

function hopform_hopform_action_redirect_form($action, &$form, &$form_state) {
  $form['path'] = array(
    '#type' => 'textfield',
    '#title' => t('Path'),
    '#default_value' => !empty($action->settings['path']) ? drupal_get_path_alias($action->settings['path']) : '',
    '#description' => t('Enter the path that the user should be redirected to.'),
    '#required' => TRUE,
  );
}

function hopform_hopform_action_redirect_form_submit($action, $form, &$form_state) {
  $action->settings['path'] = drupal_get_normal_path($form_state['values']['path']);
}

function hopform_hopform_action_redirect_summary($action) {
  if (empty($action->settings['path']) || $action->settings['path'] == '<front>') {
    return t('Redirect to frontpage');
  }

  return t('Redirect to %path', array('%path' => drupal_get_path_alias($action->settings['path'])));
}

function hopform_hopform_action_redirect_execute($action, $form, &$form_state, $f) {
  if (empty($action->settings['path']) || $action->settings['path'] == '<front>') {
    $form_state['redirect'] = '<front>';
  } else {
    $form_state['redirect'] = $action->settings['path'];
  }
}