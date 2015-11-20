<?php if (!empty($elements['#children'])): ?>
  <div class="hopform-form-entry-data">
    <?php if (!empty($elements['#label'])): ?>
      <p class="hopform-form-entry-data-label"><strong><?php print check_plain($elements['#label']); ?></strong></p>
    <?php endif; ?>
    <?php if (!empty($elements['#value'])): ?>
      <p class="hopform-form-entry-data-value"><?php print check_plain($elements['#value']); ?></p>
    <?php endif; ?>

    <?php print $elements['#children']; ?>
  </div>
<?php else: ?>
  <p class="hopform-form-entry-data">
    <?php print !empty($elements['#label']) ? ('<strong>' . check_plain($elements['#label'] . ': ') . '</strong>') : ''; ?>

    <?php if (!empty($elements['#url'])): ?>
      <a href="<?php print check_plain($elements['#url']); ?>">
        <?php print !empty($elements['#value']) ? check_plain($elements['#value']) : ''; ?>
      </a>
    <?php else: ?>
      <?php print !empty($elements['#value']) ? check_plain($elements['#value']) : ''; ?>
    <?php endif; ?>
  </p>
<?php endif; ?>