cmd_Makefile := cd ..; /usr/share/nodejs/node-gyp/gyp/gyp_main.py -fmake --ignore-environment "-Dlibrary=shared_library" "-Dvisibility=default" "-Dnode_root_dir=/usr/include/nodejs" "-Dnode_gyp_dir=/usr/share/nodejs/node-gyp" "-Dnode_lib_file=/usr/include/nodejs/<(target_arch)/node.lib" "-Dmodule_root_dir=/home/luca/Workspace/Node/Node-API/serial_addon" "-Dnode_engine=v8" "--depth=." "-Goutput_dir=." "--generator-output=build" -I/home/luca/Workspace/Node/Node-API/serial_addon/build/config.gypi -I/usr/share/nodejs/node-gyp/addon.gypi -I/usr/include/nodejs/common.gypi "--toplevel-dir=." binding.gyp