#!/bin/sh

LED_GPIO=/sys/class/gpio/gpio440

if test -d $LED_GPIO; then
    echo "GPIO440 already exported"
else
    echo 440 > /sys/class/gpio/export
fi

echo out > $LED_GPIO/direction

while true; do
    echo 0 > $LED_GPIO/value
    sleep 0.5
    echo 1 > $LED_GPIO/value
    sleep 0.5
done

