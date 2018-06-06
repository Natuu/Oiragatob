# Oiragatob

Oigaratob est une intelligence artificielle pour agar.io

## Compilation et lancement
Compiler avec
```
make
```

Lancer avec
```
./bot -o agar.io 127.0.0.1:1443
```

## Fonctionnement
### Création
Lorsque le client est connecté au serveur, le bot envoie un paquet avec son nom.

### Grille et densité
Le bot gère la carte comme une grille. Chaque zone de cette grille a une densité qui lui est propre.
Cette densité dépend des cellules qui la composent.

#### Mapping
Pour créer cette grille, lorsqu'un paquet contenant toutes les cellules visibles est reçu, la map visible est découpée en zones de taille mangeable par le bot. Chaque zone est représentée par un entier.

#### Remplissage
Toutes les cellules reçues sont analysées une par une. La zone qui contient la position de la cellule analysée est alors "densifiée".
Concrètement, à sa valeur est ajoutée la taille de la cellule *fois un facteur*. Cette taille est également ajoutée *fois un autre facteur* aux cellules alentours pour favoriser une zone avec deux cellules proches. On appellera ceci par la suite "auréolage".

#### Attrait
Chaque type de cellule a un "attrait" différent :

| Cellule | Attrait | Justification |
|---------|---------|---------------|
| Food    |    **1**    | Valeur de base|
| Virus trop gros  | **0** | Le bot l'ignore |
| Virus mangeable | **1** Si solo, **-1** sinon | Le bot ne veut pas être split non volontairement en multi |
| Ennemi trop gros | **-100** | Le bot cherche à éviter les adversaires dangereux |
| Ennemi de notre taille | **-1** | Pour l'instant le bot évite les adversaires incertains |
| Ennemi mangeable | **1** | C'est de la nourriture |
| Cellule nous appartenant | **-10** Si solo **10** Si multi | Pour se répandre ou se rassembler selon la stratégie |

### Direction
Par défaut, la plus petite des cellules du bot pilote.
Elle se dirige vers la zone la plus attrayante en fonction de sa distance.

### Split
En solo le bot va se splitter le plus possible.
En multi, il se splittera si une zone permet au morceau ejecté de retrouver la taille qu'il avait avant split *fois un facteur*.
