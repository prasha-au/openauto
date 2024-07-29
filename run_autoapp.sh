#!/usr/bin/env sh

cd "${0%/*}"

run_autoapp() {
    until ./bin/autoapp; do
        echo "Crashed with exit code $?.  Respawning.." >&2
    done
}

run_swc()  {
    until python ./swc/swc.py; do
        echo "Crashed with exit code $?.  Respawning.." >&2
        sleep 1
    done
}

(trap 'kill 0' INT; \
    run_swc > run_swc.log 2>&1 & \
    run_autoapp  > run_autoapp.log 2>&1 \
)

