<?php $this->layout('theme::layout/05_page') ?>
<article class="Page">

    <div class="Page__header">
        <div class="outdated">
            This documents an old version of bs::framework.
            <a style="color:white" href="http://docs.bsframework.io/latest/index.html">
                Click here to see the latest release.
            </a>
        </div>
        <h1><?= $page['breadcrumbs'] ? $this->get_breadcrumb_title($page, $base_page) : $page['title'] ?></h1>
        <?php if ($params['html']['date_modified']) { ?>
        <span class="ModifiedDate">
            <?= Todaymade\Daux\FormatDate::format($params, $page['modified_time']) ?>
        </span>
        <?php } ?>
        <?php
        $edit_on = $params->getHTML()->getEditOn();
        if ($edit_on) { ?>
        <span class="EditOn">
            <a href="<?= $edit_on['basepath'] ?>/<?= $page['relative_path'] ?>" target="_blank">
                <?= str_replace(":name:", $edit_on['name'], $this->translate("Edit_on")) ?>
            </a>
        </span>
        <?php } ?>
    </div>

    <div class="s-content">
        <?= $page['content']; ?>
    </div>

    <?php
    $buttons = (!empty($page['prev']) || !empty($page['next']));
    $has_option = array_key_exists('jump_buttons', $params['html']);
    if ($buttons && (($has_option && $params['html']['jump_buttons']) || !$has_option)) {
    ?>
    <nav>
        <ul class="Pager">
            <?php if (!empty($page['prev'])) {
        ?><li class=Pager--prev><a href="<?= $base_url . $page['prev']->getUrl() ?>"><?= $this->translate("Link_previous") ?></a></li><?php

    } ?>
            <?php if (!empty($page['next'])) {
        ?><li class=Pager--next><a href="<?= $base_url . $page['next']->getUrl() ?>"><?= $this->translate("Link_next") ?></a></li><?php

    } ?>
        </ul>
    </nav>
    <?php

} ?>
</article>

