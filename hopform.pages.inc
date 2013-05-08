<?php

function hopform_form_page_view($form) {
  drupal_set_title($form->title);
  $build = hopform_form_view_multiple(array($form->fid => $form), 'full');
  $build['form'] = hopform_form_display($form);

  return $build;
}

function hopform_form_page_edit($form) {
  return 'edit';
}

function hopform_form_page_form_edit($form) {
  require_once drupal_get_path('module', 'form_builder') . '/includes/form_builder.admin.inc';

  return array(
    form_builder_interface('hopform_' . $form->type, hopform_form_form_id($form)),
    drupal_get_form('hopform_save_form', $form),
  );
}

function hopform_form_page_actions_edit($form) {
  return drupal_get_form('hopform_actions_form', $form);
}

function hopform_save_form($form, &$form_state, $f) {
  $form['#hopform'] = $f;

  $form['actions'] = array(
    '#type' => 'actions',
  );
  $form['actions']['save'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
  );
  $form['actions']['cancel'] = array(
    '#type' => 'submit',
    '#value' => t('Cancel'),
    '#submit' => array('hopform_save_form_cancel'),
  );

  return $form;
}

function hopform_save_form_submit($form, &$form_state) {
  $f = $form['#hopform'];

  $form_cache = form_builder_cache_load('hopform_' . $f->type, hopform_form_form_id($f));

  $f->form = $form_cache;

  hopform_form_save($f);

  form_builder_cache_delete('hopform_' . $f->type, hopform_form_form_id($f));

  drupal_set_message(t('Your form has been saved.'));
  $form_state['redirect'] = 'form/' . $f->fid;
}

function hopform_save_form_cancel($form, &$form_state) {
  $f = $form['#hopform'];

  form_builder_cache_delete('hopform_' . $f->type, hopform_form_form_id($f));

  drupal_set_message(t('The changes to your form have been discarded.'));
  $form_state['redirect'] = 'form/' . $f->fid;
}

function hopform_actions_form($form, &$form_state, $f) {
  

  return $form;
}
