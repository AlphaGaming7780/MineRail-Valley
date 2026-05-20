# Documentation de l'API de jeu

Ce document resume l'architecture imaginee pour MineRail Valley et explique comment s'en servir pour ajouter des mecanismes de jeu. Il decrit l'etat actuel du code, pas une API finale: plusieurs systemes sont deja utilisables, d'autres sont encore des intentions a finir.

## Vue d'ensemble

Le projet est structure autour d'une boucle centrale SFML pilotee par `Game::GameManager`. Les services principaux sont exposes en singletons:

- `GameManager`: demarrage, boucle principale, chargement, shutdown.
- `GameWindow`: creation et gestion de la fenetre SFML.
- `EventManager`: bus d'evenements type-safe.
- `InputManager`: actions clavier/souris regroupees par blocs activables.
- `World`: proprietaire des `GameObject` et des `SystemBase`.
- `RenderingManager`: collecte les objets du monde et les dessine par layer.
- `UIManager`: racine UI, layout, redraw et evenements souris.
- `AudioManager`: sons, musiques et volumes.
- `AssetDatabase<T>` et ses derives: chargement/cache d'assets.

Le flux actuel ressemble a ceci:

```text
main()
  -> GameManager::Start()
     -> cree GameWindow
     -> initialise EventManager
     -> OnStart()
        -> cree CameraSystem
        -> Load(InGame, NewGame, MapDatabase::GetDefault())
     -> MainLoop()
        -> input/event update
        -> audio update
        -> world update
        -> ui update
        -> render world + UI
        -> Time::Update()
```

## Cycle de vie du jeu

`GameManager::Start()` initialise les services puis lance `MainLoop()`. La boucle appelle, dans l'ordre:

1. `GameWindow::Clear()`
2. `InputManager::ResetEdges()`
3. `EventManager::process()`
4. `InputManager::Update()`
5. `AudioManager::Update()`
6. `GameManager::OnUpdate()`
7. `World::Update()`
8. `UIManager::Update()`
9. `RenderingManager::Render()`
10. `UIManager::Draw()`
11. `GameWindow::Display()`
12. `Time::Update()`

`Load(GameMode, Purpose, MapData*)` sert de point d'entree pour changer de phase. Il met le monde en pause, passe l'etat en `Loading`, affiche `UILoadingScreen`, envoie `LoadingStart`, puis `LoadingComplete`, remet le monde en route et passe en `WorldReady`.

Pour construire le jeu, l'idee est donc de brancher les comportements de chargement sur les evenements `LoadingStart` et `LoadingComplete`, plutot que de tout mettre directement dans `GameManager`.

## Etats, modes et intentions

Les enums dans `GameStates.h` definissent le contexte global:

- `GameState`: `Booting`, `Loading`, `WorldReady`, `Quitting`, etc.
- `GameMode`: `MainMenu`, `InGame`, etc.
- `Purpose`: `NewGame`, `LoadGame`, `SaveGame`, `Cleanup`, etc.

Exemple d'usage prevu:

```cpp
Load(GameMode::InGame, Purpose::NewGame, MapDatabase::Instance().GetDefault());
```

Les systemes peuvent reagir ainsi:

```cpp
void MySystem::OnGameLoadingStart(GameMode mode, Purpose purpose)
{
    SetEnable(false);
}

void MySystem::OnGameLoadingComplete(GameMode mode, Purpose purpose)
{
    if (mode == GameMode::InGame)
        SetEnable(true);
}
```

## World, objets et systemes

`World` est le conteneur principal du gameplay. Il possede:

- les `GameObject`, stockes dans `m_GameObjects`;
- les `SystemBase`, stockes par type dans `m_Systems`;
- l'etat pause/unpause du monde.

Pour creer un systeme:

```cpp
World::Instance().GetOrCreateSystem<CameraSystem>();
```

`GetOrCreateSystem<T>()` garantit qu'un seul systeme du type `T` existe. Le systeme recoit un pointeur vers le monde et un logger, puis `OnCreate()` est appele.

Pour creer un objet:

```cpp
TileObject* tile = World::Instance().CreateGameObject<TileObject>(tileData, index);
tile->m_RenderLayer = RenderLayer::Terrain;
```

Les objets sont mis a jour apres les systemes, seulement si `m_Enabled` vaut `true`.

Attention: actuellement `GameObject::m_Enabled` vaut `false` par defaut. Les objets crees ne recoivent donc pas leur `Update()` tant qu'ils ne sont pas explicitement actives.

## GameObject et donnees d'assets

`GameObject` represente une entite dessinable. Il porte:

- une texture (`m_Texture`);
- une taille logique (`m_Size`);
- un sprite SFML (`m_Sprite`);
- des informations d'animation sprite-sheet;
- un `RenderLayer`;
- un `zIndex`.

Deux constructeurs existent:

- `GameObject()`: charge la texture par defaut.
- `GameObject(const ObjectData&)`: hydrate l'objet depuis des donnees JSON.

`ObjectData` contient les champs JSON prevus:

```json
{
  "texture": "Textures/DEFAULT.png",
  "color": { "r": 255, "g": 255, "b": 255, "a": 255 },
  "size": { "x": 1.0, "y": 1.0 },
  "animated": false,
  "framesX": 1,
  "framesY": 1,
  "duration": 0.0
}
```

Pour les tiles, `TileData` contient maintenant un `ObjectData object` et un booleen `m_CanBuild`. `TileObject` construit donc son sprite avec `tileData.object`, copie `m_CanBuild`, positionne le sprite selon son index de grille, puis active l'objet (`m_Enabled = true`).

Format actuel d'une tile:

```json
{
  "ObjectData": {
    "texture": "Textures\\DEFAULT.png",
    "color": { "r": 255, "g": 255, "b": 255, "a": 255 },
    "size": { "x": 1.0, "y": 1.0 },
    "animated": false,
    "framesX": 1,
    "framesY": 1,
    "duration": 0.0
  },
  "m_CanBuild": false
}
```

`World::CreateMap()` lit une `MapData`, charge chaque tile via `TileDatabase`, puis cree des `TileObject`.

## Cartes et assets JSON

`MapDatabase` charge des fichiers dans `assets/Maps`. Le format actuel de `MapData` est:

```json
{
  "MapSize": 32,
  "Difficulty": 1,
  "tiles": [
    "Tiles/DEFAULT.json"
  ]
}
```

`MapSize` est utilise comme largeur de carte car `CreateMap()` calcule:

```cpp
int xIndex = i % mapEdgeSize;
int yIndex = i / mapEdgeSize;
```

Chaque entree de `tiles` pointe vers un fichier charge par `TileDatabase`.

Le systeme d'assets est generique:

```cpp
sf::Texture* tex = TextureDatabase::Instance().Load("Textures/DEFAULT.png");
TextureDatabase::Instance().Unload("Textures/DEFAULT.png");
```

`AssetDatabase<T>` met les assets en cache et garde un compteur de references. Si un asset ne charge pas, `Load()` tente de renvoyer l'asset par defaut du database concerne. `TextureDatabase` utilise maintenant `Textures/DEFAULT.png` comme texture par defaut.

## Systeme d'evenements

`EventManager` est un bus par type. Un objet observe un evenement en heritant de `IEventObserver<TEvent>` puis en s'enregistrant:

```cpp
class MySystem : public IEventObserver<MyEvent>
{
public:
    void OnEvent(const MyEvent& ev) override;
};

EventManager::Instance().Register<MyEvent>(this);
EventManager::Instance().Notify(MyEvent{});
```

Les evenements SFML sont aussi redistribues par type concret grace a `sf::Event::visit`. Par exemple `GameWindow` observe `sf::Event::Closed`, `InputManager` observe les evenements clavier/souris, et `UIManager` observe `sf::Event::Resized`.

Limite actuelle: il est interdit de register/unregister pendant une notification (`m_IsNotifying`). Si un systeme doit se detruire ou s'inscrire en reaction a un evenement, il faudra probablement ajouter une file d'attente.

## Inputs

Les inputs sont abstraits par des enum d'actions. Un action set doit heriter de `InputActionSet<Enum>`.

Exemple existant:

```cpp
enum class CameraAction : uint8_t {
    MoveUp,
    MoveDown,
    MoveLeft,
    MoveRight,
    Zoom,
    COUNT
};

struct CameraInputs : Game::InputActionSet<CameraAction> {
    static inline std::array<Mapping, (size_t)CameraAction::COUNT> DefaultMap = { {
        { Enum::MoveUp,    sf::Keyboard::Scan::W },
        { Enum::MoveDown,  sf::Keyboard::Scan::S },
        { Enum::MoveLeft,  sf::Keyboard::Scan::A },
        { Enum::MoveRight, sf::Keyboard::Scan::D },
        { Enum::Zoom,      sf::Mouse::Wheel::Vertical },
    } };
};
```

Dans un systeme:

```cpp
InputManager& input = InputManager::Instance();
input.Register<CameraInputs>();
input.EnableBlock<CameraAction>(true);

InputBindingState& moveUp = input.GetBinding(CameraAction::MoveUp);
if (moveUp.pressed) {
    // action continue
}
if (moveUp.justPressed) {
    // front montant
}
```

`InputManager::ResetEdges()` remet `justPressed`, `justReleased` et `wheelDelta` a zero chaque frame. L'UI peut bloquer les inputs monde: si la souris est au-dessus de l'UI, les clics ne sont pas transmis aux action sets souris.

Un deuxieme action set existe pour l'outil de rail:

```cpp
enum class TrackToolAction : uint8_t {
    Apply,
    Cancle,
    COUNT
};

struct TrackInputs : Game::InputActionSet<TrackToolAction> {
    static inline std::array<Mapping, (size_t)TrackToolAction::COUNT> DefaultMap = { {
        { Enum::Apply,  sf::Mouse::Button::Left },
        { Enum::Cancle, sf::Mouse::Button::Right }
    } };
};
```

`TrackToolSystem` s'en sert pour detecter clic gauche/clic droit. Le nom `Cancle` est celui du code actuel; il faudra le garder tel quel tant que l'enum n'est pas renomme.

## Camera et rendu

`Camera` encapsule une `sf::View`. Elle garde une taille de base, un zoom absolu, et s'adapte aux resize pour conserver un ratio 16:9 sans bandes.

`CameraSystem` est l'exemple de systeme gameplay deja branche:

- enregistre `CameraInputs`;
- desactive la camera pendant le chargement;
- reactive la camera en `GameMode::InGame`;
- deplace la camera avec WASD;
- zoome avec la molette.

`RenderingManager::Render()`:

1. applique la view camera;
2. recupere tous les objets du `World`;
3. les range par `RenderLayer`;
4. trie chaque layer par `zIndex`;
5. dessine les sprites dans l'ordre des layers.

Layers actuels:

```cpp
Terrain,
Tracks,
Trains,
NONE,
COUNT
```

Pour qu'un objet apparaisse, il doit avoir un `m_Sprite` valide et un `m_RenderLayer` pertinent.

## Outils de gameplay

Un debut de couche "outils" existe:

- `DefaultToolSystem`: systeme vide qui represente l'outil par defaut.
- `TrackToolSystem`: systeme d'edition de rails avec deux etats, `Default` et `Creating`.
- `TrainSystem`: squelette de futur systeme de trains.

`TrackToolSystem::OnCreate()` recupere ou cree le `DefaultToolSystem`, enregistre `TrackInputs`, puis garde des pointeurs vers les bindings `Apply` et `Cancle`.

Logique actuelle:

```cpp
if (m_State == State::Default)
{
    if (m_ApplyBinding->justPressed)
    {
        // debut de creation a implementer
    }
    else if (m_CancleBinding->justPressed)
    {
        SetEnable(false);
        m_DefaultToolSystem->SetEnable(true);
    }
}
```

L'intention semble etre: activer un outil specialise, laisser clic gauche appliquer/commencer une action, et clic droit annuler pour revenir a l'outil par defaut. Pour l'instant, `TrackToolSystem` n'est pas encore inclus dans `include/Game/Systems.hpp`, et `GameManager::OnStart()` ne le cree pas encore.

## UI

L'UI est une arborescence de `UIElement`. `UIManager` possede une racine (`UIGroup`) et dessine l'UI dans une `sf::RenderTexture`, puis blitte cette texture par-dessus le rendu monde.

Classes de base:

- `UIElement`: hierarchy, layout, hit test, focus, hover, click.
- `UIGroup`: conteneur sans hitbox propre, actif si un enfant est touche.
- `UIWidget`: element concret avec position, taille, ancre.
- `UIRoot`: racine transparente pleine taille.

Widgets existants:

- `UIButton`: trois textures possibles, callback au clic.
- `UILabel`: texte SFML.
- `UIProgressBar`: frame, barre interne, pivot/endcap.

Changer de racine UI:

```cpp
UILoadingScreen& loading = UIManager::Instance().SetRoot<UILoadingScreen>();
```

Ajouter un enfant:

```cpp
auto* label = new UILabel("Hello", { 20.f, 20.f }, UIAnchor::TopLeft, font, 24);
root.AddChild(label);
label->ReDraw();
```

Quand un widget change visuellement, il doit appeler `ReDraw()`. Le manager regroupe les elements invalides et les redessine dans `FlushRedraw()`.

Point d'attention: le redraw d'elements transparents est encore note comme imparfait, car le parent doit parfois etre redessine pour eviter l'accumulation de transparence.

## Audio

`AudioManager` ecoute:

- `PlaySoundEvent`
- `PlayMusicEvent`
- `PauseEvent`

Utilisation directe:

```cpp
AudioManager::Instance().PlaySound("Sounds/Menu Selection Click.wav", 100.f);
AudioManager::Instance().PlayMusic("Musics/theme.wav", true, 70.f);
```

Utilisation par evenement:

```cpp
EventManager::Instance().Notify(PlaySoundEvent{
    "Sounds/Menu Selection Click.wav",
    100.f
});
```

Les sons courts sont stockes en `sf::Sound*` dans `m_PlayingSounds`, puis purges quand ils sont termines. Les musiques sont gerees par `MusicDatabase`.

`AudioManager` s'enregistre maintenant lui-meme aupres de `EventManager` pour `PlayMusicEvent`, `PlaySoundEvent` et `PauseEvent`. `AudioManager::Update()` est appele dans la boucle principale, ce qui purge les sons courts termines.

## Comment ajouter une fonctionnalite de gameplay

Pour ajouter un nouveau comportement, le chemin recommande est:

1. Creer les donnees d'asset necessaires dans `include/Game/AssetDatabase/AssetType` si le contenu doit venir du JSON.
2. Ajouter un `AssetDatabase` dedie si ce type de donnees doit etre charge/cache.
3. Creer un `GameObject` si la fonctionnalite a une representation dans le monde.
4. Creer un `SystemBase` si la fonctionnalite a une logique par frame ou reagit au chargement.
5. Ajouter un `InputActionSet` si le joueur doit interagir.
6. Brancher le systeme dans `GameManager::OnStart()` ou lors d'une phase de chargement.
7. Utiliser `EventManager` pour decoupler l'UI, l'audio et le gameplay.

Exemple minimal de systeme:

```cpp
class BuildSystem : public SystemBase
{
public:
    BuildSystem(World* world, pallas::Logger& logger)
        : SystemBase(world, logger) {}

    void OnCreate() override
    {
        // register inputs, cache references, etc.
    }

    void Update() override
    {
        // logique par frame
    }

protected:
    void OnGameLoadingComplete(GameMode mode, Purpose purpose) override
    {
        SetEnable(mode == GameMode::InGame);
    }
};
```

Puis:

```cpp
World::Instance().GetOrCreateSystem<BuildSystem>();
```

## Ce qui reste a implementer ou corriger

Priorite haute:

- Verifier `FontDatabase::GetDefaultPath()` et `SoundDatabase::GetDefaultPath()`: les chemins par defaut pointent vers `Fonts/DEFAULT.png` et `Sound/DEFAULT.mp3`, alors que les dossiers/assets visibles sont plutot `Fonts` et `Sounds`.
- Corriger ou retirer `#include <Game/Rendering/RenderCommand.hpp>` dans `Rendering.hpp`, car le fichier n'apparait pas dans le depot.
- Corriger `InputManager::BindKey()`: il assigne `st.key`, mais `InputBindingState` contient `scan`, pas `key`.
- Enregistrer correctement `Camera` aux evenements `sf::Event::Resized`: le constructeur appelle `EventManager::Instance()` mais ne fait pas `Register`.
- Inclure les nouveaux systemes/outils dans les agregateurs (`Systems.hpp`, `InputsActionSets.hpp`) et verifier qu'ils sont bien ajoutes au projet Visual Studio si l'objectif est de les compiler.
- Ajouter les constructeurs attendus aux nouveaux systemes (`DefaultToolSystem`, `TrainSystem`) ou les rendre compatibles avec `World::GetOrCreateSystem<T>()`, qui instancie les systemes avec `(World*, pallas::Logger&)`.
- Faire heriter `TrainSystem` publiquement de `SystemBase` si on veut l'utiliser comme les autres systemes (`class TrainSystem : public SystemBase`).
- Implementer les constructeurs ou methodes declarees de `InGameUI`, car le `.cpp` existe mais ne definit encore rien.

Gameplay encore a construire:

- `TrackToolSystem` detecte deja les inputs, mais le placement de rails, l'etat `Creating`, la validation et la creation d'objets restent a implementer.
- `DefaultToolSystem` et `TrainSystem` sont encore des squelettes.
- Les tracks (`TrackObject`, `LineTrack`, `NodeTrack`) sont seulement des structures de base; il manque la logique de connexion, placement, validation et rendu detaille.
- Les stations n'ont pas encore de donnees ni de comportement (`StationData.h` est vide actuellement).
- Les trains et wagons existent comme champs de relations, mais pas encore de mouvement, pathfinding, arrets, chargement/dechargement ou collision.
- Le systeme de construction sur tiles constructibles (`m_CanBuild`) commence a se dessiner avec `TrackToolSystem`, mais il n'exploite pas encore les tiles.
- Les modes `MainMenu`, `LoadGame`, `SaveGame` et `Cleanup` sont prevus par les enums mais peu exploites.
- La sauvegarde/serialisation est mentionnee dans `GameManager::Load()` mais pas implementee.

Data/assets:

- Les fichiers de tiles visibles ont ete alignes sur `ObjectData` + `m_CanBuild`; garder cette convention pour les nouvelles tiles.
- Clarifier si les chemins doivent utiliser `/`, `\\` ou `std::filesystem::path` partout.
- Ajouter des fichiers par defaut reels pour chaque database afin que `GetDefault()` ne plante pas.
- Ajouter un database pour les tracks/stations si ces objets doivent etre data-driven comme les tiles.

Architecture:

- Remplacer progressivement les pointeurs nus proprietaires par des `std::unique_ptr` pour `World`, UI et audio.
- Eviter les destructions/register/unregister pendant `EventManager::Notify()` ou ajouter une file d'operations differees.
- Decider si `GameObject::m_Enabled` doit etre `true` par defaut. Les `TileObject` sont maintenant actives explicitement, mais les autres objets restent inactifs par defaut.
- Ajouter une API publique plus propre pour regler layer, zIndex, enabled, sprite, texture, au lieu d'exposer autant de membres publics.
- Reduire les logs par frame dans `RenderingManager::Render()`, car logger chaque layer a chaque frame risque de ralentir et de remplir les logs.
- Ajouter des tests ou au moins une scene de validation pour le chargement de carte, les inputs, le rendu par layer et l'UI.

## Resume pratique

Pour avancer vite sans casser l'intention de l'API:

- mettre la logique long terme dans des `SystemBase`;
- creer les entites du monde via `World::CreateGameObject`;
- passer les transitions de jeu par `LoadingStart` / `LoadingComplete`;
- decrire les contenus via JSON + `AssetDatabase`;
- envoyer les effets transverses avec `EventManager`;
- garder `GameManager` comme orchestrateur, pas comme classe gameplay.

Le moteur a deja une bonne colonne vertebrale. La prochaine grosse etape est de stabiliser les petits problemes d'API/compilation, puis de choisir un premier systeme gameplay concret, probablement construction de rails ou mouvement de train, et de le brancher proprement sur `World`, `InputManager`, `RenderingManager` et les assets JSON.
