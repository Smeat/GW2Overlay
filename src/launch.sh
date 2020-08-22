#!/bin/bash

#GW2_PID=$(pidof GW2-64.exe)
OVERLAY_CMD=$@

echo -n "Waiting for GW2 to start"
while [ -z ${GW2_PID} ]
do
	echo -n "."
	sleep 2
	UI_PIDS=($(pidof CoherentUI_Host))
	RUNNING=${#UI_PIDS[@]}
	if [ -z ${RUNNING} ]; then
		RUNNING=0
	fi
	for PID in "${UI_PIDS[@]}"
	do
		OUTPUT=$(cat /proc/${PID}/cmdline |  tr '\0' '\n')
		if [[ "${OUTPUT}" == *"type=renderer"* ]]; then
			RUNNING=0
		fi
	done
	if [ $RUNNING -eq 1 ]; then
		GW2_PID=$(pidof GW2-64.exe)
	fi
done
sleep 5
echo ""
echo "Found gw2 process ${GW2_PID}"

OLD_ENV=$(export -p)

source <(xargs -0 bash -c 'printf "export %q\n" "$@"' -- < /proc/${GW2_PID}/environ)
unset WINESERVERSOCKET
unset WINELOADERNOEXEC
unset WINEPRELOADRESERVE
unset LD_PRELOAD

echo ${WINEPREFIX}
PYTHON_BIN="${WINEPREFIX}/drive_c/Program Files/Python38/python.exe"
"${WINE}" "${PYTHON_BIN}" "${WINEPREFIX}/mumble.py" &
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

