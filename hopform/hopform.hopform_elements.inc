<?php

function _hopform_hopform_elements() {
  $fields = array();

  $fields['fieldset'] = array(
    'title' => t('Fieldset'),
    'properties' => array(
      'title',
      'description',
      'collapsible',
      'collapsed',
    ),
    'default' => array(
      '#title' => t('New fieldset'),
      '#type' => 'fieldset',
      '#collapsible' => TRUE,
      '#collapsed' => FALSE,
    ),
  );

  $fields['number'] = array(
    'title' => t('Number'),
    'properties' => array(
      'title',
      'description',
      'field_prefix',
      'field_suffix',
      'default_value',
      'required',
    ),
    // TODO: how to handle validate functions?
    'validate' => array(
      'number',
    ),
    'default' => array(
      '#key' => 'number',
      '#title' => t('New number'),
      '#type' => 'textfield',
    ),
    'show_callback' => 'hopform_elements_text_show',
  );

  $fields['select'] = array(
    'title' => t('Select list'),
    'properties' => array(
      'title',
      'description',
      'default_value',
      'default_value_from_url',
      'required',
      'options',
      'multiple', // Handled by options element.
      'key_type', // Handled by options element.
      'key_type_toggle', // Handled by options element.
      'key_type_toggled', // Handled by options element.
    ),
    'default' => array(
      '#title' => t('New select list'),
      '#type' => 'select',
      '#options' => array('1' => 'one', '2' => 'two', '3' => 'three'),
      '#multiple_toggle' => TRUE,
      '#default_value' => '',
    ),
    'show_callback' => 'hopform_elements_select_show',
  );

  $fields['checkboxes'] = array(
    'title' => t('Checkboxes'),
    'properties' => array(
      'title',
      'description',
      'default_value',
      'default_value_from_url',
      'required',
      'options',
      'multiple',
      'key_type', // Handled by options element.
      'key_type_toggle', // Handled by options element.
      'key_type_toggled', // Handled by options element.
    ),
    'default' => array(
      '#title' => t('New checkboxes'),
      '#type' => 'checkboxes',
      '#options' => array('one' => 'one', 'two' => 'two', 'three' => 'three'),
    ),
    'show_callback' => 'hopform_elements_checkboxes_show',
  );

  $fields['radios'] = array(
    'title' => t('Radios'),
    'properties' => array(
      'title',
      'description',
      'default_value',
      'default_value_from_url',
      'required',
      'options',
      'key_type', // Handled by options element.
      'key_type_toggle', // Handled by options element.
      'key_type_toggled', // Handled by options element.
    ),
    'default' => array(
      '#title' => t('New radios'),
      '#type' => 'radios',
      '#options' => array('one' => 'one', 'two' => 'two', 'three' => 'three'),
    ),
    'show_callback' => 'hopform_elements_radios_show',
  );

  $fields['textfield'] = array(
    'title' => t('Textfield'),
    'properties' => array(
      'title',
      'disabled',
      'description',
      'field_prefix',
      'field_suffix',
      'default_value',
      'default_value_from_url',
      'required',
      'size',
    ),
    'default' => array(
      '#title' => t('New textfield'),
      '#type' => 'textfield',
    ),
    'show_callback' => 'hopform_elements_text_show',
  );

  $fields['textarea'] = array(
    'title' => t('Textarea'),
    'properties' => array(
      'title',
      'description',
      'default_value',
      'default_value_from_url',
      'required',
      'rows',
      'cols',
    ),
    'default' => array(
      '#title' => t('New textarea'),
      '#type' => 'textarea',
    ),
    'show_callback' => 'hopform_elements_textarea_show',
  );

  $fields['file'] = array(
    'title' => t('File'),
    'properties' => array(
      'title',
      'description',
      'required',
      'upload_location',
      'upload_validators',
    ),
    'default' => array(
      '#title' => t('New file'),
      '#type' => 'managed_file',
      '#upload_location' => 'private://hopform',
      '#upload_validators' => array(
        'file_validate_extensions' => array('pdf'),
        'file_validate_size' => array(5 * 1024),
      ),
    ),
    'show_callback' => 'hopform_elements_file_show',
    'url_callback' => 'hopform_elements_file_url',
  );

  return $fields;
}

function hopform_elements_textarea_show($element, $value) {
  return $value;
}

function hopform_elements_text_show($element, $value) {
  return $value;
}

function hopform_elements_checkboxes_show($element, $value) {
  $result = array();
  foreach (array_keys(array_filter($value)) as $key) {
    $result[] = $element['#options'][$key] . (!empty($element['#key_type_toggled']) ? (' (' . $key . ')') : '');
  }

  return $result;
}

function hopform_elements_radios_show($element, $value) {
  return $element['#options'][$value] . (!empty($element['#key_type_toggled']) ? (' (' . $value . ')') : '');
}

function hopform_elements_select_show($element, $value) {
  return !empty($element['#multiple']) ? hopform_elements_checkboxes_show($element, $value) : hopform_elements_radios_show($element, $value);
}

function hopform_elements_file_show($element, $value) {
  $file = file_load($value);
  return $file ? $file->filename : '';
}

function hopform_elements_file_url($value) {
  $file = file_load($value);

  return $file ? file_create_url($file->uri) : FALSE;
}