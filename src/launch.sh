#!/bin/bash

SCRIPT_DIR=$(dirname "$(readlink -f "$0")")

GW2_PID=$(pidof GW2-64.exe)
OVERLAY_CMD=$@

echo -n "Waiting for GW2 to start"
while [ -z ${GW2_PID} ]
do
	echo -n "."
	sleep 2
	GW2_PID=$(pidof GW2-64.exe)
done
OLD_ENV=$(export -p)

source <(xargs -0 bash -c 'printf "export %q\n" "$@"' -- < /proc/${GW2_PID}/environ)
unset WINESERVERSOCKET
unset WINELOADERNOEXEC
unset WINEPRELOADRESERVE
unset LD_PRELOAD

echo ${WINEPREFIX}
PYTHON_BIN="${WINEPREFIX}/drive_c/Program Files/Python38/python.exe"
#"${WINE}" "${SCRIPT_DIR}/helper/mumble.exe" &
"${WINE}" "${PYTHON_BIN}" "${SCRIPT_DIR}/helper/mumble.py" &
MUMBLE_PID=$!

$OVERLAY_CMD &
OVERLAY_PID=$!

function kill_children() {
	kill ${OVERLAY_PID}
	kill ${MUMBLE_PID}
}

trap kill_children SIGTERM SIGINT

tail --pid=${GW2_PID} -f /dev/null

kill_children

