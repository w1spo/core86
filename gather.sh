#!/bin/bash
# scal wszystkie .c i .h w jeden plik z komentarzami nazwy plików

OUTPUT="All_in_one.txt"
rm -f "$OUTPUT"

# znajdź wszystkie pliki .c i .h w katalogu Kernel/src i Mandatory (możesz dodać inne foldery)
FILES=$(find Kernel/src Mandatory -type f \( -name "*.c" -o -name "*.h" \) | sort)

for f in $FILES; do
    echo "// $f" >> "$OUTPUT"
    cat "$f" >> "$OUTPUT"
    echo -e "\n" >> "$OUTPUT"
done

echo "✅ Wszystkie pliki scalone do $OUTPUT"
