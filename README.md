# Guia d'us

### Guia d'us del programa: `generate_marker`:

<li> El programa generarà una única marca ArUco amb un marc d’1 bit d’amplada

Requereix de 4 parametres obligatoris:

1. `-name=` diccionari (alfanumèric)
2. `-id=` id de la marca dins el diccionari (numèric)
3. `-size=` mida en píxels de la marca (numèric)
4. `-file=` nom del fitxer de la imatge PNG de la marca ArUco (alfanumèric)
   

<li> Un exemple de crida per línia de comandes podria ser aquesta:

    $ ./generate_marker -name=DICT_4X4_50 -id=6 -size=200 -file=marker.png

### Guia d'us del programa: `generate_board`:

<li> El programa generarà una matriu de marques ArUco amb un marc d’1 bit
d’amplada per cada marca. </li>

Requereix de 4 parametres obligatoris:

1. `-rows=` files de la matriu ArUco (numèric)
2. `-cols=` columnes de la matriu ArUco (numèric)
3. `-name=` diccionari (alfanumèric)
4. `-pixels=` mida en píxels de la marca (numèric)
5. `-margin=` separació en píxels entre cada marca (numèric)
6. `-output=` nom del fitxer de la imatge PNG de la matriu ArUco (alfanumèric)

<li> Un exemple de crida per línia de comandes podria ser aquesta:

    $ ./generate_board -name=DICT_4X4_50 -rows=4 -cols=2 -pixels=200 -margin=50 -output=board.png

### Guia d'us del programa: `draw_markers`:

<li> El programa ha de ser capaç de reconèixer totes les marques que se li posin
a davant, i sobre el mateix vídeo capturat, sobreimpressionar a cada marca el
següent:

1. El contorn de la marca en un requadre verd
2. La cantonada superior esquerra amb un quadradet vermell
3. L’identificador de cada marca dins del diccionari escollit </li>

Requereix de 1 parametre obligatori:

1. `-name=` diccionari (alfanumèric)

<li> Un exemple de crida per línia de comandes podria ser aquesta:
    
    $ ./draw_markers -name=DICT_4X4_50

### Guia d'us del programa: `calibrate`:

<li> El programa ha de ser capaç de capturar una imatge del video (sense les
sobreimpressions de les deteccions de les marques) cada vegada que es premi la
tecla ’c’.

<li> Cada imatge capturada serà una de les que s’utilitzaran per calibrar la
càmera al final del procés de captura. </li>

<li> El procés de captura d’imatges de calibratge
finalitzarà quan es premi la tecla ESC. </li>

Requereix de 7 parametres obligatoris:

1. `-name=` diccionari (alfanumèric)
2. `-params=` paràmetres del detector (alfanumèric)1
3. `-rows=` files de la matriu ArUco (numèric)
4. `-cols=` columnes de la matriu ArUco (numèric)
5. `-len=` longitud del costat d’una marca ArUco sola, en metres (numèric)
6. `-dist=` distància entre marques ArUco, en metres (numèric)
7. `-output=` nom del fitxer de paràmetres de calibratge en format yaml (alfanumèric)

<li> Un exemple de crida per línia de comandes podria ser aquesta:

    $ ./calibrate -name=DICT_4X4_50 -params=detector_params.yaml -rows=2 -cols=4 -len=0.04 -dist=0.01 -output=fitxerCalibracio.yaml

### Guia d'us del programa: `pose_estimation`:

<li> El programa detectarà la posició i orientació d’una marca ArUco que capturarà
en video.

<li> A sobre de la marca ArUco s’hi sobreimpressionarà un marc que envoltarà
la marca i un requadre vermell petit en una de les seves cantonades. També
se sobreimpressionarà la id de la marca ArUco.

<li> Al mig de la marca ArUco hi dibuixareu un sistema de coordenades consistent
en 3 segments: un de blau, normal al pla de la marca (Z), un de vermell
contingut al mateix pla que la marca (X) i un de verd (Y) a 90º del vermell.

Requereix de 3 parametres obligatoris:

1. `-name=` diccionari (alfanumèric)
2. `-id=` id de la marca dins el diccionari (numèric)
3. `-length=` longitud del costat d’una marca ArUco sola, en metres (numèric)

<li> Un exemple de crida per línia de comandes podria ser aquesta:

    $ ./pose_estimation -name=DICT_4X4_50 -id=6 -length=0.04

### Guia d'us del programa: `draw_cube`:

<li>El cub ha de sobreimpressionar-se sobre el vídeo de la marca ArUco i ha de seguir
la posició i orientació d’aquesta en tot moment. </li>

Requereix de 3 parametres obligatoris:

1. `-name=` diccionari (alfanumèric)
2. `-id=` id de la marca dins el diccionari (numèric)
3. `-length=` longitud del costat d’una marca ArUco sola, en metres (numèric)

<li> Un exemple de crida per línia de comandes podria ser aquesta:

    $ ./draw_cube -name=DICT_4X4_50 -id=6 -length=0.04





