# Базовый образ Arch
FROM archlinux:latest

# Обновляем систему и устанавливаем пакеты
RUN pacman -Syu --noconfirm \
    cmake \
    git \
    curl \
    python \
    python-pip \
    ffmpeg \
    build-essential \
    libssl-dev \
    libcurl4-openssl-dev \
    pkg-config \
    zlib1g-dev \
    libopus-dev \
    libogg-dev \
    nodejs \
    npm

# Создаём рабочую директорию
WORKDIR /app

# Копируем файлы бота
COPY . /app

# Устанавливаем Python зависимости, если нужны
RUN pip install --no-cache-dir -r requirements.txt

# Запуск бота
CMD ["python", "restore.py"]
