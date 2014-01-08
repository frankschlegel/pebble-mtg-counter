/*
 * Copyright (c) 2013 Bert Freudenberg
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 *
 * Original from https://github.com/bertfreudenberg/PebbleONE.
 * Modified for pebble-mtg-counter.
 *
 */

// the config dict is sent as app message to the watch
var config = {
    "life_default": 20,
    "match_duration": 50,
    "match_end_vibration": 1,
    "before_match_end_vibration": 0,
    "before_match_end_time": 10,
    "show_timer": 1,
    "rotation_lock": 0,
    "invert_colors": 0,
};
var allowed_config_keys = Object.keys(config);

// config_html will be assigned the contents of config.html during build (See /wscript).
var config_html; 

function send_config_to_pebble() {
    console.log("sending config " + JSON.stringify(config)); 
    Pebble.sendAppMessage(config,
        function ack(e) { console.log("Successfully delivered message " + JSON.stringify(e.data)); },
        function nack(e) { console.log("Unable to deliver message " + JSON.stringify(e)); });
}

// read config from persistent storage
Pebble.addEventListener('ready', function () {
    var stored_config = window.localStorage.getItem('config');
    if (typeof stored_config !== 'undefined' && stored_config !== null) {
        config = stored_config;
        console.log("loaded config " + JSON.stringify(stored_config));
    }
});

// got message from pebble
Pebble.addEventListener('appmessage', function(e) {
    console.log("got message " + JSON.stringify(e.payload));
    if (e.payload.request_config) {
        send_config_to_pebble();
    }
});

// open config window
Pebble.addEventListener('showConfiguration', function () {
    var html = config_html.replace('"CONFIG_JSON_REPLACED_AT_RUNTIME"', JSON.stringify(config), 'g');
    Pebble.openURL('data:text/html,' + encodeURIComponent(html + '<!--.html'));
});

// store config and send to watch
Pebble.addEventListener('webviewclosed', function (e) {
    if (e.response && e.response.length) {
        var returned_config = JSON.parse(e.response);

        // strip keys that are not allowed. else sending would fail!
        var stripped_config = {};
        for (var i = allowed_config_keys.length - 1; i >= 0; i--) {
            var key = allowed_config_keys[i];
            var value = returned_config[key];
            if (typeof value !== 'undefined') {
                stripped_config[key] = value;
            }
        }

        // store
        config = stripped_config;
        console.log("storing config " + JSON.stringify(config));
        window.localStorage.setItem('config', config);
        // send
        send_config_to_pebble();
    }
});
