<?php

function hopform_form_page_entries($form) {
  $result = db_select('hopform_form_entries', 'e')
    ->fields('e')
    ->condition('e.fid', $form->fid)
    ->orderBy('e.created', 'DESC')
    ->extend('PagerDefault')
    ->limit(50)
    ->execute();

  $entries = array();
  foreach ($result as $entry) {
    if (!empty($entry->data)) {
      $entry->data = unserialize($entry->data);
    }

    $entries[] = $entry;
  }

  $header = array(
    t('Hostname'),
    t('Created'),
    array(
      'colspan' => 2,
      'data' => t('Operations'),
    )
  );

  $rows = array();
  foreach ($entries as $entry) {
    $rows[] = array(
      check_plain($entry->hostname),
      format_date($entry->created, 'short'),
      l(t('View'), 'form/' . $form->fid . '/entries/' . $entry->feid),
      l(t('Delete'), 'form/' . $form->fid . '/entries/' . $entry->feid . '/delete'),
    );
  }

  $build = array(
    drupal_get_form('hopform_entries_export', $form),
    array(
      '#theme' => 'table',
      '#rows' => $rows,
      '#header' => $header,
      '#empty' => t('No entries found'),
    ),
    array('#theme' => 'pager'),
  );

  return $build;
}

function hopform_form_page_entry($form, $entry) {
  if ($form->fid != $entry->fid) {
    return MENU_NOT_FOUND;
  }
  
  return hopform_form_entry_view($entry);
}