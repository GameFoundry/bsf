<?php $this->layout('theme::layout/00_layout') ?>

<?php $this->start('classes') ?>homepage<?php $this->stop() ?>

<div class="Navbar NoPrint">
    <div class="Container">
        <?php $this->insert('theme::partials/navbar_content', ['params' => $params]); ?>
    </div>
</div>

<div class="Homepage">
    <div class="HomepageTitle Container">
        <?= ($params['tagline'])? '<h2>' . $params['tagline'] . '</h2>' : '' ?>
    </div>

    <div class="HomepageImage Container">
        <?= ($params['image'])? '<img class="homepage-image img-responsive" src="' . $params['image'] . '" alt="' . $params['title'] . '">' : '' ?>
    </div>

    <div class="HomepageButtons">
        <div class="Container">
            <?php
            if ($params['html']['repo']) {
                echo '<a href="https://github.com/' . $params['html']['repo'] . '" class="Button Button--secondary Button--hero">' . $this->translate("View_on_github") . '</a>';
            }
            $view_doc = $this->translate("View_documentation");
            foreach ($page['entry_page'] as $key => $node) {
                echo '<a href="' . $node . '" class="Button Button--primary Button--hero">' . str_replace("__VIEW_DOCUMENTATION__", $view_doc, $key) . '</a>';
            }
            if(isset($params['html']['buttons']) && is_array($params['html']['buttons'])) {
                foreach ($params['html']['buttons'] as $name => $link ) {
                    echo '<a href="' . $link . '" class="Button Button--secondary Button--hero">' . $name . '</a>';
                }
            }
            ?>
        </div>
    </div>
</div>

<div class="HomepageContent">
    <div class="Container">
        <div class="Container--inner">
            <div class="doc_content s-content">
                <?= $page['content']; ?>
            </div>
        </div>
    </div>
</div>

<div class="HomepageFooter">
    <div class="Container">
        <div class="Container--inner">
            <?php if (!empty($params['html']['links'])) { ?>
                <ul class="HomepageFooter__links">
                    <?php foreach ($params['html']['links'] as $name => $url) {
                    echo '<li><a href="' . $url . '" target="_blank">' . $name . '</a></li>';
                } ?>
                </ul>
            <?php } ?>

            <?php if (!empty($params['html']['twitter'])) { ?>
                <div class="HomepageFooter__twitter">
                    <?php foreach ($params['html']['twitter'] as $handle) { ?>
                    <div class="Twitter">
                        <iframe allowtransparency="true" frameborder="0" scrolling="no" style="width:162px; height:20px;" src="https://platform.twitter.com/widgets/follow_button.html?screen_name=<?= $handle; ?>&amp;show_count=false"></iframe>
                    </div>
                    <?php } ?>
                </div>
            <?php } ?>
        </div>
    </div>
</div>
