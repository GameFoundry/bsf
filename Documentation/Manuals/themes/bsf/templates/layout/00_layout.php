<!DOCTYPE html>
<html class="no-js" lang="<?=$params['language'] ?>">
<head>
    <title><?= $page['title']; ?> <?= ($page['title'] != $params['title'])? '- ' . $params['title'] : "" ?></title>
<?php //SEO meta tags...
    if (array_key_exists('attributes', $page) && array_key_exists('description', $page['attributes'])) {
        echo "    <meta name=\"description\" content=\"{$page['attributes']['description']}\">\n";
    } elseif (array_key_exists('tagline', $params)) {
        echo "    <meta name=\"description\" content=\"{$params['tagline']}\">\n";
    }
    if (array_key_exists('attributes', $page) && array_key_exists('keywords', $page['attributes'])) {
        echo "    <meta name=\"keywords\" content=\"{$page['attributes']['keywords']}\">\n";
    }
    if (array_key_exists('attributes', $page) && array_key_exists('author', $page['attributes'])) {
        echo "    <meta name=\"author\" content=\"{$page['attributes']['author']}\">\n";
    } elseif (array_key_exists('author', $params)) {
        echo "    <meta name=\"author\" content=\"{$params['author']}\">\n";
    }
?>
    <meta charset="UTF-8">
    <meta http-equiv="X-UA-Compatible" content="IE=edge">

    <link rel="icon" href="<?= $params['theme']['favicon']; ?>" type="image/x-icon">

    <!-- Mobile -->
    <meta name="apple-mobile-web-app-capable" content="yes" />
    <meta name="viewport" content="width=device-width, initial-scale=1.0">

    <!-- Font -->
    <?php foreach ($params['theme']['fonts'] as $font) {
        echo "<link href='$font' rel='stylesheet' type='text/css'>";
    } ?>

    <!-- CSS -->
    <?php foreach ($params['theme']['css'] as $css) {
        echo "<link href='$css' rel='stylesheet' type='text/css'>";
    } ?>

    <?php if ($params['html']['search']) { ?>
        <!-- Tipue Search -->
        <link href="<?= $base_url; ?>tipuesearch/tipuesearch.css" rel="stylesheet">
    <?php } ?>

    <!--[if lt IE 9]>
    <script src="<?= $base_url; ?>themes/daux/js/html5shiv-3.7.3.min.js"></script>
    <![endif]-->
</head>
<body class="<?= $params['html']['float'] ? 'with-float' : ''; ?> <?= $this->section('classes'); ?>">
    <?= $this->section('content'); ?>

    <?php
    if ($params['html']['google_analytics']) {
        $this->insert('theme::partials/google_analytics', ['analytics' => $params['html']['google_analytics'], 'host' => array_key_exists('host', $params) ? $params['host'] : '']);
    }
    if ($params['html']['piwik_analytics']) {
        $this->insert('theme::partials/piwik_analytics', ['url' => $params['html']['piwik_analytics'], 'id' => $params['html']['piwik_analytics_id']]);
    }
    ?>

    <!-- JS -->
    <?php foreach ($params['theme']['js'] as $js) {
        echo '<script src="' . $js . '"></script>';
    } ?>

    <?php if ($params['html']['search']) { ?>
        <script>
            <?php
            $search_strings = [
                "Search_one_result",
                "Search_results",
                "Search_no_results",
                "Search_common_words_ignored",
                "Search_too_short",
                "Search_one_character_or_more",
                "Search_should_be_x_or_more",
                "Link_previous",
                "Link_next",
            ];
            $search_translations = [];
            foreach($search_strings as $key) {
                $search_translations[$key] = $this->translate($key);
            }
            ?>

            window.searchLanguage = <?= json_encode($page['language']) ?>;
            window.searchTranslation = <?= json_encode($search_translations) ?>;
        </script>

        <!-- Tipue Search -->
        <script type="text/javascript" src="<?php echo $base_url; ?>tipuesearch/tipuesearch.js"></script>

        <script>
            window.onunload = function(){}; // force $(document).ready to be called on back/forward navigation in firefox
            $(function() {
                tipuesearch({
                    'base_url': '<?php echo $base_url?>'
                });
            });
        </script>
    <?php } ?>

</body>
</html>
