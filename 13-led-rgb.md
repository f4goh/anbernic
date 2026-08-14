# RG40XX H — Commande RGB des joysticks avec MuOS

## 1. La commande SH qui fonctionne

La commande utilisée par MuOS est :

    /opt/muos/script/device/rgb.sh -b serial 1 255 255 0 0 0 0 255

Elle permet de commander les LEDs RGB des deux joysticks de la RG40XX H via le MCU.

Décomposition :

    /opt/muos/script/device/rgb.sh

C'est le script officiel de MuOS qui gère les LEDs RGB.

    -b serial

Force l'utilisation du backend série.

Le script possède trois backends :

    auto
    sysfs
    serial

Ici on utilise explicitement :

    serial

qui correspond à :

    /dev/ttyS5

    1

C'est le mode LED utilisé par le MCU.

Pour le protocole série :

    1 = mode STATIC / SOLID

    255

C'est la luminosité.

La luminosité est comprise entre :

    0   = éteint
    255 = maximum


Les six valeurs suivantes représentent les couleurs :

    255 0 0
    0   0 255

Dans l'ordre utilisé par MuOS :

    RIGHT_R RIGHT_G RIGHT_B
    LEFT_R  LEFT_G  LEFT_B


Donc :

    255 0 0

signifie :

    joystick DROIT = rouge


et :

    0 0 255

signifie :

    joystick GAUCHE = bleu


La commande complète :

    /opt/muos/script/device/rgb.sh -b serial 1 255 255 0 0 0 0 255

signifie donc :

    mode       = 1 (solid)
    luminosité = 255
    droite     = RGB(255, 0, 0)   rouge
    gauche     = RGB(0, 0, 255)   bleu


------------------------------------------------------------
# 2. Éteindre les LEDs avec le script MuOS
------------------------------------------------------------

La commande :

    /opt/muos/script/device/rgb.sh -b serial 1 0 0 0 0 0 0 0

signifie :

    mode       = 1
    luminosité = 0
    droite     = noir
    gauche     = noir

Résultat :

    LEDs éteintes


------------------------------------------------------------
# 3. Le programme C avec argc / argv
------------------------------------------------------------

Le programme C en exemple permet d'utiliser directement :

    ./ledrgb-arm64


Il accepte plusieurs formes de commandes.


------------------------------------------------------------
## 3.1 Même couleur sur les deux joysticks
------------------------------------------------------------

Syntaxe :

    ./ledrgb-arm64 R G B BRIGHTNESS

Exemple :

    ./ledrgb-arm64 255 0 0 255

Résultat :

    DROITE = rouge
    GAUCHE = rouge
    luminosité = 255


Exemple :

    ./ledrgb-arm64 0 255 0 128

Résultat :

    DROITE = vert
    GAUCHE = vert
    luminosité = 128


Exemple :

    ./ledrgb-arm64 0 0 255 50

Résultat :

    DROITE = bleu
    GAUCHE = bleu
    luminosité = 50


Les valeurs RGB sont comprises entre :

    0 et 255

La luminosité est également comprise entre :

    0 et 255


------------------------------------------------------------
## 3.2 Couleur différente pour chaque joystick
------------------------------------------------------------

Syntaxe :

    ./ledrgb-arm64 RR RG RB LR LG LB BRIGHTNESS

Signification :

    RR = rouge joystick DROIT
    RG = vert  joystick DROIT
    RB = bleu  joystick DROIT

    LR = rouge joystick GAUCHE
    LG = vert  joystick GAUCHE
    LB = bleu  joystick GAUCHE

    BRIGHTNESS = luminosité


Exemple :

    ./ledrgb-arm64 255 0 0 0 0 255 255

Résultat :

    DROITE = rouge
    GAUCHE = bleu
    luminosité = 255


Exemple :

    ./ledrgb-arm64 0 255 0 255 0 255 200

Résultat :

    DROITE = vert
    GAUCHE = violet
    luminosité = 200


Exemple :

    ./ledrgb-arm64 255 255 255 255 255 255 100

Résultat :

    DROITE = blanc
    GAUCHE = blanc
    luminosité = 100


------------------------------------------------------------
## 3.3 Éteindre
------------------------------------------------------------

Commande :

    ./ledrgb-arm64 off

C'est l'équivalent logique de :

    /opt/muos/script/device/rgb.sh -b serial 1 0 0 0 0 0 0 0


------------------------------------------------------------
# 4. Que sont argc et argv ?
------------------------------------------------------------

Dans un programme C :

    int main(int argc, char **argv)

argc contient le nombre d'arguments.

argv contient les arguments sous forme de chaînes de caractères.


Par exemple :

    ./ledrgb-arm64 255 0 0 128

Le shell transmet au programme :

    argv[0] = "./ledrgb-arm64"
    argv[1] = "255"
    argv[2] = "0"
    argv[3] = "0"
    argv[4] = "128"

Et :

    argc = 5


Attention :

argv contient toujours des chaînes de caractères.

Par exemple :

    argv[1]

contient :

    "255"

et pas directement le nombre 255.

Le programme utilise donc :

    atoi(argv[1])

pour convertir la chaîne en entier.


------------------------------------------------------------
# 5. Architecture du fonctionnement
------------------------------------------------------------

Quand on exécute :

    ./ledrgb-arm64 255 0 0 0 0 255 255

le fonctionnement est :

    argc / argv
          |
          v
    lecture des paramètres
          |
          v
    rgb_joysticks()
          |
          v
    construction du paquet
          |
          v
    calcul du checksum
          |
          v
    ouverture de /dev/ttyS5
          |
          v
    configuration UART 115200
          |
          v
    write()
          |
          v
    MCU de la RG40XX H
          |
          v
    LEDs RGB


------------------------------------------------------------
# 6. Protocole série utilisé par le MCU
------------------------------------------------------------

Le protocole que nous avons confirmé avec le script MuOS utilise :

    /dev/ttyS5

Paramètres série :

    Baudrate : 115200
    Data     : 8 bits
    Parité   : aucune
    Stop     : 1
    Flow     : aucun

Donc :

    115200 8N1


------------------------------------------------------------
# 7. Structure du paquet
------------------------------------------------------------

Pour le mode 1, le paquet fait exactement :

    51 octets

Structure :

    +--------+----------------+
    | Octet  | Fonction       |
    +--------+----------------+
    | 0      | Mode           |
    | 1      | Luminosité     |
    | 2-25   | 8 x RGB DROIT  |
    | 26-49  | 8 x RGB GAUCHE |
    | 50     | Checksum       |
    +--------+----------------+


Soit :

    1 octet
    + 1 octet
    + 24 octets
    + 24 octets
    + 1 octet
    = 51 octets


------------------------------------------------------------
# 8. Octet 0 : mode
------------------------------------------------------------

Le premier octet indique le mode.

Pour notre commande :

    01

signifie :

    mode SOLID / STATIC


MuOS documente également d'autres modes série :

    1 = solid
    2 = breath fast
    3 = breath medium
    4 = breath slow
    5 = mono rainbow
    6 = multi rainbow


------------------------------------------------------------
# 9. Octet 1 : luminosité
------------------------------------------------------------

Le deuxième octet correspond à la luminosité :

    00 = 0%
    FF = 100%

Exemple :

    01 FF

signifie :

    mode solid
    luminosité maximale


Exemple :

    01 80

signifie approximativement :

    mode solid
    luminosité 128/255


------------------------------------------------------------
# 10. LEDs du joystick DROIT
------------------------------------------------------------

Les octets 2 à 25 contiennent :

    8 x RGB


Chaque LED utilise trois octets :

    R G B


Par exemple :

    FF 00 00

signifie :

    rouge


Les 8 LEDs sont répétées :

    FF 00 00
    FF 00 00
    FF 00 00
    FF 00 00
    FF 00 00
    FF 00 00
    FF 00 00
    FF 00 00


Donc les 24 octets correspondent aux 8 LEDs du joystick droit.


------------------------------------------------------------
# 11. LEDs du joystick GAUCHE
------------------------------------------------------------

Les octets 26 à 49 contiennent également :

    8 x RGB


Par exemple :

    00 00 FF

signifie :

    bleu


Pour mettre les 8 LEDs gauches en bleu :

    00 00 FF
    00 00 FF
    00 00 FF
    00 00 FF
    00 00 FF
    00 00 FF
    00 00 FF
    00 00 FF


------------------------------------------------------------
# 12. Exemple complet
------------------------------------------------------------

Pour :

    droite = rouge
    gauche = bleu
    luminosité = 255

le début du paquet est :

    01 FF

Puis les 8 LEDs DROITES :

    FF 00 00
    FF 00 00
    FF 00 00
    FF 00 00
    FF 00 00
    FF 00 00
    FF 00 00
    FF 00 00

Puis les 8 LEDs GAUCHES :

    00 00 FF
    00 00 FF
    00 00 FF
    00 00 FF
    00 00 FF
    00 00 FF
    00 00 FF
    00 00 FF

Puis :

    CHECKSUM


Le paquet commence donc par :

    01 FF FF 00 00 FF 00 00 FF ...


et se termine dans notre exemple fonctionnel par :

    ... FF F0


------------------------------------------------------------
# 13. Le checksum
------------------------------------------------------------

Le dernier octet permet au MCU de vérifier le paquet.

Le checksum utilisé par MuOS est simplement :

    somme de tous les octets précédents
    modulo 256


En C :

    static uint8_t rgb_checksum(const uint8_t *data, size_t len)
    {
        unsigned int sum = 0;

        for (size_t i = 0; i < len; i++)
            sum += data[i];

        return (uint8_t)(sum & 0xff);
    }


Le checksum est calculé sur :

    octet 0
    octet 1
    ...
    octet 49

mais PAS sur le checksum lui-même.


Il est ensuite placé dans :

    octet 50


------------------------------------------------------------
# 14. Exemple de calcul du checksum
------------------------------------------------------------

Le paquet contient 50 octets avant le checksum.

On calcule :

    checksum =
        (octet0
        + octet1
        + ...
        + octet49) & 0xFF


Le :

    & 0xFF

permet de conserver uniquement les 8 bits de poids faible.


------------------------------------------------------------
# 15. Pourquoi notre premier programme C ne fonctionnait pas
------------------------------------------------------------

Le premier code contenait :

    packet[p++] = checksum(packet, p);


C'était une mauvaise écriture.

Le problème venait du fait que l'incrémentation de p et l'utilisation de p dans l'appel de fonction pouvaient produire un comportement incorrect.

La bonne version est :

    uint8_t chk = checksum(packet, p);
    packet[p++] = chk;


Il faut donc toujours :

    1. calculer le checksum
    2. ajouter le checksum au paquet


------------------------------------------------------------
# 16. Fonction C réutilisable
------------------------------------------------------------

La fonction importante pour les futurs programmes est :

    int rgb_joysticks(
        uint8_t right_r,
        uint8_t right_g,
        uint8_t right_b,

        uint8_t left_r,
        uint8_t left_g,
        uint8_t left_b,

        uint8_t brightness
    );


Exemple :

    rgb_joysticks(
        255, 0, 0,
        0, 0, 255,
        255
    );


Cela donne :

    joystick DROIT  = rouge
    joystick GAUCHE = bleu
    luminosité      = 255


Autre exemple :

    rgb_joysticks(
        255, 128, 0,
        0, 255, 255,
        180
    );


Cela donne :

    DROITE  = orange
    GAUCHE  = cyan
    luminosité = 180


------------------------------------------------------------
# 17. Fonctions pratiques
------------------------------------------------------------

On peut également utiliser :

    rgb_off();


pour éteindre les LEDs.


Et :

    rgb_all(255, 0, 0, 255);


pour mettre les deux joysticks en rouge.


Exemple :

    rgb_all(0, 255, 0, 128);

donne :

    deux joysticks verts
    luminosité 128


------------------------------------------------------------
# 18. Résumé
------------------------------------------------------------

Commande MuOS :

    /opt/muos/script/device/rgb.sh -b serial 1 255 255 0 0 0 0 255

signifie :

    backend série
    |
    +-- /dev/ttyS5
    |
    +-- mode 1
    |
    +-- luminosité 255
    |
    +-- droite = RGB(255,0,0)
    |
    +-- gauche = RGB(0,0,255)


Le C fait exactement la même chose mais directement :

    open("/dev/ttyS5")
          |
          v
    UART 115200 8N1
          |
          v
    construction paquet 51 octets
          |
          v
    checksum
          |
          v
    write()
          |
          v
    MCU
          |
          v
    LEDs


Le protocole confirmé pour le mode 1 est donc :

    [MODE]
    [BRIGHTNESS]
    [8 x RGB RIGHT]
    [8 x RGB LEFT]
    [CHECKSUM]


Total :

    51 octets


Le point important pour les futurs programmes est que la communication avec les LEDs
ne passe pas par /sys/class/leds/ sur cette RG40XX H.

Elle passe par :

    /dev/ttyS5

avec le protocole série du MCU.

