<?php

function hopform_hopform_action_email_form($action, &$form, &$form_state) {
  if (!isset($form_state['addresses_count'])) {
    $form_state['addresses'] = !empty($action->settings['addresses']) ? array_values($action->settings['addresses']) : array();
    $form_state['addresses_count'] = !empty($action->settings['addresses']) ? (count($action->settings['addresses']) + 1) : 3;
  }

  $form['addresses_wrapper'] = array(
    '#type' => 'fieldset',
    '#title' => t('Email addresses'),
  );

  $form['addresses_wrapper']['addresses'] = array(
    '#prefix' => '<div id="hopform-action-email-addresses">',
    '#suffix' => '</div>',
    '#tree' => TRUE,
  );

  for ($i = 0; $i < $form_state['addresses_count']; $i++) {
    $form['addresses_wrapper']['addresses'][$i] = array(
      '#type' => 'textfield',
      '#title' => t('Email address'),
      '#title_display' => 'invisible',
      '#default_value' => !empty($form_state['addresses'][$i]) ? $form_state['addresses'][$i] : '',
      '#element_validate' => array('_hopform_hopform_action_email_element_validate'),
    );
  }

  $form['addresses_wrapper']['email_more'] = array(
    '#type' => 'submit',
    '#value' => t('More addresses'),
    '#attributes' => array(
      'title' => t("If the amount of boxes above isn't enough, click here to add more addresses."),
    ),
    '#weight' => 1,
    '#limit_validation_errors' => array(array('addresses')),
    '#submit' => array('hopform_hopform_action_email_form_more_submit'),
    '#ajax' => array(
      'callback' => 'hopform_hopform_action_email_form_more_js',
      'wrapper' => 'hopform-action-email-addresses',
      'effect' => 'fade',
    ),
  );

  $form['from'] = array(
    '#type' => 'textfield',
    '#title' => t('From address'),
    '#default_value' => !empty($action->settings['from']) ? $action->settings['from'] : '',
    '#element_validate' => array('_hopform_hopform_action_email_element_validate'),
    '#required' => TRUE,
  );
  $form['subject'] = array(
    '#type' => 'textfield',
    '#title' => t('Subject'),
    '#default_value' => !empty($action->settings['subject']) ? $action->settings['subject'] : '',
    '#required' => TRUE,
  );

  $form['intro'] = array(
    '#type' => 'textarea',
    '#title' => t('Introduction'),
    '#default_value' => !empty($action->settings['intro']) ? $action->settings['intro'] : '',
    '#description' => t('This text will be displayed at the top of the e-mail.'),
  );

}

function hopform_hopform_action_email_form_more_submit($form, &$form_state) {
  _hopform_hopform_action_email_form_submit_cleanup($form_state);

  if ($form_state['values']['email_more']) {
    $n = $_GET['q'] == 'system/ajax' ? 1 : 5;
    $form_state['addresses_count'] += $n;
  }

  $form_state['rebuild'] = TRUE;
}

function hopform_hopform_action_email_form_submit($action, $form, &$form_state) {
  _hopform_hopform_action_email_form_submit_cleanup($form_state);
  $action->settings['addresses'] = $form_state['addresses'];
  $action->settings['from'] = $form_state['values']['from'];
  $action->settings['subject'] = $form_state['values']['subject'];
  $action->settings['intro'] = $form_state['values']['intro'];
}

function _hopform_hopform_action_email_form_submit_cleanup(&$form_state) {
  $countdown = 0;
  $addresses = array();
  for ($i = 0; $i < $form_state['addresses_count']; $i++) {
    if (!empty($form_state['values']['addresses'][$i]) && trim($form_state['values']['addresses'][$i])) {
      $addresses[] = trim($form_state['values']['addresses'][$i]);
    } elseif (!empty($form_state['addresses'][$i])) {
      $countdown += 1;
    }
  }

  $form_state['addresses_count'] -= $countdown;
  $form_state['addresses'] = $addresses;
  unset($form_state['input']['addresses']);
}

function hopform_hopform_action_email_form_more_js($form, $form_state) {
  return $form['addresses_wrapper']['addresses'];
}

function _hopform_hopform_action_email_element_validate($element, &$form_state) {
  if (empty($element['#value']) || !trim($element['#value'])) {
    return;
  }

  $email = $element['#value'];
  if (!valid_email_address($email)) {
    form_error($element, t('Only valid e-mailaddress are allowed.'));
  }
}

function hopform_hopform_action_email_summary($action) {
  $addresses = !empty($action->settings['addresses']) ? array_values($action->settings['addresses']) : array();

  return t('<ul><li>To !addresses</li><li>From %from</li><li>Subject: %subject</li></ul>', array(
    '!addresses' => '<em>' . implode('</em>, <em>', array_map('check_plain', $addresses)) . '</em>',
    '%from' => $action->settings['from'],
    '%subject' => $action->settings['subject'],
  ));
}

function hopform_hopform_action_email_execute($action, $form, &$form_state, $f) {
  $language = $GLOBALS['language'];

  $result = hopform_form_build_values($f->form, $form_state);
  $text   = _hopform_hopform_action_email_construct_text($result);
  $text   = implode("\n", $text);

  $intro  = _hopform_hopform_action_email_format_text(!empty($action->settings['intro']) ? $action->settings['intro'] : '');
  $intro = implode("\n", $intro);

  if (!empty($action->settings) && !empty($action->settings['addresses'])) {
    foreach ($action->settings['addresses'] as $address) {
      drupal_mail('hopform', 'action_email', $address, $language, array(
        'text' => $text,
        'action' => $action,
        'intro' => $intro,
      ), $action->settings['from']);
    }
  }
}

function _hopform_hopform_action_email_construct_text($info, $prefix = 0, $width = 80) {
  $text = array();

  foreach ($info as $key => $_info) {
    if (!empty($_info['label'])) {
      $text[] = $_info['label'] . ':';
    }
    if (!empty($_info['value'])) {
      if (is_array($_info['value'])) {
        foreach ($_info['value'] as $row) {
          $text = array_merge($text, _hopform_hopform_action_email_format_text($row, $width));
        }
      } else {
        $text = array_merge($text, _hopform_hopform_action_email_format_text($_info['value'], $width));
      }
    }
    if (!empty($_info['children'])) {
      $text = array_merge($text, _hopform_hopform_action_email_construct_text($_info['children'], $prefix + 2, $width - 2));
    }

    if (!count($text) || $text[count($text)-1] != '') {
      $text[] = '';
    }
  }

  if ($prefix) {
    $p = str_repeat(' ', $prefix);
    foreach ($text as $key => $value) {
      $text[$key] = $value ? ($p . $value) : '';
    }
  }

  return $text;
}

function _hopform_hopform_action_email_format_text($text, $width = 80) {
  $chunks = explode("\n", $text);

  $result = array();
  foreach ($chunks as $chunk) {
    if (strlen($chunk) <= $width) {
      $result[] = $chunk;
    }
    else {
      $wrapped = wordwrap($chunk, $width);
      $result += _hopform_hopform_action_email_format_text($wrapped, $width);
    }
  }

  return $result;
}
