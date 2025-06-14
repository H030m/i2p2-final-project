# I2P-2 Final Project - Multiplayer Dungeon Survival Game

## 🧠 Overview

This is a cross-platform (Windows/Linux) 2D **multiplayer survival game** developed for the I2P-2 course. Players control characters to battle various unique enemies, earn coins, and upgrade weapons. Defeat the **final boss (Yan Dong-Yong)** to win!

---

## 🎮 Controls

| Key           | Action   |
|---------------|----------|
| `W`           | Move Up  |
| `A`           | Move Left|
| `S`           | Move Down|
| `D`           | Move Right|
| `Left Click`  | Attack   |

---

## 🔫 Weapons

### Gun (Rifle)
- Fast firing rate.
- Upgrades:
  - Reduce reload time.
  - Increase bullet size.
  - Increase damage.

### Shotgun
- Fires a spread of bullets at close range.
- Upgrades:
  - Increase number of pellets.
  - Wider spread angle.
  - Increase bullet size and damage.

### Orbit
- Generates bullets orbiting around the player.
- Upgrades:
  - Increase number of orbiting bullets.
  - Increase bullet size and damage.

### Bounce
- Fires bullets that bounce off walls.
- Upgrades:
  - Increase number of bounces.
  - Increase bullet size and damage.

---

## 👹 Enemies

### 1. Zhou Bai-Xiang (NTHU Vtuber)
- **Invisibility** when not attacked for a period.
- Becomes visible only when hit.

### 2. Mai Wei-Ji (NAND~)
- Summons a **NAND gate shield**.
- Must destroy the shield to damage the main body.

### 3. Yan Dong-Yong (BOSS - "108 Curriculum Failure")
- When hit:
  - Heals and grants the player **+0.2 score**.
- Attacks:
  - Unleashes the “**Curriculum Failure Shockwave**” to damage players.

### Enemy Revival Mechanic
- **All enemies** can **revive after death**.
- Each revival **increases**:
  - Health
  - Attack power
  - Movement speed

---

## 🕹️ Game Modes

### 1. PLAY Mode
- Choose a map.
- Select **2 initial weapons**.
- Defeat enemies to earn coins.
- Spend coins to upgrade weapons.
- Defeat all **Yan Dong-Yong** to win.
- Game over if HP reaches 0.
- On revival, coins from last death are retained.

### 2. DRAW Mode
- Design custom maps and enemy spawn points.
- Save and play custom maps.

---

## 🧩 Features

- ✅ **Cross-platform multiplayer** (Windows & Linux)
- ✅ Enemies use **A\*** pathfinding
- ✅ **UI** shows status of all players
- ✅ **Fullscreen support**
- ✅ **Camera** follows the player
- ✅ Custom **map editor** with save/load
- ✅ enemy sound effects are voice-acted by the developers themselves.
- ✅ **enter your high school road** in win scene and will trigger hidden event base on the input.

---

## 💡 Project Info

- Course: Introduction to Programming II (I2P-2)
- Language: C++
- Libraries: Allegro5, JSON, Socket networking
- Developers: Samuel, Ryan & Ray.

---

## 📸 Screenshots

> _Feel free to insert gameplay screenshots or GIFs here_

---
