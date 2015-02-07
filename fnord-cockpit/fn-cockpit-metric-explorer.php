<?php

function fn_render_metric_explorer($rpc_url) {
  $base_path = dirname(__FILE__);
?>

<style type="text/css">
  <?php
    include($base_path . "/metric-explorer/fn-metric-explorer.css");
  ?>
</style>

<?php
  include($base_path . "/metric-explorer/metric-explorer-list.html");
  include($base_path . "/metric-explorer/metric-explorer-preview.html");
?>

<div id="metric-explorer-viewport" style="height: 100%;">
</div>

<script type='text/javascript'>
  //var baseUrl = "<?= $rpc_url ?>";
  var baseUrl = "http://s01.nue01.production.fnrd.net:7009/metrics/"
  var views = {
    "metrics" : "<fn-metric-explorer-list></fn-metric-explorer-list>",
    "metric" : "<fn-metric-explorer-preview></fn-metric-explorer-preview>",
    "search" : "<fn-metric-explorer-list></fn-metric-explorer-list>"
  }

  var routes = {
    "metrics": "fn-metric-explorer-list",
    "metric": "fn-metric-explorer-preview",
    "search": "fn-metric-explorer-list"
  };

  var default_route = "metrics";
  function openUrl(raw_url, push_state) {
    var url = Fnord.parseUrlQueryString(raw_url);
    var query_params = url["query_params"];

    var view_name = routes[url["path"]];

    if (view_name == undefined) {
      alert("no route found for: " + url["path"]);
      return;
    }

    if (typeof push_state == "undefined" || push_state === true) {
      window.history.pushState({url: raw_url}, "", "#" + raw_url);
    }

    var viewport = document.querySelector("#metric-explorer-viewport");
    var view = document.createElement(view_name);

    raw_url = "#" + raw_url;
    if (view.getAttribute('data-url') != raw_url) {
      view.setAttribute("data-url", raw_url);
    }
    viewport.innerHTML = "";
    viewport.appendChild(view);

  };

  function setUrlHash(url_hash) {
    window.history.pushState({url: url_hash}, "#", url_hash);
    window.location.hash = url_hash;
  }

  window.addEventListener('fn-ready' ,function() {
    var fragment = window.location.hash;
    if (fragment) {
      openUrl(fragment.substring(1));
    } else {
      openUrl(default_route);
    }

  }, false)
</script>

<?php
  }
?>


