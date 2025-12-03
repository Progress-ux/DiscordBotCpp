# DiscordBotCpp

## Клонирование репозитория
```bash
git clone https://github.com/Progress-ux/DiscordBotCpp.git
cd DiscordBotCpp
```

## Создание `.env`
Создайте файл `.env` в корне проекта:
```
TOKEN=ВАШ_ТОКЕН
```

---

## Установка зависимостей

### Windows
1. Установите [FFmpeg](https://ffmpeg.org/download.html) и добавьте его в переменные окружения.
2. Установите CMake и компилятор (например, MSVC через Visual Studio).
3. Установите Git, если ещё не установлен.

### Linux (Arch)
```bash
sudo pacman -Syu cmake ffmpeg git base-devel
```

---

## Сборка бота

Создайте папку `build` и перейдите в неё:
```bash
mkdir build
cd build
```

Сборка через CMake:
```bash
cmake ..
cmake --build .
```

Запуск:
```bash
./src/main        # Linux
src/main.exe      # Windows
```

---

## Настройка ограничений (опционально, для контейнера/виртуальной среды)

**Пример для Docker на Linux:**
```bash
docker run -d \
  --name mybot_container \
  --cpus="2" \
  --memory="1.5g" \
  --memory-swap="1.5g" \
  --storage-opt size=5G \
  mybot:latest
```

Для ограничения сети используйте `tc` внутри контейнера:
```bash
tc qdisc add dev eth0 root tbf rate 5mbit burst 32kbit latency 400ms
```

---

## Возможные команды бота

- `/play <url>` – добавить трек в очередь и начать воспроизведение  
- `/skip` – пропустить текущий трек  
- `/pause` – приостановить воспроизведение  
- `/resume` – продолжить воспроизведение  
- `/stop` – остановить воспроизведение  
- `/join` – подключить бота к голосовому каналу  
- `/leave` – отключить бота от голосового канала  
- `/ping` – проверить отклик бота  
- `/back` – вернуться к предыдущему треку  
- `/queue` – показать очередь воспроизведения  
- `/history` – показать историю воспроизведения

---

## Рекомендуемые пакеты

### Linux Arch
```bash
sudo pacman -S cmake ffmpeg git base-devel
```

### Windows
- CMake
- Visual Studio (MSVC)
- FFmpeg
- Git
