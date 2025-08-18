[platformio]
default_envs = tx

[env:tx]
platform = raspberrypi
board = pico
framework = arduino
build_src_filter = +<tx/main.cpp>
monitor_speed = 115200
upload_protocol = picotool

[env:rx]
platform = raspberrypi
board = pico
framework = arduino
build_src_filter = +<rx/main.cpp>
monitor_speed = 115200
upload_protocol = picotool
