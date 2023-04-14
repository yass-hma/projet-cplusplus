SVG pilotable par le réseau
===========================

Ce projet permet de manipuler des images svg chargées sur un serveur, et fournit un simple client pour envoyer des 
requêtes de modifications de l'image simplement.

I - Compilation
===============

liste des dépendances:
* gtk+-3.0
* cairo 
* librsvg2
* tinyxml2
* cbor
----------------------
compilation et lancement
---------------------
placez vous dans le dossier contenant le fichier Makefile (racine du dépôt) et lancez les commandes suivantes :

   ```shell script
  make svgclient
  make svgserver
  ```
    
Les executables générés sont placés a la racine du projet.
L'executable du serveur s'appelle **svgserver** et 
L'executable d'un client de base en CLI s'appelle **svgclient**



II - Creation d'une image svg pilotable
=======================================

Tout d'abord, il faut une image svg normale. Vous pouvez la creer avec un logiciel comme inkscape ou bien à la
main.

Ensuite pour rendre certains attributs pilotables on va utiliser la balise `driven`, qui ne fait pas partie de la norme 
svg. La balise `driven` contient deux attributs au minimum :
* `target`, qui doit contenir le nom de l’attribut dont on veut contrôler la valeur. Cet attribut est pris parmi les 
  attribut de la balise parente de la balise `driven` que l'on est en train de définir.
* `by`, qui est le nom de l’entrée utilisée pour contrôler cet attribut (on l'appelera **nom du driven** tout au long de 
  ce document).

**Exemple :**
```svg
<circle cx="50" cy="25" r="20" fill="yellow">
    <driven target="cx" by="sun_x"/>
    <driven target="cy" by="sun_y"/>
</circle>
```
Cela crée un cercle de couleur jaune dont la position est modifiable avec les driven portant les noms `sun_x` pour 
manipuler l'attribut `cx` et `sun_y` pour manipuler l'attribut `cy`.


##### valeurs possibles pour l'attribut type :
 
| valeur                     | condition de validation de la valeur
|---------------------------:|:--------------------------------------------------------------------------------------------------
| int                        | Doit être un entier.
| float                      | Doit être un float.
| color                      | Doit être une couleur sous forme hexadecimale. (ex: #FFFFFF, #f6a, ...)
| string                     | Doit être une chaine de caractères.


**Exemple :**
```svg
<circle cx="250" cy="125" r="100" stroke="yellow" opacity="0.5" fill="orange" stroke-width="5">
    <driven target="cx" by="sun_x" type="float"/>
    <driven target="fill" by="color_fill" type="color"/>
    <driven target="opacity" by="sun_op" type="float"/>
</circle>
```
Lors de la réception d'une nouvelle valeur par le serveur, l'image
est mise a jour uniquement si elle respecte la condition imposée
par l'attribut type, sinon la valeur reçue est ignorée.

### 2 - Transitions
Une transition permet à un attribut de passer d'une valeur a une autre de façon fluide.

Pour indiquer un délai de transition lors de la mise a jour d'un attribut piloté,
il suffit de rajouter l'attribut `delay="val"` a la balise driven concerné, val 
doit être une durée en milisecondes.
**Exemple :**
```svg
    <driven target="fill" by="color_fill" type="color" delay="1000"/>
    <driven target="opacity" by="sun_op" type="float" delay="1000" />

```
  
### 3 - Attributs composites

Un attribut composite est un attribut qui se compose ou peux se composer de plusieurs valeurs que l'on peux distinguer 
les unes des autres. On a notemment les attributs **style**.

Les attributs composites sont traités de la manière suivante :
* Pour un attribut **style**, pour chaque sous attribut, un driven sera créé ayant le nom 
  `PREFIX:NOM_SOUS_ATTRIBUT` où PREFIX est la valeur de l'attribut `by`.

  **Exemple :**
  ```svg
  <circle cx="-110" cy="200" r="100" fill="orange" style="stroke:yellow;stroke-width:5">
      <driven target="stroke" by="cstroke" />
  </circle>
  ```
  
### 4 - Valeurs calculées

Pour piloter un attribut par une valeur calculée, il faut simplement spécifier l'opération a effectuer dans l'attribut
`by` et ainsi, chaque variable dans l'attribut `by` deviendra un driven en elle même.  
Les types des drivens générés sont **déduit automatiquement** en fonction de l'opération (voir tableau des
opérations réalisabes ci-dessous).  

##### tableau des opérations disponibles (d, d1,d2 sont des drivens/opérandes):

 opération |     type des opérandes    |      type du résultat     |description
:---------:|:-------------------------:|:-------------------------:|:----------:
  d1 - d2  |      double ou entier     |      double ou entier     | soustraction
  d1 + d2  |      double ou entier     |      double ou entier     | addition
  d1 * d2  |      double ou entier     |      double ou entier     | multiplication
  d1 / d2  |      double ou entier     |      double ou entier     | division
Autres opérations a venir.
  
On peux utiliser les parenthèses pour former des expressions plus complexes.

**Exemple :**

```svg
<circle cx="-110" cy="200" r="100" fill="orange">
    <driven target="cy" by="(input1 - input2) * input3"/>
</circle>
```
Donnera les drivens `input1`, `input2` et `input3` qui sont de type double ou entier , et on peut les réutiliser dans d'autres 
attributs `by`.

III - Lancer un serveur
=======================

Pour cela utilisez la commande :
```shell script
./svgserver chemin_image_svg
```
L'executable se trouve à la racine du projet après la compilation.

Sans préciser le port, c'est le port 6789 qui sera utilisé.

IV - Utilisation du client simple 
=================================

#### client en ligne de commandes

Pour cela utilisez la commande : 
```shell script
./svgclient [addresse_ip]
```
L'executable se trouve à la racine du projet après la compilation.

Sans argument, addresse_ip sera 127.0.0.1 (localhost)
