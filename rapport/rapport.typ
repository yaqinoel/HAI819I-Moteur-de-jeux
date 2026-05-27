#set document(
  title: "Prototype de moteur 3D",
  author: (
    "GUIBERT-LODE Titouan",
    "Zhang Yaqi",
  ),
)

#set page(
  paper: "a4",
  margin: (x: 2.75cm, y: 2.35cm),
  numbering: none,
)

#set text(font: "New Computer Modern", size: 10.15pt, lang: "fr")
#set par(justify: true, leading: 0.54em)
#set heading(numbering: "1.")
#set figure(numbering: "1")

#let small-table(body) = text(size: 8.2pt)[#body]
#let note(body) = text(size: 8.8pt, fill: rgb("#666666"))[#body]

#let todo(body) = block[
  #text(fill: rgb("#b3261e"), weight: "bold")[À compléter :] #body
]

#let capture(name) = figure(
  rect(
    width: 100%,
    height: 4.4cm,
    stroke: (paint: rgb("#9a9a9a"), dash: "dashed"),
    fill: rgb("#f7f7f7"),
  )[
    #align(center + horizon)[#text(fill: rgb("#777777"))[#name]]
  ],
  caption: [#name],
)

#align(center)[
  #v(2.6cm)
  #text(size: 17pt, weight: "bold")[Projet de l’UE Moteur de Jeux]

  #v(0.5cm)
  #text(size: 12pt)[HAI819I -- 2025/2026]


  #v(1.2cm)
  #table(
    columns: (5.2cm, 3.2cm),
    stroke: none,
    inset: 4pt,
    align: (left, left),
    [GUIBERT-LODE Titouan],
    [ZHANG Yaqi],
  )

  #v(1.4cm)
  Université de Montpellier -- Master 1 Informatique
]

#pagebreak()

#v(2.2cm)
#text(size: 15pt, weight: "bold")[Sommaire]
#v(0.8cm)
#text(size: 11.4pt)[
  #outline(title: none)
]

#pagebreak()
#counter(page).update(1)
#set page(numbering: "1")


= Introduction

Notre projet avait pour but de développer un prototype de moteur 3D, accompagné d'une démonstration inspirée de Minecraft. Ce prototype nous permet de montrer les fonctionnalités du moteur dans un monde en voxels : génération procédurale du terrain, rendu temps réel, interactions joueur, blocs dynamiques et moteur physique.

= Bases essentielles

Nous avons choisi une architecture similaire à celle de Godot, car ce moteur est open source et il est donc facile d’étudier son code source pour s’en inspirer.

Nous avons donc commencé par reproduire les graphes de scène comme on peut les trouver dans Godot, composés d’une hiérarchie de Node3D. Par exemple, pour avoir un cube dynamique, on utilise un RigidBody3D qui a pour enfants un Collider3D et un Mesh. Chaque objet a donc sa position locale et sa position monde. À la racine de l’arbre formé par les Node, il n’y a qu’un seul objet Scene. On peut donc simplement changer de scène en changeant l’objet instancié dans main().

#figure(
  image("figures/scene_graphe.png", width: 50%),
  caption: [Hiérarchie de scène dans Godot. Elle fonctionne de la même façon dans notre projet, mais nous n'avons pas développé d'interface d'édition pour afficher directement notre graphe de scène ; cette figure sert donc à illustrer le principe que nous avons reproduit dans notre moteur.]
)

= Spécialisation : génération procédurale du terrain

Nous avons ensuite créé le terrain infini de Minecraft, en le divisant par chunk. Le jeu n’affiche qu’un certain nombre de chunks autour du joueur, afin de limiter le coût en performances. Le terrain est très simple avec juste de l’herbe, de la terre et de la pierre en fonction de la profondeur, et est réalisé en mélangeant des bruits de Perlin. Afin d’optimiser les performances, un chunk n’a qu’un seul mesh, qui ne contient que les faces extérieures visibles des blocs.

#figure(
  image("figures/couches_generation_procedural.png", width: 50%),
  caption: [Démonstration des couches d’herbe, de terre et de pierre]
)
#figure(
  image("figures/terrain_infini.png", width: 50%),
  caption: [Le terrain est généré jusqu’à une certaine distance autour du joueur, mais peut être étendu à l’infini.]
)
#figure(
  image("figures/terrain_culling.png", width: 50%),
  caption: [Optimisation du maillage d'un chunk : seules les faces extérieures des blocs sont générées et rendues.]
)

= Interactions

Pour naviguer ce monde l’utilisateur a accès à un CharacterController qui lui permet de se déplacer avec ZQSD, sauter avec Espace, ou regarder autour avec la souris. Par défaut le contrôleur est en vue à la première personne mais une vue à la troisième personne est aussi possible.

Une interface utilisateur simple a aussi été implémentée, avec les blocs dans l’inventaire en bas, une croix au centre de l’écran et un menu pour quitter le jeu. Cela fonctionne simplement en affichant un quad à l’écran et en lui assignant une texture.

#figure(
  image("figures/interaction_inventaire.png", width: 50%),
  caption: [On voit l’inventaire du joueur en bas qui montre le nombre et le type d’objet récupéré. On peut également voir une croix au milieu de l’écran.]
)

En plus des options de déplacement, le joueur peut casser un bloc avec le clic gauche, qui est ajouté à son inventaire, placer un bloc depuis son inventaire avec le clic droit, ou bien jeter un bloc avec la touche E, qui sera un Rigidbody.

Quand un objet est placé avec le clic droit, il vient s’ajouter à un autre objet existant. Cela veut dire que s’il est placé sur un terrain, il devient une partie du mesh statique.
#figure(
  grid(
    columns: (1fr, 1fr, 1fr),
    gutter: 0.35cm,
    image("figures/interaction_getCube_init.png", width: 100%),
    image("figures/interaction_getCube.png", width: 100%),
    image("figures/interaction_putCube.png", width: 100%),
  ),
  caption: [Interaction avec le terrain : état initial, destruction d'un bloc avec le clic gauche puis ajout d'un bloc depuis l'inventaire avec le clic droit.]
)

S’il se place sur un Rigidbody, il augmente la taille du mesh représentant l’objet dynamique.

#figure(
  image("figures/interaction_voxel_dynamique.png", width: 70%),
  caption: [À droite des objets placés sur le terrain sont considérés comme statiques, donc ils ne tombent pas. À gauche ils ont été placés comme objets dynamiques, donc ils tombent sous l’effet de la gravité.]
)

== Contrôles

#small-table[
#figure(
  table(
    columns: (3.0cm, 9.0cm),
    inset: 4pt,
    align: (left, left),
    table.header([*Contrôle*], [*Action*]),
    [`ZQSD`], [Déplacements directionnels.],
    [`Souris`], [Regarder autour du joueur.],
    [`Clic gauche`], [Casser un bloc et l’ajouter à l’inventaire.],
    [`Clic droit`], [Placer un bloc depuis l’inventaire.],
    [`E`], [Projeter un bloc dynamique avec un `RigidBody`.],
    [`1` à `5`], [Projeter des cubes avec cinq matériaux PBR différents.],
    [`F`], [Activer ou désactiver la gravité.],
    [`Espace`], [Sauter si la gravité est activée ; monter si elle est désactivée.],
    [`Left Shift`], [Descendre si la gravité est désactivée.],
    [`Molette souris`], [Naviguer dans l’inventaire.],
    [`H`], [Cacher ou afficher l’interface utilisateur.],
    [`Escape`], [Ouvrir le menu.]
  ),
  caption: [Contrôles principaux du prototype.]
)
]

= Rendu

Nous avons implémenté plusieurs techniques de rendu dans le moteur. D'abord un rendu Blinn-Phong classique, utilisé comme base pour les matériaux éclairés par des lumières directes. Nous avons ensuite ajouté un rendu PBR basé sur le modèle de Cook-Torrance, avec des paramètres et des textures physiques comme l'albedo, les normales, le metallic, la roughness et l'ambient occlusion. Pour améliorer l'éclairage global, nous avons intégré une skybox et un éclairage par environnement (IBL), avec une irradiance map, une prefilter map et une BRDF LUT. Enfin, nous avons ajouté une lumière directionnelle dans la scène et un Shadow Mapping associé pour projeter ses ombres et mieux ancrer les objets dans le monde.

#figure(
  grid(
    columns: (1fr, 1fr),
    gutter: 0.45cm,
    image("figures/cube_phong_rustIron.png", width: 100%),
    image("figures/cube_phong_plastic.png", width: 100%),
  ),
  caption: [Cubes rendus avec le pipeline Blinn-Phong classique sous une lumière directionnelle. Les deux objets utilisent deux jeux de textures différents, métal rouillé et plastique, préalablement pixelisés. Le contour noir indique l'objet sélectionné dans la scène.]
)

#figure(
  grid(
    columns: (1fr, 1fr),
    gutter: 0.45cm,
    image("figures/cube_pbr_rustIron.png", width: 100%),
    image("figures/cube_pbr_plastic.png", width: 100%),
  ),
  caption: [Même scène rendue avec le pipeline PBR Cook-Torrance. Par rapport au rendu Blinn-Phong, les textures physiques donnent un contrôle plus réaliste de l'albedo, des normales, du metallic, de la roughness et de l'ambient occlusion.]
)
#figure(
  grid(
    columns: (1fr, 1fr),
    gutter: 0.45cm,
    image("figures/IBL_skybox.png", width: 100%),
    image("figures/cube_pbr_gold_IBL.png", width: 100%),
  ),
  caption: [Skybox et éclairage par environnement intégrés au moteur. La skybox sert de fond visuel à la scène et l'IBL permet d'utiliser cet environnement pour éclairer les objets. L'effet est visible sur le cube métallique à droite, qui reflète les couleurs de l'environnement.]
)
#figure(
  image("figures/shadow_mapping.png", width: 90%),
  caption: [Shadow Mapping avec une lumière directionnelle. Les objets de la scène projettent des ombres sur le sol, ce qui rend leur position plus lisible et les ancre mieux dans l'environnement.]
)


= Physique

Nous avons implémenté un moteur physique pour gérer les interactions entre les objets de la scène. Les objets dynamiques sont représentés par des corps rigides avec une masse, une vitesse, une vitesse angulaire, de la friction et de la restitution. Le moteur applique la gravité, intègre le mouvement à chaque pas physique, détecte les collisions avec différents colliders, puis corrige les positions et les vitesses grâce à un solveur de contacts. Les objets peuvent ainsi tomber, rebondir, glisser, s'empiler et se stabiliser dans la scène.

#figure(
  grid(
    columns: (1fr, 1fr),
    gutter: 0.45cm,
    image("figures/physique_cube_inti.png", width: 100%),
    image("figures/physique_cube_cube_collision.png", width: 100%),
  ),
  caption: [Collision entre cubes physiques. Des cubes avec corps rigide et collider sont lancés dans la scène : ils tombent sous l'effet de la gravité, entrent en collision avec le terrain, puis se poussent entre eux, ce qui produit des déplacements et des rotations.]
)

#figure(
  grid(
    columns: (1fr, 1fr),
    gutter: 0.45cm,
    image("figures/physique_voxel_voxel_collision.png", width: 100%),
    image("figures/physique_cube_voxel_collision.png", width: 100%),
  ),
  caption: [Collisions avec des blocs de voxels dynamiques. Les assemblages de voxels possèdent aussi un comportement physique : ils peuvent entrer en collision avec le terrain, se pousser entre eux, et interagir avec les cubes rigides de la scène.]
)

#figure(
  grid(
    columns: (1fr, 1fr),
    gutter: 0.45cm,
    image("figures/physique_dy_voxel_init.png", width: 100%),
    image("figures/physique_dy_voxel_tombe.png", width: 100%),
  ),
  caption: [Construction dynamique d'un bloc de voxels physique. Lorsque de nouveaux voxels sont ajoutés sur le côté, le centre de masse de l'objet se déplace. Sous l'effet de la gravité, ce décalage crée un couple qui fait tourner l'assemblage, jusqu'à ce qu'il bascule et tombe sur le terrain.]
)

= Limites et perspectives

Le moteur a d’autres fonctionnalités qui n’ont pas été utilisées dans le projet, comme la possibilité d’implémenter des LOD. Cette fonctionnalité a été implémentée pendant les TP mais n’est pas utile dans un monde en voxels.

Malheureusement le temps nous a manqué pour implémenter la séparation des objets. C’est-à-dire que si un objet est coupé en deux, il devient deux objets dynamiques distincts. Avec cette fonctionnalité nous aurions pu ajouter des arbres, et donc couper un arbre l’aurait fait tomber au sol. Nous aurions utilisé la recherche de composantes connexes pour réaliser cette opération.

#figure(
  image("figures/limite.png", width: 70%),
  caption: [Le bloc à gauche a été placé en tant qu’objet statique connecté au terrain, puis les blocs le reliant ont été supprimés. Cependant l’objet ne tombe pas car le moteur ne détecte pas la déconnexion.]
)

Une autre piste d’amélioration aurait aussi été d’utiliser du multithreading pour générer les terrains distants. À l’heure actuelle on peut parfois discerner un léger ralentissement le temps de générer un nouveau terrain, ce qui aurait été résolu par cette approche.
