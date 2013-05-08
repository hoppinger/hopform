<?php

function hopform_hopform_elements() {
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
  );

  $fields['select'] = array(
    'title' => t('Select list'),
    'properties' => array(
      'title',
      'description',
      'default_value',
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
    ),
  );

  $fields['checkboxes'] = array(
    'title' => t('Checkboxes'),
    'properties' => array(
      'title',
      'description',
      'default_value',
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
  );

  $fields['radios'] = array(
    'title' => t('Radios'),
    'properties' => array(
      'title',
      'description',
      'default_value',
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
  );

  $fields['textfield'] = array(
    'title' => t('Textfield'),
    'properties' => array(
      'title',
      'description',
      'field_prefix',
      'field_suffix',
      'default_value',
      'required',
      'size',
    ),
    'default' => array(
      '#title' => t('New textfield'),
      '#type' => 'textfield',
    ),
  );

  $fields['textarea'] = array(
    'title' => t('Textarea'),
    'properties' => array(
      'title',
      'description',
      'default_value',
      'required',
      'rows',
      'cols',
    ),
    'default' => array(
      '#title' => t('New textarea'),
      '#type' => 'textarea',
    ),
  );

  $fields['file'] = array(
    'title' => t('File'),
    'properties' => array(
      'title',
      'description',
      'required',
      'size',
      'file_extension_validate',
      'file_size_validate',
    ),
    'default' => array(
      '#title' => t('New file'),
      '#type' => 'file',
      '#size' => 30,
    ),
  );

  $fields['image'] = array(
    'title' => t('Image'),
    'properties' => array(
      'title',
      'description',
      'required',
    ),
    'validate' => array(
      'file_extension_validate',
      'file_size_validate',
      'file_image_size',
    ),
    'default' => array(
      '#title' => t('New image'),
      '#type' => 'file',
      '#size' => 30,
    ),
  );

  return $fields;
}