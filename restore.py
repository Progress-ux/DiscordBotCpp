import subprocess
import time
import os

env = os.environ.copy()
env["LD_LIBRARY_PATH"] = "/home/container/library"

i = 0

subprocess.run(["chmod", "+x", "/home/container/bot"])
while True:
    print("Запускаем программу...")
    process = subprocess.Popen(["/home/container/bot"], env=env)

    
    # ждём, пока процесс завершится
    process.wait()
    i += 1

    if i > 5:
        print("Программа завершилась. Количество попыток закончилось")
        break

    print("Программа завершилась. Перезапуск через 5 секунд...")
    time.sleep(5)