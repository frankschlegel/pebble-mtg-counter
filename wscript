#
# waf build script for the pebble app.
# https://code.google.com/p/waf/
#
# The build porcess has been customized
# to embed the HTML configuration UI into the javascript.
# https://github.com/bertfreudenberg/PebbleONE
#

top = '.'
out = 'build'

def options(context):
    context.load('pebble_sdk')

def configure(context):
    context.load('pebble_sdk')

def build(context):
    context.load('pebble_sdk')

    context.pbl_program(
        source=context.path.ant_glob('src/**/*.c'),
        target='pebble-app.elf'
    )
    source_js_file = 'src/mtg-counter.js'
    config_html_js_var = 'config_html'
    config_html_file = 'src/config.html'

    # generate config.js from config.html by escaping every line and quotes
    config_html_node = context.path.make_node(config_html_file)
    config_js_node  = context.path.get_bld().make_node('src/js/config.js')
    context(rule='(echo '+config_html_js_var+'= && sed "s/\'/\\\\\\\'/g;s/^/\\\'/;s/$/\\\' +/" ${SRC} && echo "\'\';") > ${TGT}', source=config_html_node, target=config_js_node)

    # make pebble-js-app.js by appending config.js to the source javascript file
    source_js_node = context.path.make_node(source_js_file)
    build_js_node  = context.path.get_bld().make_node('src/js/pebble-js-app.js')
    context(rule='cat ${SRC} > ${TGT}', source=[source_js_node, config_js_node], target=build_js_node)

    # bundle the app with the native program and the concatenated javascript
    context.pbl_bundle(elf='pebble-app.elf', js=build_js_node)
