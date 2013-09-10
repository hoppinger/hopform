<?php

function hopform_ui_form_type_overview() {
  $types = hopform_form_types();

  $header = array(
    t('Name'),
    array(
      'data' => t('Operations'),
      'colspan' => 4,
    )
  );

  $rows = array();
  foreach($types as $info) {
    $type_url_str = str_replace('_', '-', $info->type);

    $rows[] = array(
      check_plain($info->label),
      l(t('edit'), 'admin/structure/hopform/' . $type_url_str),
      l(t('manage fields'), 'admin/structure/hopform/' . $type_url_str . '/fields'),
      l(t('manage display'), 'admin/structure/hopform/' . $type_url_str . '/display'),
      !empty($info->custom) ? l(t('delete'), 'admin/structure/hopform/' . $type_url_str . '/delete') : '',
    );
  }

  $build = array();
  $build['form_type_table'] = array(
    '#theme' => 'table',
    '#header' => $header,
    '#rows' => $rows,
    '#empty' => t('No form types available. <a href="@link">Add form type</a>.', array('@link' => url('admin/structure/hopform/add'))),
  );

  return $build;
}
