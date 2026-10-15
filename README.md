# 🔐 Chiffrement RSA en C++

Une implémentation **from scratch** du cryptosystème à clé publique RSA en C++, utilisant la bibliothèque multiprécision de Boost pour l'arithmétique sur entiers de taille arbitraire.

Ce projet est à but **éducatif** : il montre les mécanismes internes de RSA (génération de clés, exponentiation modulaire, test de primalité, inverse modulaire) sans s'appuyer sur une bibliothèque de cryptographie.

## Table des matières

- [Fonctionnalités](#fonctionnalités)
- [Prérequis](#prérequis)
- [Compilation et exécution](#compilation-et-exécution)
- [Utilisation](#utilisation)
- [Référence de l'API](#référence-de-lapi)
- [Fonctionnement](#fonctionnement)
- [Exemple d'exécution](#exemple-dexécution)
- [Structure du projet](#structure-du-projet)
- [Avertissement de sécurité](#avertissement-de-sécurité)
- [Pistes d'amélioration](#pistes-damélioration)
- [Licence](#licence)

---

## Fonctionnalités

- Arithmétique en précision arbitraire via `boost::multiprecision::cpp_int`
- Test de primalité **Miller-Rabin** avec 12 bases fixes (déterministe pour les valeurs < 3,3 × 10²⁴)
- Génération de clés RSA avec taille en bits configurable
- Exponentiation modulaire (square-and-multiply) pour un chiffrement/déchiffrement efficace
- Algorithme d'Euclide étendu pour le calcul des inverses modulaires
- Conversion chaîne ↔ entier pour chiffrer des messages texte
- Structure `RSAKEYS` retournant `(n, e, d)` en un seul appel
- ⚠️ Pas de padding OAEP : il s'agit du RSA « textbook » (voir l'[avertissement de sécurité](#avertissement-de-sécurité))

## Prérequis

- C++11 ou supérieur
- Boost (headers uniquement : `boost/multiprecision/cpp_int.hpp`)

### Installation de Boost

**Ubuntu / Debian**

```bash
sudo apt install libboost-all-dev
```

**macOS (Homebrew)**

```bash
brew install boost
```

**NixOS**

```nix
environment.systemPackages = [ pkgs.boost ];
```

**Windows**

Installer Boost via vcpkg ou l'installateur officiel.

## Compilation et exécution

```bash
g++ -std=c++11 -O2 -o rsa_crypto_cpp main.cpp
./rsa_crypto_cpp
```

Ou avec CMake :

```bash
mkdir build && cd build
cmake .. && make
./rsa_crypto_cpp
```

## Utilisation

La fonction `main()` illustre un aller-retour complet :

```cpp
RSAKEYS keys = keygen(1024);                       // modulo de 2048 bits
cpp_int cipher = encrypt("Hello", keys.e, keys.n);
string plain = decrypt(cipher, keys.d, keys.n);
```

### Exemple rapide

```cpp
int main() {
    RSAKEYS keys = keygen(512);                    // modulo de 1024 bits
    cpp_int c = encrypt("Secret", keys.e, keys.n);
    cout << decrypt(c, keys.d, keys.n) << endl;    // -> "Secret"
}
```

## Référence de l'API

| Fonction | Description |
|----------|-------------|
| `cpp_int big_int(int bits)` | Génère un entier aléatoire de `bits` bits avec `mt19937_64`, par blocs de 64 bits. |
| `bool isprime(cpp_int value)` | Test de Miller-Rabin avec les bases `{2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37}`. |
| `cpp_int gen_prime(int bits)` | Tire des candidats impairs jusqu'à trouver un nombre premier. |
| `cpp_int mod_pow(base, exp, mod)` | Exponentiation modulaire (square-and-multiply). |
| `cpp_int mod_inverse(e, phiN)` | Euclide étendu pour `e⁻¹ mod φ(N)`. |
| `RSAKEYS keygen(int bits)` | Génère `(n, e, d)` avec `e = 65537`. |
| `cpp_int str_to_m(string)` | Convertit une chaîne en grand entier (8 bits par caractère, big-endian). |
| `string m_to_str(cpp_int)` | Opération inverse de `str_to_m`. |
| `cpp_int encrypt(string, e, n)` | Retourne `m^e mod n`. |
| `string decrypt(cpp_int, d, n)` | Retourne `c^d mod n` sous forme de chaîne. |

### Structure

```cpp
struct RSAKEYS {
    cpp_int n;  // module = p * q
    cpp_int e;  // exposant public
    cpp_int d;  // exposant privé
};
```

## Fonctionnement

### 1. Génération des clés

1. Générer deux grands nombres premiers aléatoires `p` et `q`, de `bits` bits chacun.
2. Calculer le module et l'indicatrice d'Euler :

   ```
   n = p · q
   φ(n) = (p − 1)(q − 1)
   ```

3. Choisir `e = 65537` (ou le prochain entier impair premier avec `φ(n)`).
4. Calculer l'exposant privé :

   ```
   d = e⁻¹ mod φ(n)
   ```

La clé publique est `(n, e)` ; la clé privée est `(n, d)`.

### 2. Chiffrement

```
c = m^e mod n
```

### 3. Déchiffrement

```
m = c^d mod n
```

La correction repose sur le théorème d'Euler : si `ed ≡ 1 (mod φ(n))`, alors `m^(ed) ≡ m (mod n)` pour tout `m < n`.

## Exemple d'exécution

Une vraie session terminal du projet sous NixOS, montrant un aller-retour complet de la chaîne `"Hello"` :

```text
[user@nixos:~/Projects/dev/rsa_crypto_cpp/build]$ ./rsa_crypto_cpp
Generating keys ...

n = 643215351280983775315286239567722289703925538856330697074569007954104325333611380657880931361643051922019445012647908076722117632648942263993327382778184170718027823609487621766441390027169896544719142351509448195264501909673163028687123901404247067501861820132198816837372206957284051831174080614302931582034112704823310594366329214450560566538857081810582460286876151853790005717917966347864932407791897983693198196082464654291399849038110846488198347163895150275352246724194586975310758812097606955159836614428573093308982840836467082213830817238980983713460094687617172995037527163230239055455471271265230576579

e = 65537

d = 449711927781954583955959698845424768245778294946914397525883493499321212309251370571200423515294357113063658542877150250751242077690574537718208859213561452102945708616664362374843385147854674909983304418702632493938000549355844837839277115007461508460912316100484947225311379754790615056475388678578125761289554551474736448728862366969717858768308377805678167533992972222272290958898804161618479107555406677246985973491748408936698724705936035068955974712760263265174648989822708491575228149528836240701539655787097320637871278850274107274446134706564654680870056519634648745638756263236493575475854957044569958513

numerical represntation of the word Hello : 310939249775

m = Hello (Word To Encrypt)

Encrypted m (Hello) in RSA: 583626004394508231227849384305847300263147965523762966450686756086516448896462067291425577535013092509004760748975835428076520144565363981607578040912196475418836790073754231802628688250687260230258432438702892778277233645550981003705196875176271935958238949096179188665267462455326458823192841403452045258099736948744774700827311575547865210961040719942348458933635578726158574502864887875481022842601203697199629293649993434059194509491657862613724547751726829834628165725267338087758585373936092391966430312662681392935544526895873435435690055025201915104645065045377168144361322732650427819783553614775947363958

Decrypted m (Hello) in RSA: Hello
```

### Ce que nous dit la sortie

| Champ | Description |
|-------|-------------|
| `n` | Le module RSA de 2048 bits (`n = p·q`), affiché en décimal. |
| `e` | L'exposant public (`65537`, le nombre de Fermat standard). |
| `d` | L'exposant privé, calculé comme `e⁻¹ mod φ(n)`. |
| `str_to_m("Hello")` | `310939249775` : la représentation entière de `"Hello"`, octet par octet. |
| Valeur chiffrée | Le chiffré `c = m^e mod n`. |
| Valeur déchiffrée | `"Hello"` : récupéré avec succès via `m = c^d mod n`. |

### Vérification : `str_to_m("Hello")`

Chaque caractère de `"Hello"` occupe un octet (8 bits), empaquetés en big-endian :

```
H = 0x48 = 72
e = 0x65 = 101
l = 0x6C = 108
l = 0x6C = 108
o = 0x6F = 111

0x48 65 6C 6C 6F
= 72·256⁴ + 101·256³ + 108·256² + 108·256 + 111
= 310939249775 ✅ correspond à la sortie du programme
```

> **Note :** la génération des clés utilise `std::random_device` à l'exécution, donc chaque lancement produit un triplet `(n, e, d)` différent, mais `decrypt(encrypt(m)) == m` est toujours vérifié.

## Structure du projet

```text
.
├── main.cpp      # Implémentation complète en un seul fichier
└── README.md
```

## Avertissement de sécurité

Ce code est à but **éducatif uniquement**. Il n'est pas conçu pour protéger de vraies données, car :

- Il utilise du RSA « textbook » sans padding OAEP.
- `str_to_m` produit un encodage déterministe et sans padding.
- La source aléatoire (`mt19937_64` initialisé par `random_device`) n'est pas cryptographiquement sûre.
- `isprime` utilise des bases de Miller-Rabin fixes : déterministe sur les plages testées, mais non prouvé pour des tailles arbitraires.
- Aucune protection contre les attaques par canaux auxiliaires, pas d'opérations en temps constant.

Pour une vraie application, utilisez une bibliothèque éprouvée comme :

- [OpenSSL](https://www.openssl.org/)
- [libsodium](https://libsodium.org/)
- [Botan](https://botan.randombit.net/)

## Pistes d'amélioration

- Remplacer `mt19937_64` par un CSPRNG (par ex. `/dev/urandom`)
- Ajouter le padding OAEP
- Paralléliser la recherche de nombres premiers avec OpenMP
- Gérer le chiffrement de fichiers et pas seulement de courtes chaînes
- Remplacer le test de primalité maison par `boost::multiprecision::miller_rabin_test`

## Licence

MIT : libre d'utilisation, de modification et de distribution à des fins éducatives.
