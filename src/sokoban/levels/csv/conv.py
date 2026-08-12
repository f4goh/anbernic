import csv
import sys
from pathlib import Path


def csv_to_c(csv_file, output_file, array_name="LEVEL_01"):
    rows = []

    # Lecture du CSV
    with open(csv_file, "r", encoding="utf-8") as f:
        reader = csv.reader(f)

        for row in reader:
            if not row:
                continue

            values = [int(value.strip()) for value in row]

            # Supprime les lignes entièrement composées de -1
            if all(value == -1 for value in values):
                continue

            # Remplace -1 par 255
            values = [255 if value == -1 else value for value in values]

            rows.append(values)

    if not rows:
        print("Aucune donnée à générer.")
        return

    # Vérifie que toutes les lignes ont la même taille
    width = len(rows[0])

    for i, row in enumerate(rows):
        if len(row) != width:
            raise ValueError(
                f"La ligne {i + 1} contient {len(row)} valeurs "
                f"au lieu de {width}."
            )

    height = len(rows)

    # Génération du fichier C
    with open(output_file, "w", encoding="utf-8") as f:
        f.write("#include <stdint.h>\n\n")

        f.write(f"const uint8_t {array_name}[{height}][{width}] =\n")
        f.write("{\n")

        for i, row in enumerate(rows):
            values = ",".join(f"{value:3}" for value in row)

            # Une seule ligne par ligne du CSV
            f.write(f"    {{{values}}}")

            if i < height - 1:
                f.write(",")

            f.write("\n")

        f.write("};\n")

    print(f"Fichier généré : {output_file}")
    print(f"Dimensions : {height} x {width}")


if __name__ == "__main__":
    if len(sys.argv) < 3:
        print("Utilisation :")
        print("    python conv.py fichier.csv fichier.c")
        print("Pas plus de 15 lignes par 20")
        print("Exemple :")
        print("    python conv.py level01.csv level01.c")
        sys.exit(1)

    csv_file = sys.argv[1]
    output_file = sys.argv[2]

    # Nom du tableau optionnel
    array_name = sys.argv[3] if len(sys.argv) >= 4 else "LEVEL_01"

    csv_to_c(csv_file, output_file, array_name)



