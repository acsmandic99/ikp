#!/bin/bash

# Konfiguracija
START_PORT=${1:-52001}  # Prvi pod-agregator (npr. 52001)
BROJ_AGREGATORA=10
KLIJENATA_PO_AGREGATORU=100
PROGRAM="./client"

echo "Pokrećem stress test: $((BROJ_AGREGATORA * KLIJENATA_PO_AGREGATORU)) klijenata..."

for i in $(seq 0 $((BROJ_AGREGATORA - 1)))
do
    TRENUTNI_PORT=$((START_PORT + i))
    echo "Šaljem $KLIJENATA_PO_AGREGATORU klijenata na port $TRENUTNI_PORT..."
    
    for j in $(seq 1 $KLIJENATA_PO_AGREGATORU)
    do

        echo $TRENUTNI_PORT | $PROGRAM > /dev/null 2>&1 &
    done
done

echo "Svi klijenti su pokrenuti. Ukupno: $(pgrep -c client) aktivnih procesa."
echo "Pritisni ENTER da ugasiš sve klijente..."
read

pkill client
echo "Test završen."
