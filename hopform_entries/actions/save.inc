<?php

function hopform_entries_hopform_action_save_summary($action) {
  return t('Save entry');
}

function hopform_entries_hopform_action_save_execute($action, $form, &$form_state, $f) {
  $result = hopform_form_build_values($f->form, $form_state);

  $entry = hopform_form_entry_new($f);
  $entry->data = $result;
  hopform_form_entry_save($entry);
}
