<?php

/**
 * Implements hook_hopform_elements().
 */
function hopform_hopform_elements() {
  require_once drupal_get_path('module', 'hopform') . '/hopform.hopform_elements.inc';

  return _hopform_hopform_elements();
}

/**
 * Implements hook_hopform_action_info().
 */
function hopform_hopform_action_info() {
  return array(
    'email'    => array('label' => 'Email',
                        'file' => drupal_get_path('module', 'hopform') . '/actions/email.inc',
                        'description' => 'Verstuurd een e-mail naar een opgegeven adres.'),
    'message'  => array('label' => 'Message',
                        'file' => drupal_get_path('module', 'hopform') . '/actions/message.inc',
                        'description' => 'Geeft een Drupal-message weer, bijvoorbeeld een groene balk.'),
    'redirect' => array('label' => 'Redirect',
                        'file' => drupal_get_path('module', 'hopform') . '/actions/redirect.inc',
                        'description' => 'Stuurt de bezoeker naar een andere pagina.'),
  );
}
