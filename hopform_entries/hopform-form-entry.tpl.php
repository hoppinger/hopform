<div class="hopform-form-entry">
  <dl>
    <dt><?php print t('Hostname'); ?></dt>
    <dd><?php print check_plain($entry->hostname); ?></dd>

    <dt><?php print t('Created'); ?></dt>
    <dd><?php print format_date($entry->created, 'long'); ?></dd>
  </dl>

  <?php print render($content); ?>
</div>