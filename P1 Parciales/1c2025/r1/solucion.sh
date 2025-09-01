#!/bin/bash

ZIPFILE="data.zip"

if [[ ! -f "$ZIPFILE" ]]; then
    echo "❌ No se encontró $ZIPFILE"
    exit 1
fi

echo "Elige una opción:"
echo "1) enunciado"
echo "2) solucion"
read -rp "Opción: " opcion

case "$opcion" in
    1)
        echo "📦 Sobrescribiendo con 'enunciado/src'..."
        unzip -o "$ZIPFILE" "enunciado/src/*" -d .
        rsync -a enunciado/src/ src/
        rm -rf enunciado
        ;;
    2)
        echo "📦 Sobrescribiendo con 'solucion/src'..."
        unzip -o "$ZIPFILE" "solucion/src/*" -d .
        rsync -a solucion/src/ src/
        rm -rf solucion
        ;;
    *)
        echo "❌ Opción inválida"
        ;;
esac

