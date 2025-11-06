# DiscordBotCpp

### Копирование моего репо
```shell
git clone https://github.com/Progress-ux/DiscordBotCpp.git
cd DiscordBotCpp
```

### Добавление DPP
```shell
mkdir libs
cd libs
git clone https://github.com/brainboxdotcc/DPP.git
git clone https://github.com/xiph/ogg.git  
git clone https://github.com/xiph/opusfile
cd ..
```

### Создание папки build 
```shell
mkdir build 
cd build 
```

### Создайте .env
```
TOKEN=ВАШ_ТОКЕН
```

### Сборка бота
```shell
cmake ..
cmake --build .
./main
```

### Возможные команды 
```shell
sudo pacman -S cmake
```