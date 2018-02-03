#!/vendor/bin/sh
modconnection=$(getprop sys.mod.connected 2> /dev/null)

if [ "$modcopnnection" == "1" ] || [ "$modconnection" = "0" ]; then
    killall com.motorola.modservice
    return 0
fi

