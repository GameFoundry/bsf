<?php $this->layout('theme::layout/00_layout') ?>
<div class="Columns content">
    <aside class="Columns__left Collapsible">
        <button type="button" class="Button Collapsible__trigger">
            <span class="Collapsible__trigger__bar"></span>
            <span class="Collapsible__trigger__bar"></span>
            <span class="Collapsible__trigger__bar"></span>
        </button>

        <?php $this->insert('theme::partials/navbar_content', ['params' => $params]); ?>

        <div class="Collapsible__content">
            <!-- Navigation -->
            <?php
            $rendertree = $tree;
            $path = '';

            if ($page['language'] !== '') {
                $rendertree = $tree[$page['language']];
                $path = $page['language'];
            }

            echo $this->get_navigation($rendertree, $path, isset($params['request']) ? $params['request'] : '', $base_page, $params['mode']);
            ?>


            <div class="Links">
                <?php if (!empty($params['html']['links'])) { ?>
                    <hr/>
                    <?php foreach ($params['html']['links'] as $name => $url) { ?>
                        <a href="<?= $url ?>" target="_blank"><?= $name ?></a>
                        <br />
                    <?php } ?>
                <?php } ?>
            </div>

            <?php if ($params['html']['toggle_code']) { ?>
                    <div class="CodeToggler">
                        <hr/>
                        <?php if ($params['html']['float']) { ?>
                            <span class="CodeToggler__text"><?=$this->translate("CodeBlocks_title") ?></span>
                            <div class="ButtonGroup" role="group">
                                <button class="Button Button--default Button--small CodeToggler__button CodeToggler__button--hide"><?=$this->translate("CodeBlocks_hide") ?></button>
                                <button class="Button Button--default Button--small CodeToggler__button CodeToggler__button--below"><?=$this->translate("CodeBlocks_below") ?></button>
                                <button class="Button Button--default Button--small CodeToggler__button CodeToggler__button--float"><?=$this->translate("CodeBlocks_inline") ?></button>
                            </div>
                        <?php } else { ?>
                            <label class="Checkbox"><?=$this->translate("CodeBlocks_show") ?>
                                
                                <input type="checkbox" class="CodeToggler__button--main" checked="checked"/>
                                <div class="Checkbox__indicator"></div>
                            </label>
                        <?php } ?>
                    </div>
                <?php } ?>

                <div>
                    <span class="version-select-box">
                        <span class="version-select"></span>
                        Version
                    </span>
                </div>

                <?php if (!empty($params['html']['twitter'])) { ?>
                    <div class="Twitter">
                        <hr/>
                        <?php foreach ($params['html']['twitter'] as $handle) { ?>
                            <iframe allowtransparency="true" frameborder="0" scrolling="no" style="width:162px; height:20px;" src="https://platform.twitter.com/widgets/follow_button.html?screen_name=<?= $handle; ?>&amp;show_count=false"></iframe>
                            <br />
                            <br />
                        <?php } ?>
                    </div>
                <?php } ?>

                <?php if (!empty($params['html']['powered_by'])) { ?>
                    <div class="PoweredBy">
                        <hr/>
                        <?= $params['html']['powered_by'] ?>
                    </div>
                <?php } ?>
        </div>
    </aside>
    <div class="Columns__right <?= $params['html']['float'] ? 'Columns__right--float' : 'Columns__right--full'; ?>">
        <div class="Columns__right__content">
            <div class="doc_content">
                <?= $this->section('content'); ?>
            </div>
        </div>
    </div>
</div>
