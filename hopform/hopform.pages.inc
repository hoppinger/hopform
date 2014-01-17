<?php

/**
 * Menu callback. Choose a form type.
 */
function hopform_form_add_page() {
  $item = menu_get_item();
  $content = system_admin_menu_block($item);

  // Redirect to a single item is there is only one type.
  if (count($content) == 1) {
    $item = array_shift($content);
    drupal_goto($item['href']);
  }

  return theme('hopform_form_add_list', array('content' => $content));
}

/**
 * Theme callback for form type choosing list.
 */
function theme_hopform_form_add_list($variables) {
  $content = $variables['content'];
  $output = '';

  if ($content) {
    $output = '<dl class="node-type-list">';
    foreach ($content as $item) {
      $output .= '<dt>' . l($item['title'], $item['href'], $item['localized_options']) . '</dt>';
      $output .= '<dd>' . filter_xss_admin($item['description']) . '</dd>';
    }
    $output .= '</dl>';
  }
  else {
    $output = '<p>' . t('You have not created any form types yet.') . '</p>';
  }
  return $output;
}

/**
 * Menu callback. Add a new form of a certain type.
 */
function hopform_form_page_add($type) {
  $types = hopform_form_types();
  drupal_set_title(t('Create @name', array('@name' => $types[$type]->label)), PASS_THROUGH);

  $form = hopform_form_create(array('type' => $type));
  return drupal_get_form('hopform_form_' . $type . '_form', $form);
}

/**
 * Form callback. Hopform Form entity.
 */
function hopform_form_form($form, &$form_state, $f) {
  $form_state['hopform_form'] = $f;

  $form['title'] = array(
    '#type' => 'textfield',
    '#title' => t('Title'),
    '#required' => TRUE,
    '#default_value' => $f->title,
    '#maxlength' => 255,
    '#weight' => -5,
  );

  $form['additional_settings'] = array(
    '#type' => 'vertical_tabs',
    '#weight' => 99,
  );

  $form['options'] = array(
    '#type' => 'fieldset',
    '#title' => t('Publishing options'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#group' => 'additional_settings',
    '#attributes' => array(
      'class' => array('hopform-form-form-options'),
    ),
    '#attached' => array(
      'js' => array(drupal_get_path('module', 'hopform') . '/hopform.js'),
    ),
    '#weight' => 0,
  );
  $form['options']['status'] = array(
    '#type' => 'checkbox',
    '#title' => t('Published'),
    '#default_value' => $f->status,
  );

  if (module_exists('path')) {
    $path = array();
    if (!empty($form_state['hopform_form']->fid)) {
      $conditions = array('source' => 'form/' . $form_state['hopform_form']->fid);
      $path = path_load($conditions);
      if ($path === FALSE) {
        $path = array();
      }
    }
    $path += array(
      'pid' => NULL,
      'source' => isset($form_state['hopform_form']->fid) ? 'form/' . $form_state['hopform_form']->fid : NULL,
      'alias' => '',
      'language' => LANGUAGE_NONE,
    );

    $form['path'] = array(
      '#type' => 'fieldset',
      '#title' => t('URL path settings'),
      '#collapsible' => TRUE,
      '#collapsed' => empty($path['alias']),
      '#group' => 'additional_settings',
      '#attributes' => array(
        'class' => array('path-form'),
      ),
      '#attached' => array(
        'js' => array(drupal_get_path('module', 'path') . '/path.js'),
      ),
      '#access' => user_access('create url aliases') || user_access('administer url aliases'),
      '#weight' => 30,
      '#tree' => TRUE,
      '#element_validate' => array('path_form_element_validate'),
    );
    $form['path']['alias'] = array(
      '#type' => 'textfield',
      '#title' => t('URL alias'),
      '#default_value' => $path['alias'],
      '#maxlength' => 255,
      '#description' => t('Optionally specify an alternative URL by which this form can be accessed. Use a relative path and don\'t add a trailing slash or the URL alias won\'t work.'),
    );
    $form['path']['pid'] = array('#type' => 'value', '#value' => $path['pid']);
    $form['path']['source'] = array('#type' => 'value', '#value' => $path['source']);
    $form['path']['language'] = array('#type' => 'value', '#value' => $path['language']);
  }

  // Add the buttons.
  $form['actions'] = array('#type' => 'actions');
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
    '#weight' => 5,
  );
  if (!empty($f->fid) && hopform_form_access('delete', $form)) {
    $form['actions']['delete'] = array(
      '#type' => 'submit',
      '#value' => t('Delete'),
      '#weight' => 15,
      '#submit' => array('hopform_form_form_delete_submit'),
    );
  }

  field_attach_form('hopform_form', $f, $form, $form_state);
  return $form;
}

/**
 * Form validate callback. Hopform Form entity.
 */
function hopform_form_form_validate($form, &$form_state) {
  $f = $form_state['hopform_form'];

  field_attach_form_validate('hopform_form', $f, $form, $form_state);
}

/**
 * Form submit callback. Hopform Form entity.
 */
function hopform_form_form_submit($form, &$form_state) {
  $types = hopform_form_types();

  $f = $form_state['hopform_form'];
  $insert = empty($f->fid);

  entity_form_submit_build_entity('hopform_form', $f, $form, $form_state);

  // Save the entity;
  hopform_form_save($f);
  cache_clear_all();

  if ($f->fid) {
    $t_args = array('@type' => $types[$f->type]->label, '%title' => $f->title);
    if ($insert) {
      drupal_set_message(t('@type %title has been created.', $t_args));
    }
    else {
      drupal_set_message(t('@type %title has been updated.', $t_args));
    }

    $form_state['fid'] = $f->fid;
    $form_state['redirect'] = 'form/' . $f->fid;
  }
  else {
    drupal_set_message(t('The form could not be saved.'), 'error');
    $form_state['rebuild'] = TRUE;
  }
}

/**
 * Menu callback. View Hopform Form entity.
 */
function hopform_form_page_view($form) {
  drupal_set_title($form->title);

  $build = hopform_form_view_multiple(array($form->fid => $form), 'full', TRUE);
  $build['form'] = hopform_form_display($form);

  return $build;
}

/**
 * Menu callback. Edit Hopform Form entity.
 */
function hopform_form_page_edit($form) {
  return drupal_get_form('hopform_form_' . $form->type . '_form', $form);
}

/**
 * Menu callback. Edit the form of Hopform Form entity.
 */
function hopform_form_page_form_edit($form) {
  require_once drupal_get_path('module', 'form_builder') . '/includes/form_builder.admin.inc';

  return array(
    form_builder_interface('hopform_' . $form->type, hopform_form_form_id($form)),
    drupal_get_form('hopform_save_form', $form),
  );
}

/**
 * Form callback. Save buttons for editing of the form of Hopform Form entity.
 */
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

/**
 * Form submit callback. Save button for editing of the form of Hopform Form 
 * entity.
 */
function hopform_save_form_submit($form, &$form_state) {
  $f = $form['#hopform'];

  $form_cache = form_builder_cache_load('hopform_' . $f->type, hopform_form_form_id($f));

  $f->form = $form_cache;

  hopform_form_save($f);
  cache_clear_all();

  form_builder_cache_delete('hopform_' . $f->type, hopform_form_form_id($f));

  drupal_set_message(t('Your form has been saved.'));
  $form_state['redirect'] = 'form/' . $f->fid;
}

/**
 * Form submit callback. Cancel button for editing of the form of Hopform Form 
 * entity.
 */
function hopform_save_form_cancel($form, &$form_state) {
  $f = $form['#hopform'];

  form_builder_cache_delete('hopform_' . $f->type, hopform_form_form_id($f));

  drupal_set_message(t('The changes to your form have been discarded.'));
  $form_state['redirect'] = 'form/' . $f->fid;
}

/**
 * Menu callback. Edit the actions for a Hopform Form entity.
 */
function hopform_form_page_actions_edit($form) {
  return drupal_get_form('hopform_actions_form', $form);
}

/**
 * Menu callback. Choose an action type.
 */
function hopform_form_page_actions_action_add_page($form) {
  $actions = hopform_actions();

  $content = array();
  foreach ($actions as $action) {
    $content[] = array(
      'title' => $action['label'],
      'href' => 'form/' . $form->fid . '/actions/add/' . str_replace('_', '-', $action['action']),
      'description' => !empty($action['description']) ? $action['description'] : '',
      'localized_options' => array(),
    );
  }

  // Redirect to a single item, if there's only one.
  if (count($content) == 1) {
    $item = array_shift($content);
    drupal_goto($item['href']);
  }

  return theme('hopform_form_add_list', array('content' => $content));
}

function hopform_form_page_actions_action_edit($form, $action) {
  if ($action->fid != $form->fid) {
    return MENU_NOT_FOUND;
  }

  return drupal_get_form('hopform_form_action_form', $action);
}

function hopform_form_page_actions_action_delete($form, $action) {
  if ($action->fid != $form->fid) {
    return MENU_NOT_FOUND;
  }

  return drupal_get_form('hopform_form_action_delete_confirm', $action);
}

function hopform_form_action_delete_confirm($form, &$form_state, $action) {
  $actions_info = hopform_actions();
  $action_info = $actions_info[$action->type];

  $form_state['hopform_action'] = $action;

  return confirm_form($form,
    t('Are you sure you want to delete the action of type %type?', array('%type' => $action_info['label'])),
    'form/' . $action->fid . '/actions',
    t('This action cannot be undone.'),
    t('Delete'),
    t('Cancel')
  );
}

/**
 * Execute node deletion
 */
function hopform_form_action_delete_confirm_submit($form, &$form_state) {
  $action = $form_state['hopform_action'];

  if ($form_state['values']['confirm']) {
    $actions_info = hopform_actions();
    $action_info = $actions_info[$action->type];

    hopform_form_action_delete($action->aid);

    drupal_set_message(t('Action of type %type has been deleted.', array('%type' => $action_info['label'])));
  }

  $form_state['redirect'] = 'form/' . $action->fid . '/actions';
}

function hopform_form_delete_confirm($form, &$form_state, $f) {
  $form_state['hopform_form'] = $f;

  return confirm_form($form,
    t('Are you sure you want to delete %title?', array('%title' => $f->title)),
    'form/' . $f->fid,
    t('This action cannot be undone.'),
    t('Delete'),
    t('Cancel')
  );
}

function hopform_form_delete_confirm_submit($form, &$form_state) {
  if ($form_state['values']['confirm']) {
    $f = $form_state['hopform_form'];

    hopform_form_delete($f->fid);
    drupal_set_message(t('Form %title has been deleted.', array('%title' => $f->title)));
  }

  $form_state['redirect'] = '<front>';
}

/**
 * Menu callback. Add an action to a form of a certain type.
 */
function hopform_form_page_actions_action_add($form, $action) {
  $actions_info = hopform_actions();
  if (empty($actions_info[$action])) {
    return MENU_NOT_FOUND;
  }

  $action_object = hopform_form_action_create($form, array('type' => $action));
  return drupal_get_form('hopform_form_action_form', $action_object);
}

/**
 * Form calback. Edit the actions for a Hopform Form entity.
 */
function hopform_actions_form($form, &$form_state, $f) {
  $actions = hopform_form_actions($f);
  $actions_info = hopform_actions();

  $form['hopform_actions'] = array(
    '#tree' => TRUE,
    '#theme' => 'hopform_actions_form_actions',
  );
  foreach ($actions as $action) {
    $form['hopform_actions'][$action->aid] = array(
      '#hopform_action' => $action,
      'weight' => array(
        '#type' => 'weight',
        '#default_value' => $action->weight,
        '#title' => t('Weight'),
        '#title_display' => 'invisible',
        '#delta' => 50,
      ),
      'type' => array(
        '#markup' => check_plain($actions_info[$action->type]['label']),
      ),
      'summary' => array(
        '#markup' => hopform_form_action_summary($action),
      ),
      'edit_link' => array(
        '#markup' => l('edit', 'form/' . $f->fid . '/actions/' . $action->aid . '/edit'),
      ),
      'delete_link' => array(
        '#markup' => l('delete', 'form/' . $f->fid . '/actions/' . $action->aid . '/delete'),
      ),
    );
  }

  $form['actions'] = array(
    '#type' => 'actions',
  );
  $form['actions']['save'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
  );

  return $form;
}

function hopform_actions_form_submit($form, &$form_state) {
  $actions = array();

  foreach (element_children($form['hopform_actions']) as $key) {
    if (empty($form['hopform_actions'][$key]['#hopform_action'])) {
      continue;
    }

    $actions[] = array(
      'action' => $form['hopform_actions'][$key]['#hopform_action'],
      'weight' => intval($form_state['values']['hopform_actions'][$key]['weight'])
    );
  }

  usort($actions, 'drupal_sort_weight');
  foreach ($actions as $weight => $info) {
    $info['action']->weight = $weight;
    hopform_form_action_save($info['action']);
  }

  drupal_set_message(t('The changes have been saved.'));
}

function theme_hopform_actions_form_actions($variables) {
  $elements = $variables['elements'];

  $rows = array();
  foreach (element_children($elements) as $key) {
    $elements[$key]['weight']['#attributes']['class'][] = 'action-weight';
    $rows[] = array(
      'data' => array(
        render($elements[$key]['type']),
        render($elements[$key]['summary']),
        render($elements[$key]['edit_link']),
        render($elements[$key]['delete_link']),
        render($elements[$key]['weight']),
      ),
      'class' => array('draggable'),
    );
  }

  $header = array(t('Type'), t('Summary'), array('data' => t('Operations'), 'colspan' => 2), t('Weight'));

  $output = drupal_render_children($elements);
  $output .= theme('table', array('header' => $header, 'rows' => $rows, 'attributes' => array('id' => 'actions-order')));
  drupal_add_tabledrag('actions-order', 'order', 'sibling', 'action-weight', NULL, NULL, TRUE);

  return $output;
}

/**
 * Form callback. Edit action on an form.
 */
function hopform_form_action_form($form, &$form_state, $action) {
  $form_state['hopform_action'] = $action;

  hopform_action_attach_form($action, $form, $form_state);

  // Add the buttons.
  $form['actions'] = array('#type' => 'actions');
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
    '#weight' => 5,
  );
  if (!empty($action->aid)) {
    $form['actions']['delete'] = array(
      '#type' => 'submit',
      '#value' => t('Delete'),
      '#weight' => 15,
      '#submit' => array('hopform_form_action_form_delete_submit'),
    );
  }

  return $form;
}

function hopform_form_action_form_validate($form, &$form_state) {
  $action = $form_state['hopform_action'];

  hopform_action_attach_form_validate($action, $form, $form_state);
}

/**
 * Form submit callback. Edit action on a form.
 */
function hopform_form_action_form_submit($form, &$form_state) {
  $action = $form_state['hopform_action'];

  hopform_action_attach_form_submit($action, $form, $form_state);
  hopform_form_action_save($action);

  $form_state['redirect'] = 'form/' . $action->fid . '/actions';
}

function hopform_form_page_overview() {
  $types = hopform_form_types();

  $query = db_select('hopform_form', 'f')
    ->extend('PagerDefault')
    ->extend('TableSort');

  $header = array(
    'title' => array('data' => t('Title'), 'field' => 'f.title'),
    'type' => array('data' => t('Type'), 'field' => 'f.type'),
    'author' => t('Author'),
    'status' => array('data' => t('Status'), 'field' => 'f.status'),
    'changed' => array('data' => t('Updated'), 'field' => 'f.changed', 'sort' => 'desc'),
    'operations' => array('data' => t('Operations')),
  );

  $fids = $query
    ->fields('f',array('fid'))
    ->limit(50)
    ->orderByHeader($header)
    ->execute()
    ->fetchCol();
  $forms = hopform_form_load_multiple($fids);

  $rows = array();
  $destination = drupal_get_destination();
  foreach ($forms as $form) {
    $rows[$form->fid] = array(
      'title' => array(
        'data' => array(
          '#type' => 'link',
          '#title' => $form->title,
          '#href' => 'form/' . $form->fid,
        ),
      ),
      'type' => check_plain($types[$form->type]->label),
      'author' => theme('username', array('account' => $form)),
      'status' => $form->status ? t('published') : t('not published'),
      'changed' => format_date($form->changed, 'short'),
    );

    $operations = array();
    if (hopform_form_access('update', $form)) {
      $operations['edit'] = array(
        'title' => t('edit'),
        'href' => 'form/' . $form->fid . '/edit',
        'query' => $destination,
      );
    }
    if (hopform_form_access('form_update', $form)) {
      $operations['edit_form'] = array(
        'title' => t('edit form'),
        'href' => 'form/' . $form->fid . '/form',
        'query' => $destination,
      );
    }
    if (hopform_form_access('actions_update', $form)) {
      $operations['edit_actions'] = array(
        'title' => t('edit actions'),
        'href' => 'form/' . $form->fid . '/actions',
        'query' => $destination,
      );
    }
    if (hopform_form_access('delete', $form)) {
      $operations['delete'] = array(
        'title' => t('delete'),
        'href' => 'form/' . $form->fid . '/delete',
        'query' => $destination,
      );
    }

    $rows[$form->fid]['operations'] = array();
    if (count($operations) > 1) {
      $rows[$form->fid]['operations'] = array(
        'data' => array(
          '#theme' => 'links__hopform_form_operations',
          '#links' => $operations,
          '#attributes' => array('class' => array('links', 'inline')),
        ),
      );
    }
    elseif (!empty($operations)) {
      $link = reset($operations);
      $rows[$form->fid]['operations'] = array(
        'data' => array(
          '#type' => 'link',
          '#title' => $link['title'],
          '#href' => $link['href'],
          '#options' => array('query' => $link['query']),
        ),
      );
    }
  }

  $build = array(
    'forms' => array(
      '#theme' => 'table',
      '#header' => $header,
      '#rows' => $rows,
      '#empty' => t('No forms available.'),
    ),
    'pager' => array(
      '#theme' => 'pager',
    ),
  );

  return $build;
}
