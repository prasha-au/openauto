#!/usr/bin/env sh

cd "${0%/*}"

run_autoapp() {
    until ./bin/autoapp --platform eglfs; do
        echo "Crashed with exit code $?.  Respawning.." >&2
    done
}

(trap 'kill 0' INT; \
    run_autoapp  > run_autoapp.log 2>&1 \
)

